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


// Generic constructor
IndexRunner::IndexRunner()
{
    moveToThread(this);
    keepRunning = true;
    pauseIndexing = false;
}


// Destructor
IndexRunner::~IndexRunner() {
}



// Main thread runner.  This just basically starts up the event queue.  Everything else
// is done via events signaled from the main thread.
void IndexRunner::run() {
    moveToThread(this);
    QLOG_DEBUG() << "Starting IndexRunner";
    indexTimer = new QTimer();
    indexTimer->setInterval(4000);
    connect(indexTimer, SIGNAL(timeout()), this, SLOT(index()));
    indexTimer->start();
    textDocument = new QTextDocument();
    hammer = new Thumbnailer();
    exec();
    QLOG_DEBUG() << "Indexrunner exiting.";
}



// The index timer has expired.  Look for any unindexed notes or resources
void IndexRunner::index() {

    indexTimer->stop();   // Stop the timer because we are already working

    QList<qint32> lids;
    NoteTable noteTable;
    ResourceTable resourceTable;

    // Get any unindexed notes
    if (keepRunning && noteTable.getIndexNeeded(lids) > 0 && !pauseIndexing) {
        QLOG_DEBUG() << "Unindexed Notes found: " << lids.size();


        // Index any undindexed note content.
        for (qint32 i=0; i<lids.size() && keepRunning && !pauseIndexing; i++) {
            Note n;
            noteTable.get(n, lids[i], false, false);
            indexNote(lids[i],n);
//            Thumbnailer hammer;
//            hammer.setNote(lids[i], n);
            emit(thumbnailNeeded(lids[i]));
            //noteTable.setIndexNeeded(lids.at(i), false);
        }
    }


    lids.empty();  // Clear out the list so we can start on resources

    // Start indexing resources
    if (keepRunning && resourceTable.getIndexNeeded(lids) > 0 && !pauseIndexing) {
        QLOG_DEBUG() << "Unindexed Resources found: " << lids.size();

        // Index each resource that is needed.
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



// This indexes the actual note.
void IndexRunner::indexNote(qint32 lid, Note &n) {
    QString content = QString::fromStdString(n.content); //.replace(QString("\n"), QString(" "));


    // Start looking through the note
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

    // Get the content as an HTML doc.
    textDocument->setHtml(content);
    content = textDocument->toPlainText() + " " + QString::fromStdString(n.title);

    // Delete any old content
    QSqlQuery sql;
    sql.prepare("Delete from SearchIndex where lid=:lid and source='text'");
    sql.bindValue(":lid", lid);
    sql.exec();

    // Add the new content.  it is basically a text version of the note with a weight of 100.
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'text', :content)");
    sql.bindValue(":lid", lid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":content", QString::fromStdString(n.title) + QString(" " ) +content);
    sql.exec();

}



// Index any resources
void IndexRunner::indexRecognition(qint32 lid, Resource &r) {

    // Make sure we have something to look through.
    if (!r.__isset.recognition || !r.recognition.__isset.body)
        return;

    QDomDocument doc;
    QString emsg;
    doc.setContent(QString::fromStdString(r.recognition.body), &emsg);

    // look for text tags
    QDomNodeList anchors = doc.documentElement().elementsByTagName("t");

    // Delete the old resource index information
    QSqlQuery sql;
    sql.prepare("Delete from SearchIndex where lid=:lid and source='recognition'");
    sql.bindValue(":lid", lid);
    sql.exec();

    // Start adding words to the index.
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


// Index any PDFs that are attached.  Basically it turns the PDF into text and adds it the same
// way as a note's body
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


// Save the thumbnail to disk and mark the index as complete
void IndexRunner::renderThumbnail(qint32 lid, QString contents) {
    if (!keepRunning)
        quit();

    hammer->render(lid, contents);
    NoteTable noteTable;
    noteTable.setIndexNeeded(lid, false);
}
