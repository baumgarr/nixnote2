#ifndef FILEWATCHERMANAGER_H
#define FILEWATCHERMANAGER_H

#include <QObject>
#include <QList>

#include "watcher/filewatcher.h"

class FileWatcherManager : public QObject
{
    Q_OBJECT
private:
    QList<FileWatcher*> importKeep;
    QList<FileWatcher*> importDelete;

public:
    explicit FileWatcherManager(QObject *parent = 0);
    void reset();
    void setup();
    void dump();
    
signals:
    void fileImported(qint32 noteLid, qint32 resourceLid);
    
public slots:
    void signalImported(qint32 noteLid, qint32 resourceLid);
    
};

#endif // FILEWATCHERMANAGER_H
