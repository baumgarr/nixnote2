#include "filewatchermanager.h"
#include "sql/filewatchertable.h"

#include "global.h"

extern Global global;

FileWatcherManager::FileWatcherManager(QObject *parent) :
    QObject(parent)
{
}


void FileWatcherManager::reset() {
    for (int i=importDelete.size()-1; i>=0; i--) {
        FileWatcher *fw = importDelete.takeAt(i);
        delete fw;
    }
    for (int i=importKeep.size()-1; i>=0; i--) {
        FileWatcher *fw = importKeep.takeAt(i);
        delete fw;
    }
}

void FileWatcherManager::signalImported(qint32 noteLid, qint32 resourceLid) {
    emit fileImported(noteLid, resourceLid);
}


void FileWatcherManager::setup() {
    this->reset();

    QList<qint32> lids;
    FileWatcherTable ft;
    ft.getAll(lids);

    for (int i=0; i<lids.size(); i++) {
        FileWatcher::ScanType type;
        QString dir;
        qint32 notebookLid;
        bool includeSubdirs;
        ft.get(lids[i], dir, type, notebookLid, includeSubdirs);
        FileWatcher *fw = new FileWatcher(dir, type, notebookLid, includeSubdirs);
        if (type == FileWatcher::ImportDelete)
           importDelete.append(fw);
        else
            importKeep.append(fw);
        connect(fw, SIGNAL(fileImported(qint32,qint32)), this, SLOT(signalImported(qint32,qint32)));
    }
}


void FileWatcherManager::dump() {
    for (int i=0; i<importKeep.size(); i++) {
        FileWatcher *fw = importKeep[i];
        for (int j=0; j<fw->directories().size(); j++) {
            QLOG_DEBUG() << fw->directories().at(j);
        }
    }
}
