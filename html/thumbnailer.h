#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <QtWebKit>
#include <QObject>
#include <QSqlDatabase>

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

private:
    QSqlDatabase *db;

public:
    QWebPage page;
    Thumbnailer(QSqlDatabase *db);
    void render(qint32 lid, QString contents);
    qint32 lid;

signals:

private slots:

public slots:
    void pageReady(bool ok);

};

#endif // THUMBNAILER_H
