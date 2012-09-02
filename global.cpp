#include "global.h"

#include <QString>
#include <QProxyStyle>


//******************************************
//* Global settings used by the program
//******************************************
Global::Global()
{
    FilterCriteria *criteria = new FilterCriteria();
    filterCriteria.push_back(criteria);
    filterPosition = 0;

    criteria->resetNotebook = true;
    criteria->resetTags = true;
    criteria->resetSavedSearch = true;
    criteria->resetAttribute = true;
    criteria->resetDeletedOnly = true;
    criteria->setDeletedOnly(false);
    criteria->resetContent = true;
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
    cryptCounter = 0;
    attachmentNameDelimeter = "------";
    username = "";
    password = "";
    connected = false;
    //server = "sandbox.evernote.com";
    server = "www.evernote.com";
}


// Return the path the program is executing under
QString Global::getProgramDirPath() {
    QString path(argv[0]);
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


// Utility function for case insensitive sorting
bool caseInsensitiveLessThan(const QString &s1, const QString &s2)
 {
     return s1.toLower() < s2.toLower();
 }


Global global;
