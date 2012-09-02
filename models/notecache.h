#ifndef NOTECACHE_H
#define NOTECACHE_H

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include <QObject>


using namespace evernote::edam  ;


class NoteCache : public QObject
{
    Q_OBJECT
public:
    explicit NoteCache(QObject *parent = 0);
    QByteArray noteContent;
    bool isReadOnly;
    bool isContentReadOnly;

signals:

public slots:

};

#endif // NOTECACHE_H
