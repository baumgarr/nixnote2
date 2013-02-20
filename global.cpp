#include "global.h"

#include <QString>
#include <QProxyStyle>


//******************************************
//* Global settings used by the program
//******************************************
Global::Global()
{
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
    fileManager.setup(startupConfig.homeDirPath, startupConfig.programDirPath);
    QString settingsFile = fileManager.getHomeDirPath("") + "nixnote.conf";
    settings = new QSettings(settingsFile, QSettings::IniFormat);

    cryptCounter = 0;
    attachmentNameDelimeter = "------";
    username = "";
    password = "";
    connected = false;

    settings->beginGroup("Server");
    server = settings->value("host", "www.evernote.com").toString();
    settings->endGroup();

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



// Utility function for case insensitive sorting
bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
 {
     return s1.toLower() < s2.toLower();
 }



Global global;
