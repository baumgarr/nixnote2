#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QtWebKit>
#include <QObject>

#include "html/noteformatter.h"

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
    QString contents;
    Thumbnailer();
    void setNote(Note n);
    NoteFormatter formatter;
    void render(qint32 lid, QString contents);

signals:

private slots:


private:
    QProcess imgProcess;

};

#endif // THUMBNAILER_H
