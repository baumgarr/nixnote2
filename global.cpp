#include "global.h"

#include <QString>
#include <QProxyStyle>


//******************************************
//* Global settings used by the program
//******************************************
Global::Global()
{
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



// Utility function for case insensitive sorting
bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
 {
     return s1.toLower() < s2.toLower();
 }



Global global;
