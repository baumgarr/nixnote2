#ifndef FILEWATCHER_H
#define FILEWATCHER_H


#include <QFileSystemWatcher>
#include <QStringList>
#include <QFileInfo>

class FileWatcher : public QFileSystemWatcher
{
    Q_OBJECT
public:
    enum ScanType {
        ImportDelete = 0,
        ImportKeep = 1
    };
    explicit FileWatcher(QString dir, ScanType type, qint32 notebookLid, bool subdirs=true, QObject *parent = 0);

private:
    //void getDirectoryEntries(QStringList fileList, QString dir);
    QString dir;
    ScanType scanType;
    qint32 notebookLid;
    QList<QString> saveFiles;
    bool includeSubdirectories;
    void setupSubDirectories(QStringList &directories, QStringList &files, QString directory);
    void setupDirectory(QStringList &files, QString directory);
    void addDirectory(QString root);


signals:
    void fileImported(qint32 noteLid, qint32 ressourceLid);
    
public slots:
    void saveFile(QString file);
    void saveDirectory(QString dir);
    
};

#endif // FILEWATCHER_H
