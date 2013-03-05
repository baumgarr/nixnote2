/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2013 Randy Baumgarte

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
***********************************************************************************/

#include "indexrunner.h"
#include "global.h"
#include "sql/notetable.h"
#include "sql/resourcetable.h"
#include <QTextDocument>
#include <QtXml>
#include <poppler-qt4.h>


extern Global global;
using namespace Poppler;

IndexRunner::IndexRunner()
{
    moveToThread(this);
    keepRunning = true;
    pauseIndexing = false;
}

IndexRunner::~IndexRunner() {
}




void IndexRunner::run() {
//    hammer = new Thumbnailer();
    QLOG_DEBUG() << "Starting IndexRunner";
    indexTimer = new QTimer();
    indexTimer->setInterval(4000);
    connect(indexTimer, SIGNAL(timeout()), this, SLOT(index()));
    indexTimer->start();
    textDocument = new QTextDocument();
    exec();
    QLOG_DEBUG() << "Indexrunner exiting.";
}


void IndexRunner::index() {

    indexTimer->stop();

    QList<qint32> lids;
    NoteTable noteTable;
    ResourceTable resourceTable;

    if (keepRunning && noteTable.getIndexNeeded(lids) > 0 && !pauseIndexing) {
        QLOG_DEBUG() << "Unindexed Notes found: " << lids.size();


        for (qint32 i=0; i<lids.size() && keepRunning && !pauseIndexing; i++) {
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
            qint32 noteLid = noteTable.getLid(r.noteGuid);
            indexRecognition(noteLid, r);
            if (r.__isset.mime && r.mime == "application/pdf")
                indexPdf(noteLid, r);
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

    // Remove encrypted text
    while (content.indexOf("<en-crypt") > 0) {
        startPos = content.indexOf("<en-crypt");
        endPos = content.indexOf("</en-crypt>") + 11;
        content = content.mid(0,startPos)+content.mid(endPos);
    }

    // Remove any XML tags
    while (content.indexOf(QChar('<'))>=0 && keepRunning && !pauseIndexing) {
        startPos = content.indexOf(QChar('<'));
        endPos = content.indexOf(QChar('>'),startPos)+1;
        content.remove(startPos,endPos-startPos);
    };

    textDocument->setHtml(content);
    content = textDocument->toPlainText() + " " + QString::fromStdString(n.title);

    QSqlQuery sql;
    sql.prepare("Delete from SearchIndex where lid=:lid and source='text'");
    sql.bindValue(":lid", lid);
    sql.exec();
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'text', :content)");
    sql.bindValue(":lid", lid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":content", QString::fromStdString(n.title) + QString(" " ) +content);
    sql.exec();

}


void IndexRunner::indexRecognition(qint32 lid, Resource &r) {
    if (!r.__isset.recognition || !r.recognition.__isset.body)
        return;

    QDomDocument doc;
    QString emsg;
    doc.setContent(QString::fromStdString(r.recognition.body), &emsg);

    // look for text tags
    QDomNodeList anchors = doc.documentElement().elementsByTagName("t");
    QSqlQuery sql;
    sql.prepare("Delete from SearchIndex where lid=:lid and source='recognition'");
    sql.bindValue(":lid", lid);
    sql.exec();

    QSqlQuery trans;
    trans.exec("begin");
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'recognition', :content)");
    for (unsigned int i; i<anchors.length() && keepRunning && !pauseIndexing; i++) {
        QDomElement enmedia = anchors.at(i).toElement();
        QString weight = enmedia.attribute("w");
        QString text = enmedia.text();
        if (text != "") {
            int w = weight.toInt();
            sql.bindValue(":lid", lid);
            sql.bindValue(":weight", w);
            sql.bindValue(":content", text);
            sql.exec();
        }
    }
    trans.exec("commit");
}



void IndexRunner::indexPdf(qint32 lid, Resource &r) {
    ResourceTable rtable;
    qint32 reslid = rtable.getLid(r.guid);
    if (lid <= 0)
        return;
    QString file = global.fileManager.getDbaDirPath() + QString::number(reslid) +".pdf";

    QString text = "";
    Poppler::Document *doc = Poppler::Document::load(file);
    if (doc == NULL)
        return;
    for (int i=0; i<doc->numPages(); i++) {
        QRectF rect;
        text = text + doc->page(i)->text(rect) + QString(" ");
    }
    QSqlQuery sql;
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'recognition', :content)");
    sql.bindValue(":lid", lid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":content", text);
    sql.exec();

}
