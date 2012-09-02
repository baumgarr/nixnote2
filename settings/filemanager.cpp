#include "filemanager.h"
#include "global.h"
#include <iostream>
#include <cstdlib>


//*******************************************
//* This class is used to find the location
//* of various files & directories
//*******************************************

using namespace std;

FileManager::FileManager() {
}

void FileManager::setup(QString homeDirPath, QString programDirPath) {

    homeDir.setPath(toPlatformPathSeparator(homeDirPath));
    programDir.setPath(toPlatformPathSeparator(programDirPath));
    createDirOrCheckWriteable(homeDir);
    this->homeDirPath = slashTerminatePath(homeDir.path());
    this->programDirPath = slashTerminatePath(programDir.path());

    // Read only files that everyone uses
    imagesDir.setPath(programDirPath+"images");
    checkExistingReadableDir(imagesDir);
    imagesDirPath = slashTerminatePath(imagesDir.path());

    spellDir.setPath(programDirPath+"spell");
    checkExistingReadableDir(spellDir);
    spellDirPath = slashTerminatePath(spellDir.path());

    xmlDir.setPath(programDirPath+"xml");
    checkExistingReadableDir(xmlDir);

    translateDir.setPath(programDirPath+"translations");
    checkExistingReadableDir(translateDir);
    translateDirPath= slashTerminatePath(translateDir.path());


    // Read/write directories that only we use
    logsDir.setPath(homeDirPath+"logs");
    createDirOrCheckWriteable(logsDir);

    tmpDir.setPath(homeDirPath+"tmp");
    createDirOrCheckWriteable(tmpDir);
    tmpDirPath = slashTerminatePath(tmpDir.path());

    dbDir.setPath(homeDirPath+"db");
    createDirOrCheckWriteable(dbDir);
    dbDirPath = slashTerminatePath(dbDir.path());

    dbaDir.setPath(homeDirPath+"db/dba");
    createDirOrCheckWriteable(dbaDir);
    dbaDirPath = slashTerminatePath(dbaDir.path());

}



QString FileManager::toPlatformPathSeparator(QString relativePath) {
    return relativePath;
}



/*************************************************/
/* Given a path, append either a / or a \ to     */
/* form a fully qualified path                   */
/*************************************************/
QString FileManager::slashTerminatePath(QString path) {
    if (!path.endsWith(QDir::separator())) {
        return path + QDir::separator();
    }
    return path;
}


/*************************************************/
/* Delete files in a directory.  This is used    */
/* to cleanup temporary files.                   */
/*************************************************/
void FileManager::deleteTopLevelFiles(QDir dir, bool exitOnFail) {
    dir.setFilter(QDir::Files);
    QStringList list = dir.entryList();
    for (qint32 i=0; i<list.size(); i++) {
        QFile f(list.at(i));
        if (!f.remove() && exitOnFail) {
            cout << "Error deleting file '" +f.fileName().toStdString() <<
                    "'. Aborting program";
            exit(16);
        }
    }
}


/*************************************************/
/* Create a directory if it doesn't exist.       */
/*************************************************/
void FileManager::createDirOrCheckWriteable(QDir dir) {

    if (!dir.exists()) {
         if (!dir.mkdir(dir.path())) {
             QLOG_FATAL() << "Failed to create directory '" << dir.path() << "'.  Aborting program.";
             exit(16);
         }
    } else {
        if (!dir.isReadable()) {
            QLOG_FATAL() << "Directory '" + dir.path() + "' does not have read permission.  Aborting program.";
            exit(16);
        }
    }
}


/**************************************************/
/* Check that an existing directory is readable.  */
/**************************************************/
void FileManager::checkExistingReadableDir(QDir dir) {
    if (!dir.isReadable()) {
            QLOG_FATAL() << "Directory '" + dir.path() + "' does not have read permission.  Aborting program.";
            exit(16);
    }
    if (!dir.exists()) {
         QLOG_FATAL() << "Directory '" + dir.path() + "' does not exist.  Aborting program";
         exit(16);
    }
}



/**************************************************/
/* Check that an existing directory is writable.  */
/**************************************************/
void FileManager::checkExistingWriteableDir(QDir dir) {
    this->checkExistingReadableDir(dir);
}









QDir FileManager::getProgramDirFile(QString relativePath) {
    return QDir(programDir.dirName() + toPlatformPathSeparator(relativePath));
}

QString FileManager::getProgramDirPath(QString relativePath) {
    return programDirPath + toPlatformPathSeparator(relativePath);
}
QDir FileManager::getHomeDirFile(QString relativePath) {
    return QDir(homeDir.dirName() + toPlatformPathSeparator(relativePath));
}
QString FileManager::getHomeDirPath(QString relativePath) {
    return homeDirPath + toPlatformPathSeparator(relativePath);
}
QString FileManager::getSpellDirPath(QString relativePath) {
    return spellDirPath + toPlatformPathSeparator(relativePath);
}
QDir FileManager::getSpellDirFile(QString relativePath) {
    return QDir(spellDir.dirName() + toPlatformPathSeparator(relativePath));
}
QString FileManager::getSpellDirPath() {
    return spellDirPath;
}
QDir FileManager::getSpellDirFileUser(QString relativePath) {
    return spellDirPathUser + toPlatformPathSeparator(relativePath);
}
QString FileManager::getSpellDirPathUser() {
    return spellDirPathUser;
}
QString FileManager::getDbDirPath(QString relativePath) {
    return dbDirPath + toPlatformPathSeparator(relativePath);
}
QDir FileManager::getImageDirFile(QString relativePath) {
    return QDir(imagesDir.dirName()+ toPlatformPathSeparator(relativePath));
}
QString FileManager::getImageDirPath(QString relativePath) {
    return imagesDirPath + toPlatformPathSeparator(relativePath);
}
QDir FileManager::getLogsDirFile(QString relativePath) {
    return QDir(logsDir.dirName() + toPlatformPathSeparator(relativePath));
}
QString FileManager::getQssDirPath(QString relativePath) {
    return qssDirPath + toPlatformPathSeparator(relativePath);
}
QString FileManager::getQssDirPathUser(QString relativePath) {
    return qssDirPath + toPlatformPathSeparator(relativePath);
}
QString FileManager::getTmpDirPath() {
    return tmpDirPath;
}
QString FileManager::getTmpDirPath(QString relativePath) {
    return tmpDirPath + toPlatformPathSeparator(relativePath);
}
QString FileManager::getTmpDirPathSpecialChar(QString relativePath) {
    return tmpDirPath + toPlatformPathSeparator(relativePath).replace("#", "%23");
}
QString FileManager::getDbaDirPath() {
    return dbaDirPath;
}
QString FileManager::getDbaDirPath(QString relativePath) {
    return dbaDirPath + toPlatformPathSeparator(relativePath);
}
QString FileManager::getDbaDirPathSpecialChar(QString relativePath) {
    return dbaDirPath + toPlatformPathSeparator(relativePath).replace("#", "%23");
}
QDir FileManager::getXMLDirFile(QString relativePath) {
    return QDir(xmlDir.dirName() + toPlatformPathSeparator(relativePath));
}
QString FileManager::getTranslateFilePath(QString relativePath) {
    return translateDirPath + toPlatformPathSeparator(relativePath);
}
void FileManager::purgeResDirectory(bool exitOnFail) {
    this->deleteTopLevelFiles(tmpDir.dirName(), exitOnFail);
}








