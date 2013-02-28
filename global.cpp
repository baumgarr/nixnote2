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

#include <QString>
#include <QProxyStyle>


//******************************************
//* Global settings used by the program
//******************************************
Global::Global()
{
    listView = ListViewWide;
    sharedMemory = new QSharedMemory("1b73cc55-9a2f-441b-877a-ca1d0131cd21");
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
    //jvm.create_jvm();
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
    globalSettings->beginGroup("SaveState");
    int accountId = globalSettings->value("lastAccessedAccount", 1).toInt();
    globalSettings->endGroup();

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
}


// Return the path the program is executing under
QString Global::getProgramDirPath() {
    QString path(QFileInfo(this->argv[0]).absoluteFilePath());
    int i = path.lastIndexOf("/");
    path.chop(path.length()-i);
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

// Utility function for case insensitive sorting
bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
 {
     return s1.toLower() < s2.toLower();
 }



Global global;
