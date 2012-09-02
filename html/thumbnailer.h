#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QtWebKit>
#include <QObject>

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

using namespace apache::thrift;
using namespace evernote::edam;
using namespace std;



using namespace std;

class Thumbnailer : public QObject
{
    Q_OBJECT

public:
    QWebPage page;

    Thumbnailer();
    void setNote(qint32 lid, Note n);

signals:
    void finished();

private slots:
    void render();

private:
    qint32 noteLid;

};

#endif // THUMBNAILER_H
