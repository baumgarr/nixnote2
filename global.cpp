/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

#include "global.h"

//#include <QString>
//#include <QProxyStyle>
#include <string>
#include <limits.h>
#include <unistd.h>


//******************************************
//* Global settings used by the program
//******************************************
Global::Global()
{

    listView = ListViewWide;
    FilterCriteria *criteria = new FilterCriteria();
    shortcutKeys = new ShortcutKeys();
    filterCriteria.push_back(criteria);
    filterPosition = 0;

    criteria->resetNotebook = true;
    criteria->resetTags = true;
    criteria->resetSavedSearch = true;
    criteria->resetAttribute = true;
    criteria->resetDeletedOnly = true;
    criteria->setDeletedOnly(false);
    criteria->resetLid = true;
    criteria->resetSearchString = true;
    username = "";
    password = "";
    javaFound = false;
}


Global::~Global() {
    FilterCriteria *criteria;
    for (int i=0; i<filterCriteria.size(); i++) {
        criteria = filterCriteria[i];
        if (criteria != NULL)
            delete criteria;
    }
}

//Initial global settings setup
void Global::setup(StartupConfig startupConfig) {
    fileManager.setup(startupConfig.homeDirPath, startupConfig.programDirPath, startupConfig.accountId);
    QString settingsFile = fileManager.getHomeDirPath("") + "nixnote.conf";

    globalSettings = new QSettings(settingsFile, QSettings::IniFormat);
    int accountId = startupConfig.accountId;
    if (accountId <=0) {
        globalSettings->beginGroup("SaveState");
        accountId = globalSettings->value("lastAccessedAccount", 1).toInt();
        globalSettings->endGroup();
    }

    QString key = "1b73cc55-9a2f-441b-877a-ca1d0131cd2"+
            QString::number(accountId);
    sharedMemory = new QSharedMemory(key);


    settingsFile = fileManager.getHomeDirPath("") + "nixnote-"+QString::number(accountId)+".conf";

    settings = new QSettings(settingsFile, QSettings::IniFormat);

    startupConfig.accountId = accountId;
    accountsManager = new AccountsManager(startupConfig.accountId);

    cryptCounter = 0;
    attachmentNameDelimeter = "------";
    username = "";
    password = "";
    connected = false;

    server = accountsManager->getServer();

    // Cleanup any temporary files from the last time
    QDir myDir(fileManager.getTmpDirPath());
    QStringList list = myDir.entryList();
    for (int i=0; i<list.size(); i++) {
        if (list[i] != "." && list[i] != "..") {
            QString file = fileManager.getTmpDirPath()+ list[i];
            myDir.remove(file);
        }
    }

    settings->beginGroup("Debugging");
    disableUploads = settings->value("disableUploads", true).toBool();
    settings->endGroup();

    setupDateTimeFormat();

    settings->beginGroup("Appearance");
    pdfPreview = settings->value("showPDFs", true).toBool();
    settings->endGroup();
}


// Return the path the program is executing under
// If we are in /usr/bin, then we need to return /usr/share/nixnote2.
// This is because we want to find other paths (like images).  This
// allows for users to run it out of a non-path location.
QString Global::getProgramDirPath() {
    QString path = QCoreApplication::applicationDirPath();
    if (path == "/usr/bin")
        return "/usr/share/nixnote2";
    if (path == "/usr/share/bin")
        return "/usr/share/nixnote2";
    if (path == "/usr/local/bin")
        return "/usr/share/nixnote2";
    return path;
}

bool Global::confirmDeletes() {
    return true;
}


QString Global::tagBehavior() {
    return "Count";
}


void Global::appendFilter(FilterCriteria *criteria) {
    // First, find out if we're already viewing history.  If we are we
    // chop off the end of the history & start a new one
    if (filterPosition+1 < filterCriteria.size()) {
        int position = filterPosition;
        while (position+1 < filterCriteria.size())
            delete filterCriteria.takeAt(position);
    }

    filterCriteria.append(criteria);
}


bool Global::showTrayIcon() {
    bool showTrayIcon;
    settings->beginGroup("Appearance");
    showTrayIcon = settings->value("showTrayIcon", false).toBool();
    settings->endGroup();
    return showTrayIcon;
}


bool Global::minimizeToTray() {
    bool showTrayIcon;
    settings->beginGroup("Appearance");
    showTrayIcon = settings->value("minimizeToTray", false).toBool();
    settings->endGroup();
    return showTrayIcon;
}


bool Global::closeToTray() {
    bool showTrayIcon;
    settings->beginGroup("Appearance");
    showTrayIcon = settings->value("closeToTray", false).toBool();
    settings->endGroup();
    return showTrayIcon;
}


void Global::setMinimizeToTray(bool value) {
    settings->beginGroup("SaveState");
    settings->setValue("minimizeToTray", value);
    settings->endGroup();
}


void Global::setCloseToTray(bool value) {
    settings->beginGroup("SaveState");
    settings->setValue("closeToTray", value);
    settings->endGroup();
}




void Global::setColumnPosition(QString col, int position) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnPosition-Wide");
    else
        settings->beginGroup("ColumnPosition-Narrow");
    settings->setValue(col, position);
    settings->endGroup();
}





void Global::setColumnWidth(QString col, int width) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnWidth-Wide");
    else
        settings->beginGroup("ColumnWidth-Narrow");
    settings->setValue(col, width);
    settings->endGroup();
}


int Global::getColumnWidth(QString col) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnWidth-Wide");
    else
        settings->beginGroup("ColumnWidth-Narrow");
    int value = settings->value(col, -1).toInt();
    settings->endGroup();
    return value;
 }



int Global::getColumnPosition(QString col) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnPosition-Wide");
    else
        settings->beginGroup("ColumnPosition-Narrow");
    int value = settings->value(col, -1).toInt();
    settings->endGroup();
    return value;
 }


int Global::getMinimumRecognitionWeight() {
    settings->beginGroup("Search");
    int value = settings->value("minimumRecognitionWeight", 20).toInt();
    settings->endGroup();
    return value;
}



void Global::setMinimumRecognitionWeight(int weight) {
    settings->beginGroup("Search");
    settings->setValue("minimumRecognitionWeight", weight);
    settings->endGroup();
}


qlonglong Global::getLastReminderTime() {
    settings->beginGroup("Reminders");
    qlonglong value = settings->value("lastReminderTime", 0).toLongLong();
    settings->endGroup();
    return value;
}

void Global::setLastReminderTime(qlonglong value) {
    settings->beginGroup("Reminders");
    settings->setValue("lastReminderTime", value);
    settings->endGroup();
}


// Setup the default date & time formatting
void Global::setupDateTimeFormat() {
    QString datefmt;
    QString timefmt;

    enum DateFormat {
        MMddyy = 1,
        MMddyyyy = 2,
        Mddyyyy = 3,
        Mdyyyy = 4,
        ddMMyy = 5,
        dMyy = 6,
        ddMMyyyy = 7,
        dMyyyy = 8,
        yyyyMMdd = 9,
        yyMMdd = 10
    };
    enum TimeFormat {
        HHmmss = 1,
        HHMMSSa = 2,
        HHmm = 3,
        HHmma = 4,
        hhmmss = 5,
        hhmmssa = 6,
        hmmssa = 7,
        hhmm = 8,
        hhmma = 9,
        hmma = 10
    };

    settings->beginGroup("Locale");
    int date = settings->value("dateFormat", MMddyy).toInt();
    int time = settings->value("timeFormat", HHmmss).toInt();
    settings->endGroup();

    datefmt = "MM/dd/yy";
    switch (date) {
    case MMddyy:
        datefmt = "MM/dd/yy";
        break;
    case MMddyyyy:
        datefmt = "MM/dd/yyyy";
        break;
    case Mddyyyy:
        datefmt = "M/dd/yyyy";
        break;
    case Mdyyyy:
        datefmt = "M/d/yyyy";
        break;
    case ddMMyy:
        datefmt = "dd/MM/yy";
        break;
    case dMyy:
        datefmt = "d/M/yy";
        break;
    case ddMMyyyy:
        datefmt = "dd/MM/yyyy";
        break;
    case dMyyyy:
        datefmt = "d/M/yyyy";
        break;
    case yyyyMMdd:
        datefmt = "yyyy-MM-dd";
        break;
    case yyMMdd:
        datefmt = "yy-MM-dd";
        break;
    }

    timefmt = "HH:mm:ss";
    switch (time) {
    case HHmmss:
        timefmt = "HH:mm:ss";
        break;
    case HHMMSSa:
        timefmt = "HH:MM:SS a";
        break;
    case HHmm:
        timefmt = "HH:mm";
        break;
    case HHmma:
        timefmt = "HH:mm a";
        break;
    case hhmmss:
        timefmt = "hh:mm:ss";
        break;
    case hhmmssa:
        timefmt = "hh:mm:ss a";
        break;
    case hmmssa:
        timefmt = "h:mm:ss a";
        break;
    case hhmm:
        timefmt = "hh:mm";
        break;
    case hhmma:
        timefmt = "hh:mm a";
        break;
    case hmma:
        timefmt = "h:mm a";
        break;
    }

    this->dateFormat = datefmt;
    this->timeFormat = timefmt;
}



// Utility function for case insensitive sorting
bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
 {
     return s1.toLower() < s2.toLower();
 }

QString Global::getWindowIcon() {
    settings->beginGroup("Appearance");
    QString value = settings->value("windowIcon", ":windowIcon0.png").toString();
    settings->endGroup();
    return value;
}



Global global;
