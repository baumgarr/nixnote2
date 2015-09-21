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

#include <string>
#include <limits.h>
#include <unistd.h>
#include <QWebSettings>
#include <QtGui/QDesktopWidget>
#include <QApplication>

// The following include is needed for demangling names on a backtrace
#include <cxxabi.h>
#include <execinfo.h>


//******************************************
//* Global settings used by the program
//******************************************
Global::Global()
{

    dbLock = new QReadWriteLock(QReadWriteLock::Recursive);
    listView = ListViewWide;
    FilterCriteria *criteria = new FilterCriteria();
    filterCriteria.push_back(criteria);
    filterPosition = 0;

    criteria->resetNotebook = true;
    criteria->resetTags = true;
    criteria->resetSavedSearch = true;
    criteria->resetAttribute = true;
    criteria->resetFavorite = true;
    criteria->resetDeletedOnly = true;
    criteria->setDeletedOnly(false);
    criteria->resetLid = true;
    criteria->resetSearchString = true;
    username = "";
    password = "";
    javaFound = false;
}


// Destructor
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
    shortcutKeys = new ShortcutKeys();
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
    QLOG_DEBUG() << "Shared memory key: " << key;
    sharedMemory = new QSharedMemory(key);


    settingsFile = fileManager.getHomeDirPath("") + "nixnote-"+QString::number(accountId)+".conf";

    settings = new QSettings(settingsFile, QSettings::IniFormat);

    this->forceNoStartMimized = startupConfig.forceNoStartMinimized;
    this->forceSystemTrayAvailable = startupConfig.forceSystemTrayAvailable;
    this->startupNewNote = startupConfig.startupNewNote;
    this->syncAndExit = startupConfig.syncAndExit;
    this->forceStartMinimized = startupConfig.forceStartMinimized;
    this->startupNote = startupConfig.startupNoteLid;
    startupConfig.accountId = accountId;
    accountsManager = new AccountsManager(startupConfig.accountId);
    enableIndexing = startupConfig.enableIndexing;

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
    disableUploads = settings->value("disableUploads", false).toBool();
    nonAsciiSortBug = settings->value("nonAsciiSortBug", false).toBool();
    settings->endGroup();

    setupDateTimeFormat();

    settings->beginGroup("Appearance");
    int countbehavior = settings->value("countBehavior", CountAll).toInt();
    if (countbehavior==1)
        countBehavior = CountAll;
    if (countbehavior==2)
        countBehavior = CountNone;
    pdfPreview = settings->value("showPDFs", true).toBool();
    defaultFont = settings->value("defaultFont","").toString();
    defaultFontSize = settings->value("defaultFontSize",0).toInt();
    defaultGuiFontSize = settings->value("defaultGuiFontSize", 0).toInt();
    defaultGuiFont = settings->value("defaultGuiFont","").toString();
    disableEditing = false;
    if (settings->value("disableEditingOnStartup",false).toBool() || startupConfig.disableEditing)
        disableEditing = true;
    settings->endGroup();

    if (defaultFont != "" && defaultFontSize > 0) {
        QWebSettings *settings = QWebSettings::globalSettings();
        settings->setFontFamily(QWebSettings::StandardFont, defaultFont);
        // QWebkit DPI is hard coded to 96. Hence, we calculate the correct
        // font size based on desktop logical DPI.
        settings->setFontSize(QWebSettings::DefaultFontSize,
            defaultFontSize * (QApplication::desktop()->logicalDpiX() / 96.0)
            );
    }

    settings->beginGroup("Appearance");
    QString theme = settings->value("themeName", "").toString();
    loadTheme(resourceList,theme);
    autoHideEditorToolbar = settings->value("autoHideEditorToolbar", true).toBool();
    settings->endGroup();

    minIndexInterval = 5000;
    maxIndexInterval = 120000;
    indexResourceCountPause=2;
    indexNoteCountPause=100;
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


void Global::setDeleteConfirmation(bool value) {
    settings->beginGroup("Appearance");
    settings->setValue("confirmDeletes", value);
    settings->endGroup();
}


// Should we confirm all deletes?
bool Global::confirmDeletes() {
    settings->beginGroup("Appearance");
    bool confirmDeletes = settings->value("confirmDeletes", true).toBool();
    settings->endGroup();
    return confirmDeletes;
}



// Should we display tag counts?  This is really just a stub for future changes
QString Global::tagBehavior() {
    return "Count";
}




// Append the filter criteria to the filterCriteria queue.
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



// Should we show the tray icon?
bool Global::showTrayIcon() {
    bool showTrayIcon;
    settings->beginGroup("Appearance");
    showTrayIcon = settings->value("showTrayIcon", false).toBool();
    settings->endGroup();
    return showTrayIcon;
}



// Should we minimize to the tray
bool Global::minimizeToTray() {
    bool minimizeToTray;
    settings->beginGroup("Appearance");
    minimizeToTray = settings->value("minimizeToTray", false).toBool();
    settings->endGroup();
    return minimizeToTray;
}



// Should we close to the tray?
bool Global::closeToTray() {
    bool showTrayIcon;
    settings->beginGroup("Appearance");
    showTrayIcon = settings->value("closeToTray", false).toBool();
    settings->endGroup();
    return showTrayIcon;
}



// Save the user request to minimize to the tray
void Global::setMinimizeToTray(bool value) {
    settings->beginGroup("SaveState");
    settings->setValue("minimizeToTray", value);
    settings->endGroup();
}



// Save the user's request to close to the tray
void Global::setCloseToTray(bool value) {
    settings->beginGroup("SaveState");
    settings->setValue("closeToTray", value);
    settings->endGroup();
}



// Save the position of a column in the note list.
void Global::setColumnPosition(QString col, int position) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnPosition-Wide");
    else
        settings->beginGroup("ColumnPosition-Narrow");
    settings->setValue(col, position);
    settings->endGroup();
}




// Save the with of a column in the note list
void Global::setColumnWidth(QString col, int width) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnWidth-Wide");
    else
        settings->beginGroup("ColumnWidth-Narrow");
    settings->setValue(col, width);
    settings->endGroup();
}



// Get the desired width for a given column
int Global::getColumnWidth(QString col) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnWidth-Wide");
    else
        settings->beginGroup("ColumnWidth-Narrow");
    int value = settings->value(col, -1).toInt();
    settings->endGroup();
    return value;
 }



// Get the position of a given column in the note list
int Global::getColumnPosition(QString col) {
    if (listView == ListViewWide)
        settings->beginGroup("ColumnPosition-Wide");
    else
        settings->beginGroup("ColumnPosition-Narrow");
    int value = settings->value(col, -1).toInt();
    settings->endGroup();
    return value;
 }



// Get the minimum recognition confidence.  Anything below this minimum will not be
// included in search results.
int Global::getMinimumRecognitionWeight() {
    settings->beginGroup("Search");
    int value = settings->value("minimumRecognitionWeight", 20).toInt();
    settings->endGroup();
    return value;
}



// Save the minimum recognition weight for an item to be included in a serch result
void Global::setMinimumRecognitionWeight(int weight) {
    settings->beginGroup("Search");
    settings->setValue("minimumRecognitionWeight", weight);
    settings->endGroup();
}




// Should we synchronize attachments?  Not really useful except in debugging
bool Global::synchronizeAttachments() {
    settings->beginGroup("Search");
    bool value = settings->value("synchronizeAttachments", true).toBool();
    settings->endGroup();
    return value;
}



// Should we synchronize attachments?  Not really useful except in debugging
void Global::setSynchronizeAttachments(bool value) {
    settings->beginGroup("Search");
    settings->setValue("synchronizeAttachments", value);
    settings->endGroup();
}



// get the last time we issued a reminder
qlonglong Global::getLastReminderTime() {
    settings->beginGroup("Reminders");
    qlonglong value = settings->value("lastReminderTime", 0).toLongLong();
    settings->endGroup();
    return value;
}


// Save the last time we issued a reminder.
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



// Get the default GUI font
QFont Global::getGuiFont(QFont f) {
    if (defaultGuiFont != "")
        f.setFamily(defaultGuiFont);
    if (defaultGuiFontSize > 0)
        f.setPointSize(defaultGuiFontSize);
    return f;
}


// Get a QIcon of in an icon theme
QIcon Global::getIconResource(QHash<QString,QString> &resourceList, QString key) {
    if (resourceList.contains(key) && resourceList[key].trimmed()!="")
        return QIcon(resourceList[key]);
    return QIcon(key);
}



// Get a QIcon in an icon theme
QIcon Global::getIconResource(QString key) {
    return this->getIconResource(resourceList, key);
}




// Get a QPixmap from an icon theme
QPixmap Global::getPixmapResource(QString key) {
    return this->getPixmapResource(resourceList, key);
}


// Get a QPixmap from an icon theme
QPixmap Global::getPixmapResource(QHash<QString,QString> &resourceList, QString key) {
    if (resourceList.contains(key) && resourceList[key].trimmed()!="")
        return QPixmap(resourceList[key]);
    return QPixmap(key);
}



// Load a theme into a resourceList.
void Global::loadTheme(QHash<QString, QString> &resourceList, QString theme) {
    resourceList.clear();
    if (theme.trimmed() == "")
        return;
    QFile systemTheme(fileManager.getProgramDirPath("theme.ini"));
    this->loadThemeFile(resourceList,systemTheme, theme);

    QFile userTheme(fileManager.getHomeDirPath("theme.ini"));
    this->loadThemeFile(resourceList, userTheme, theme);
}



// Load a theme from a given file
void Global::loadThemeFile(QFile &file, QString themeName) {
    this->loadThemeFile(resourceList, file, themeName);
}



// Load a theme from a given file
void Global::loadThemeFile(QHash<QString,QString> &resourceList, QFile &file, QString themeName) {
    if (!file.exists())
        return;
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    bool themeFound = false;
    QString themeHeader = "[" + themeName.trimmed() + "]";
    while(!in.atEnd()) {
        QString line = in.readLine();
        if (!line.startsWith("#")){
            if (line.startsWith("[") && themeHeader != line)
                themeFound = false;
            if (line.startsWith("[") && themeHeader == line)
                themeFound = true;
            if (themeFound && !line.startsWith("[") && line != "") {
                QStringList fields = line.split("=");
                if (fields.size() >= 2) {
                    QString key = fields[0].simplified();
                    QString value = fields[1].split("#").at(0).simplified();
                    QFile f(value);
                    if (f.exists()) {
                        resourceList.remove(":"+key);
                        resourceList.insert(":"+key,value);
                    }
                }
            }
        }
    }

    file.close();
}


// Get all available themes
QStringList Global::getThemeNames() {
    QStringList values;
    values.empty();
    QFile systemTheme(fileManager.getProgramDirPath("theme.ini"));
    this->getThemeNamesFromFile(systemTheme, values);
    QFile userTheme(fileManager.getHomeDirPath("theme.ini"));
        this->getThemeNamesFromFile(userTheme, values);
    if (!nonAsciiSortBug)
        qSort(values.begin(), values.end(), caseInsensitiveLessThan);

    return values;
}



// Get themes contained in a given file
void Global::getThemeNamesFromFile(QFile &file, QStringList &values) {
    if (!file.exists())
        return;
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine().simplified();
        if (line.startsWith("[")) {
            QString name = line.mid(1);
            name.chop(1);
            if (name.simplified() != "") {
                if (!values.contains(name, Qt::CaseInsensitive)) {
                    values.append(name);
                }
            }
        }
    }

    file.close();
}



// Get the full path of a resource in a theme file
QString Global::getResourceFileName(QHash<QString,QString> &resourceList, QString key) {
        if (resourceList.contains(key) && resourceList[key].trimmed()!="")
            return resourceList[key];

        // If we have a default resource
        QString fileName = key.remove(":");
        return fileManager.getImageDirPath("")+fileName;
}






// save the proxy address
void Global::setProxyHost(QString proxy) {
    settings->beginGroup("Proxy");
    settings->setValue("hostName", proxy);
    settings->endGroup();
}


// save the port for the proxy server
void Global::setProxyPort(int port) {
    settings->beginGroup("Proxy");
    settings->setValue("port", port);
    settings->endGroup();
}

// Save the proxy password
void Global::setProxyPassword(QString password) {
    settings->beginGroup("Proxy");
    settings->setValue("password", password);
    settings->endGroup();
}


// Save the proxy userid
void Global::setProxyUserid(QString userid){
    settings->beginGroup("Proxy");
    settings->setValue("userid", userid);
    settings->endGroup();
}

// get the proxy  hostname
QString Global::getProxyHost() {
    settings->beginGroup("Proxy");
    QString value = settings->value("hostName", "").toString();
    settings->endGroup();
    return value;
}

// Get the proxy port number
int Global::getProxyPort() {
    settings->beginGroup("Proxy");
    int value = settings->value("port", 0).toInt();
    settings->endGroup();
    return value;
}

// Get the proxy password
QString Global::getProxyPassword() {
    settings->beginGroup("Proxy");
    QString value = settings->value("password", "").toString();
    settings->endGroup();
    return value;
}

// Get the proxy userid
QString Global::getProxyUserid() {
    settings->beginGroup("Proxy");
    QString value = settings->value("userid", "").toString();
    settings->endGroup();
    return value;
}

// Get the proxy userid
void Global::setProxyEnabled(bool value) {
    settings->beginGroup("Proxy");
    settings->setValue("enabled", value);
    settings->endGroup();
}

// Get the proxy userid
bool Global::isProxyEnabled() {
    settings->beginGroup("Proxy");
    bool value = settings->value("enabled", false).toBool();
    settings->endGroup();
    return value;
}



// Mouse middle click actions
void Global::setMiddleClickAction(int value) {
    settings->beginGroup("Appearance");
    settings->setValue("mouseMiddleClickOpen", value);
    settings->endGroup();
}

int Global::getMiddleClickAction() {
   settings->beginGroup("Appearance");
   int value = settings->value("mouseMiddleClickOpen", 0).toInt();
   settings->endGroup();
   return value;
}



bool Global::newNoteFocusToTitle() {
    settings->beginGroup("Appearance");
    bool returnValue = settings->value("newNoteFocusOnTitle", false).toBool();
    settings->endGroup();
    return returnValue;
}

void Global::setNewNoteFocusToTitle(bool focus) {
    settings->beginGroup("Appearance");
    settings->setValue("newNoteFocusOnTitle", focus);
    settings->endGroup();
}


void Global::stackDump(int max) {
    void *array[30];
    size_t size;
    QLOG_ERROR() << "***** Dumping stack *****";

    // get void*'s for all entries on the stack
    size = backtrace(array, 30);
    char **messages = backtrace_symbols(array, size);

    if (max > 0)
        size = max+1;  // We add one here because we always skip the first thing on the stack (this function).
    for (size_t i = 1; i < size && messages != NULL; ++i)
    {
        char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

        // find parantheses and +address offset surrounding mangled name
        for (char *p = messages[i]; *p; ++p)
        {
            if (*p == '(')
            {
                mangled_name = p;
            }
            else if (*p == '+')
            {
                offset_begin = p;
            }
            else if (*p == ')')
            {
                offset_end = p;
                break;
            }
        }

        // if the line could be processed, attempt to demangle the symbol
        if (mangled_name && offset_begin && offset_end &&
            mangled_name < offset_begin)
        {
            *mangled_name++ = '\0';
            *offset_begin++ = '\0';
            *offset_end++ = '\0';

            int status;
            char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

            // if demangling is successful, output the demangled function name
            if (status == 0)
            {
               QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i] << " : "
                          << real_name << "+" << offset_begin << offset_end;

            }
            // otherwise, output the mangled function name
            else
            {
                QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i] << " : "
                          << mangled_name << "+" << offset_begin << offset_end;
            }
            free(real_name);
        }
        // otherwise, print the whole line
        else
        {
            QLOG_ERROR() << "[bt]: (" << i << ") " << messages[i];
        }
    }

    free(messages);
    QLOG_ERROR() << "**** Stack dump complete *****";
}









Global global;
