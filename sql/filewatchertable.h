#ifndef FILEWATCHERTABLE_H
#define FILEWATCHERTABLE_H

#include <QObject>
#include "watcher/filewatcher.h"

//*************************************
//* This table is used to store
//* config values & things about how
//* to run the program.
//*************************************

// Define key types
#define FILE_WATCHER_DIR 100   // The base directory
#define FILE_WATCHER_TYPE 101  // ImportDelete or ImportKeep
#define FILE_WATCHER_NOTEBOOK 102 // The notebook to import to
#define FILE_WATCHER_SUBDIRS 103  // Include subdirectories?

class FileWatcherTable : public QObject
{
    Q_OBJECT
public:
    explicit FileWatcherTable(QObject *parent = 0);
    qint32 addEntry(qint32 lid, QString baseDir, FileWatcher::ScanType type, qint32 notebookLid, bool includeSubdirs);
    void get(qint32 lid, QString &baseDir, FileWatcher::ScanType &type, qint32 &notebookLid, bool &includeSubdirs);
    qint32 findLidByDir(QString baseDir);
    qint32 getAll(QList<qint32> &lids);
    void expunge(qint32 lid);
    
signals:
    
public slots:
    
};

#endif // FILEWATCHERTABLE_H
