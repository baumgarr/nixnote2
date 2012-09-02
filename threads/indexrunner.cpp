#include "indexrunner.h"
#include "global.h"
#include "sql/notetable.h"
#include "sql/resourcetable.h"
#include <QTextDocument>
#include <QtXml>


IndexRunner::IndexRunner()
{
    moveToThread(this);
    keepRunning = true;
    pauseIndexing = false;
}

IndexRunner::~IndexRunner() {
}




void IndexRunner::run() {
    hammer = new Thumbnailer();
    QLOG_DEBUG() << "Starting IndexRunner";
    indexTimer = new QTimer();
    indexTimer->setInterval(4000);
    connect(indexTimer, SIGNAL(timeout()), this, SLOT(index()));
    indexTimer->start();
    textDocument = new QTextDocument();
    exec();
}


void IndexRunner::index() {

    indexTimer->stop();

    QList<qint32> lids;
    NoteTable noteTable;
    ResourceTable resourceTable;

    if (keepRunning && noteTable.getIndexNeeded(lids) > 0 && !pauseIndexing) {
        QLOG_DEBUG() << "Unindexed Notes found: " << lids.size();


        for (qint32 i=0; i<lids.size() && keepRunning && !pauseIndexing; i++) {
            QSqlQuery sql;
            sql.prepare("Delete from SearchIndex where lid=:lid");
            sql.bindValue(":lid", lids.at(i));
            sql.exec();

            Note n;
            noteTable.get(n, lids.at(i), false, false);
            indexNote(lids.at(i),n);
            //hammer->setNote(lids.at(i), n);
            noteTable.setIndexNeeded(lids.at(i), false);
        }
    }

    lids.empty();
    if (keepRunning && resourceTable.getIndexNeeded(lids) > 0 && !pauseIndexing) {
        QLOG_DEBUG() << "Unindexed Resources found: " << lids.size();
        for (qint32 i=0; i<lids.size() && keepRunning && !pauseIndexing; i++) {
            Resource r;
            resourceTable.get(r, lids.at(i));
            indexRecognition(lids.at(i), r);
            resourceTable.setIndexNeeded(lids.at(i), false);
        }
    }
    indexTimer->start();

}


void IndexRunner::indexNote(qint32 lid, Note &n) {
    QString content = QString::fromStdString(n.content); //.replace(QString("\n"), QString(" "));

    qint32 startPos =content.indexOf(QChar('<'));
    qint32 endPos = content.indexOf(QChar('>'),startPos)+1;
    content.remove(startPos,endPos-startPos);


    while (content.indexOf(QChar('<'))>=0 && keepRunning && !pauseIndexing) {
        startPos = content.indexOf(QChar('<'));
        endPos = content.indexOf(QChar('>'),startPos)+1;
        content.remove(startPos,endPos-startPos);
    };

    textDocument->setHtml(content);
    content = textDocument->toPlainText() + " " + QString::fromStdString(n.title);

    QSqlQuery sql;
    sql.exec("begin");
    sql.prepare("Delete from SearchIndex where lid=:lid and source='text'");
    sql.bindValue(":lid", lid);
    sql.exec();
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'note', :content)");
    sql.bindValue(":lid", lid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":content", content);
    sql.exec();

}


void IndexRunner::indexRecognition(qint32 lid, Resource &r) {
    if (!r.__isset.recognition || !r.recognition.__isset.body)
        return;

    QDomDocument doc;
    doc.setContent(QString::fromStdString(r.recognition.body));
    QDomElement docElem = doc.documentElement();

    // look for text tags
    QDomNodeList anchors = docElem.elementsByTagName("t");
    QSqlQuery sql;
    sql.exec("begin");
    sql.prepare("Delete from SearchIndex where lid=:lid and source='recognition'");
    sql.bindValue(":lid", lid);
    sql.exec();

    for (unsigned int i; i<anchors.length() && keepRunning && !pauseIndexing; i++) {
        QDomElement enmedia = anchors.at(i).toElement();
        QString weight = enmedia.attribute("w");
        QString text = enmedia.text();
        if (text != "") {
            int w = weight.toInt();
            sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'recognition', :content)");
            sql.bindValue(":lid", lid);
            sql.bindValue(":weight", w);
            sql.bindValue(":content", text);
            sql.exec();
        }
    }
    sql.exec("commit");
}

