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
#include "sql/nsqlquery.h"
#include "sql/resourcetable.h"
#include <QTextDocument>
#include <QtXml>
#include <poppler-qt4.h>

extern Global global;
using namespace Poppler;

// Generic constructor
IndexRunner::IndexRunner()
{
    init = false;
}


// Destructor
IndexRunner::~IndexRunner() {
}



// Main thread runner.  This just basically starts up the event queue.  Everything else
// is done via events signaled from the main thread.
void IndexRunner::initialize() {
    //moveToThread(this);
    keepRunning = true;
    pauseIndexing = false;
    init = true;
    QLOG_DEBUG() << "Starting IndexRunner";
    db = new DatabaseConnection("indexrunner");
    indexTimer = new QTimer();
    minInterval = 5000;
    maxInterval = 120000;
    indexTimer->setInterval(minInterval);
    connect(indexTimer, SIGNAL(timeout()), this, SLOT(index()));
    indexTimer->start();
    textDocument = new QTextDocument();
    QLOG_DEBUG() << "Indexrunner initialized.";
}



// The index timer has expired.  Look for any unindexed notes or resources
void IndexRunner::index() {
    if (!init)
        initialize();

    indexTimer->stop();   // Stop the timer because we are already working
    indexTimer->setInterval(minInterval);

    QList<qint32> lids;

    NoteTable noteTable(&db->conn);
    ResourceTable resourceTable(&db->conn);
    bool endMsgNeeded = false;

    int countPause = 10;

    // Get any unindexed notes
    if (keepRunning && noteTable.getIndexNeeded(lids) > 0 && !pauseIndexing) {
        QApplication::processEvents();
        endMsgNeeded = true;
        QLOG_DEBUG() << "Unindexed Notes found: " << lids.size();

        // Index any undindexed note content.
        for (int i=0; i<lids.size() && keepRunning && !pauseIndexing; i++) {
            QApplication::processEvents();
            Note n;
            noteTable.get(n, lids[i], false, false);
            indexNote(lids[i],n);
            noteTable.setIndexNeeded(lids[i], false);
            if (countPause <=0) {
                indexTimer->start();
                return;
            }
            countPause--;
        }
    }

    lids.clear();  // Clear out the list so we can start on resources

    countPause = 2;
    // Start indexing resources
    if (keepRunning && resourceTable.getIndexNeeded(lids) > 0 && !pauseIndexing) {
        endMsgNeeded = true;

        // Index each resource that is needed.
        for (int i=0; i<lids.size() && keepRunning && !pauseIndexing; i++) {
            QApplication::processEvents();
            Resource r;
            resourceTable.get(r, lids.at(i), false);
            qint32 noteLid = noteTable.getLid(r.noteGuid);
            indexRecognition(noteLid, r);
            if (r.__isset.mime && r.mime == "application/pdf")
                indexPdf(noteLid, r);
            else {
                if (r.__isset.mime && QString::fromStdString(r.mime).startsWith("application", Qt::CaseInsensitive))
                    indexAttachment(noteLid, r);
            }
            resourceTable.setIndexNeeded(lids.at(i), false);
            if (countPause <=0) {
                indexTimer->start();
                return;
            }
            countPause--;
        }
    }
    if (endMsgNeeded) {
        QLOG_DEBUG() << "Indexing completed";
    }
    indexTimer->setInterval(maxInterval);
    indexTimer->start();
}



// This indexes the actual note.
void IndexRunner::indexNote(qint32 lid, Note &n) {
    QLOG_DEBUG() << "Indexing note: " << QString::fromStdString(n.title);
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
    NSqlQuery sql(db->conn);
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

    // Setup transaction
    NSqlQuery trans(db->conn);
    trans.exec("begin");

    // Delete the old resource index information
    NSqlQuery sql(db->conn);
    sql.prepare("Delete from SearchIndex where lid=:lid and source='recognition'");
    sql.bindValue(":lid", lid);
    sql.exec();

    // Start adding words to the index.
    int tracelog = 200;
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'recognition', :content)");
    for (unsigned int i=0; i<anchors.length() && keepRunning && !pauseIndexing; i++) {
        QApplication::processEvents();
        QDomElement enmedia = anchors.at(i).toElement();
        QString weight = enmedia.attribute("w");
        QString text = enmedia.text();
        tracelog--;
        if (text != "") {
            int w = weight.toInt();
            sql.bindValue(":lid", lid);
            sql.bindValue(":weight", w);
            sql.bindValue(":content", text);
            sql.exec();
        }
        if (tracelog <=0 ) {
            tracelog = 200;
            trans.exec("commit");
        }
    }
    trans.exec("commit");
}


// Index any PDFs that are attached.  Basically it turns the PDF into text and adds it the same
// way as a note's body
void IndexRunner::indexPdf(qint32 lid, Resource &r) {
    ResourceTable rtable(&db->conn);
    qint32 reslid = rtable.getLid(r.guid);
    if (lid <= 0 || !keepRunning || pauseIndexing)
        return;
    QString file = global.fileManager.getDbaDirPath() + QString::number(reslid) +".pdf";

    QString text = "";
    Poppler::Document *doc = Poppler::Document::load(file);
    if (doc == NULL)
        return;
    for (int i=0; i<doc->numPages() && keepRunning && !pauseIndexing; i++) {
        QRectF rect;
        text = text + doc->page(i)->text(rect) + QString(" ");
    }
    NSqlQuery sql(db->conn);
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'recognition', :content)");
    sql.bindValue(":lid", lid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":content", text);
    sql.exec();
}




// Index any PDFs that are attached.  Basically it turns the PDF into text and adds it the same
// way as a note's body
void IndexRunner::indexAttachment(qint32 lid, Resource &r) {
    return;
    ResourceTable rtable(&db->conn);
    qint32 reslid = rtable.getLid(r.guid);
    if (lid <= 0 || !keepRunning || pauseIndexing)
        return;
    QString extension = "";
    if (r.__isset.attributes && r.attributes.__isset.fileName) {
        extension = QString::fromStdString(r.attributes.fileName);
        int i = extension.indexOf(".");
        extension = extension.mid(i);
    }
    if (extension == ".exe" || extension == ".dlL" || extension == ".zip" ||
            extension == ".bz" || extension == ".tar.gz" || extension == ".tar")
        return;

    QString file = global.fileManager.getDbaDirPath() + QString::number(reslid) +extension;
    QFile dataFile(file);
    if (!dataFile.exists()) {
        QDir dir(global.fileManager.getDbaDirPath());
        QStringList filterList;
        filterList.append(QString::number(lid)+".*");
        QStringList list= dir.entryList(filterList, QDir::Files);
        if (list.size() > 0) {
            file = global.fileManager.getDbaDirPath()+list[0];
        }
    }

    QString outFile = global.fileManager.getDbaDirPath() + QString::number(reslid) + "-index.txt";

    QProcess sofficeProcess;
    sofficeProcess.start("soffice --headless --convert-to txt:\"text\" "+file,
                         QIODevice::ReadWrite|QIODevice::Unbuffered);

    QLOG_DEBUG() << "Starting soffice " << sofficeProcess.waitForStarted();
    QLOG_DEBUG() << "Stopping soffice " << sofficeProcess.waitForFinished() << " Return Code: " << sofficeProcess.state();
    QLOG_DEBUG() << "soffice Errors:" << sofficeProcess.readAllStandardError();
    QLOG_DEBUG() << "soffice Output:" << sofficeProcess.readAllStandardOutput();

//    NSqlQuery sql(db->conn);
//    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'recognition', :content)");
//    sql.bindValue(":lid", lid);
//    sql.bindValue(":weight", 100);
//    QLOG_DEBUG() << "Adding note resource to index DB";
//    sql.exec();
}

