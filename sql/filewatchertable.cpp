#include "filewatchertable.h"
#include "sql/configstore.h"
#include <QSqlQuery>

FileWatcherTable::FileWatcherTable(QObject *parent) :
    QObject(parent)
{
}


qint32 FileWatcherTable::addEntry(qint32 lid, QString baseDir, FileWatcher::ScanType type, qint32 notebookLid, bool includeSubdirs) {
    if (lid == 0) {
        ConfigStore cs;
        lid = cs.incrementLidCounter();
    }
    QSqlQuery sql;
    sql.prepare("Insert Into DataStore (lid, key, data) values (:lid, :key, :data)");
    sql.bindValue(":lid", lid);
    sql.bindValue(":key", FILE_WATCHER_DIR);
    sql.bindValue(":data", baseDir);
    sql.exec();

    sql.bindValue(":lid", lid);
    sql.bindValue(":key", FILE_WATCHER_TYPE);
    sql.bindValue(":data", type);
    sql.exec();

    sql.bindValue(":lid", lid);
    sql.bindValue(":key", FILE_WATCHER_NOTEBOOK);
    sql.bindValue(":data", notebookLid);
    sql.exec();

    sql.bindValue(":lid", lid);
    sql.bindValue(":key", FILE_WATCHER_SUBDIRS);
    sql.bindValue(":data", includeSubdirs);
    sql.exec();


    return lid;
}

void FileWatcherTable::get(qint32 lid, QString &baseDir, FileWatcher::ScanType &type, qint32 &notebookLid, bool &includeSubdirs) {
    QSqlQuery sql;
    sql.prepare("Select key, data from DataStore where lid=:lid");
    sql.bindValue(":lid", lid);
    sql.exec();

    while(sql.next()) {
        int key = sql.value(0).toInt();
        switch(key) {
        case FILE_WATCHER_DIR :
            baseDir = sql.value(1).toString();
            break;
        case FILE_WATCHER_NOTEBOOK :
            notebookLid = sql.value(1).toInt();
            break;
        case FILE_WATCHER_SUBDIRS :
            includeSubdirs = sql.value(1).toBool();
            break;
        case FILE_WATCHER_TYPE : {
            int value = sql.value(1).toInt();
            if (value == FileWatcher::ImportDelete)
                type = FileWatcher::ImportDelete;
            else
                type = FileWatcher::ImportKeep;
         }
         default :
            break;
        }
    }
}

qint32 FileWatcherTable::findLidByDir(QString baseDir) {
    QSqlQuery sql;
    sql.prepare("Select lid from DataStore where key=:key and data=:data");
    sql.bindValue(":key", FILE_WATCHER_DIR);
    sql.bindValue(":data", baseDir);
    sql.exec();
    if (sql.next())
        return sql.value(0).toInt();
    else
        return -1;
}


qint32 FileWatcherTable::getAll(QList<qint32> &lids) {
    QSqlQuery sql;
    sql.prepare("Select lid from DataStore where key=:key");
    sql.bindValue(":key", FILE_WATCHER_DIR);
    sql.exec();
    lids.clear();
    while(sql.next())
        lids.append(sql.value(0).toInt());
    return lids.size();
}

void FileWatcherTable::expunge(qint32 lid) {
    QSqlQuery sql;
    sql.prepare("Delete from datastore where lid=:lid");
    sql.bindValue(":lid", lid);
    sql.exec();
}

