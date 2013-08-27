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
    QTimer timer;
    int minTime;
    int maxTime;


public:
    QWebPage *page;
    Thumbnailer(QSqlDatabase *db);
    ~Thumbnailer();
    void render(qint32 lid);
    qint32 lid;
    bool idle;
    void capturePage(QWebPage *page);
    void startTimer(int minSeconds, int maxSeconds);

signals:

private slots:

public slots:
    void pageReady(bool ok);
    void generateNextThumbnail();

};

#endif // THUMBNAILER_H
