#ifndef FILEMANAGER_H
#define FILEMANAGER_H


#include <QObject>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <exception>

//************************************************
//* This class is used to retrieve the
//* location of certain directories.
//************************************************
class FileManager : public QObject
{
    Q_OBJECT

private:

    //QRegExp ALL_PATH_SEPARATORS_REGEX = "[/\\\\]";

    QString programDirPath;
    QDir programDir;

    QString  homeDirPath;
    QDir homeDir;

    QString dbDirPath;
    QDir dbDir;

    QDir logsDir;

    QString imagesDirPath;
    QDir imagesDir;

    QString spellDirPath;
    QDir spellDir;

    QString spellDirPathUser;
    QDir spellDirUser;

    //QString qssDirPath;
    //QDir qssDir;

    //QString qssDirPathUser;
    //QDir qssDirUser;

    QString tmpDirPath;
    QDir tmpDir;

    QString dbaDirPath;
    QDir dbaDir;

    //QDir xmlDir;

    QString translateDirPath;
    QDir translateDir;

    QString toPlatformPathSeparator(QString relativePath);
    QString slashTerminatePath(QString path);
    void deleteTopLevelFiles(QDir dir, bool exitOnFail);
    void createDirOrCheckWriteable(QDir dir);
    void checkExistingReadableDir(QDir dir);
    void checkExistingWriteableDir(QDir dir);


public:
    FileManager();
    void setup(QString homeDirPath, QString programDirPath, int id);
    QDir getProgramDirFile(QString relativePath);
    QString getProgramDirPath(QString relativePath);
    QDir getHomeDirFile(QString relativePath);
    QString getHomeDirPath(QString relativePath);
    QString getSpellDirPath(QString relativePath);
    QDir getSpellDirFile(QString relativePath);
    QString getSpellDirPath();
    QDir getSpellDirFileUser(QString relativePath);
    QString getSpellDirPathUser();
    QString getDbDirPath(QString relativePath);
    QString getDbaDirPath();
    QString getDbaDirPath(QString relativePath);
    QString getDbaDirPathSpecialChar(QString relativePath);
    QDir getImageDirFile(QString relativePath);
    QString getImageDirPath(QString relativePath);
    QDir getLogsDirFile(QString relativePath);
    //QString getQssDirPath(QString relativePath);
    QString getQssDirPathUser(QString relativePath);
    QString getTmpDirPath();
    QString getTmpDirPath(QString relativePath);
    QString getTmpDirPathSpecialChar(QString relativePath);
    //QDir getXMLDirFile(QString relativePath);
    QString getTranslateFilePath(QString relativePath);
    void purgeResDirectory(bool exitOnFail);


signals:

public slots:

};

#endif // FILEMANAGER_H


