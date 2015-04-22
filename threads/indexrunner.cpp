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
    officeFound = false;  // temporarily disabled to test performance impact
}


// Destructor
IndexRunner::~IndexRunner() {
    delete indexHash;
}



// Main thread runner.  This just basically starts up the event queue.  Everything else
// is done via events signaled from the main thread.
void IndexRunner::initialize() {
    //moveToThread(this);
    keepRunning = true;
    pauseIndexing = false;
    disableIndexing = global.disableIndexing;
    init = true;
    QLOG_DEBUG() << "Starting IndexRunner";
    db = new DatabaseConnection("indexrunner");
    indexTimer = new QTimer();
    indexTimer->setInterval(global.minIndexInterval);
    connect(indexTimer, SIGNAL(timeout()), this, SLOT(index()));
    indexTimer->start();
    textDocument = new QTextDocument();
    indexHash = new QHash<qint32, IndexRecord*>();
    QLOG_DEBUG() << "Indexrunner initialized.";
}



// The index timer has expired.  Look for any unindexed notes or resources
void IndexRunner::index() {
    if (disableIndexing)
        return;

    if (!keepRunning || pauseIndexing) {
        return;
    }

    if (!init)
        initialize();

    indexTimer->stop();   // Stop the timer because we are already working
    indexTimer->setInterval(global.minIndexInterval);

    QList<qint32> lids;

    NoteTable noteTable(db);
    ResourceTable resourceTable(db);
    bool endMsgNeeded = false;

    int countPause = global.indexNoteCountPause;

    QList<qint32> finishedLids;
    // Get any unindexed notes
    if (keepRunning && !pauseIndexing && noteTable.getIndexNeeded(lids) > 0) {
        QApplication::processEvents();
        endMsgNeeded = true;
        QLOG_DEBUG() << "Unindexed Notes found: " << lids.size();

        // Index any undindexed note content.
        for (int i=0; keepRunning && !pauseIndexing && i<lids.size(); i++) {
            QApplication::processEvents();
            Note n;
            noteTable.get(n, lids[i], false, false);
            indexNote(lids[i],n);
            finishedLids.append(lids[i]);
            if (countPause <=0) {
                flushCache();
                for (int j=0; j<finishedLids.size(); j++)
                    noteTable.setIndexNeeded(finishedLids[j], false);
                indexTimer->start();
                return;
            }
            countPause--;
        }
    }
    if (keepRunning && !pauseIndexing)
       flushCache();
    for (int j=0; !pauseIndexing && keepRunning && j<finishedLids.size(); j++)
        noteTable.setIndexNeeded(finishedLids[j], false);


    lids.clear();  // Clear out the list so we can start on resources

    if (!keepRunning || pauseIndexing) {
        indexTimer->start();
        return;
    }

    countPause = global.indexResourceCountPause;
    finishedLids.clear();
    // Start indexing resources
    if (keepRunning && !pauseIndexing && resourceTable.getIndexNeeded(lids) > 0) {
        endMsgNeeded = true;

        // Index each resource that is needed.
        for (int i=0; keepRunning && !pauseIndexing && i<lids.size(); i++) {
            QApplication::processEvents();
            Resource r;
            resourceTable.get(r, lids.at(i), false);
            qint32 noteLid = noteTable.getLid(r.noteGuid);
            indexRecognition(noteLid, r);
            QString mime = "";
            if (r.mime.isSet())
                mime = r.mime;
            if (mime == "application/pdf")
                indexPdf(noteLid, r);
            else {
                if (mime.startsWith("application", Qt::CaseInsensitive))
                    indexAttachment(noteLid, r);
            }
            finishedLids.append(lids[i]);
            if (countPause <=0) {
                flushCache();
                for (int j=0; keepRunning && !pauseIndexing && j<finishedLids.size(); j++) {
                    resourceTable.setIndexNeeded(finishedLids[j], false);
                }
                indexTimer->start();
                return;
            }
            countPause--;
        }
    }
    if (!keepRunning || pauseIndexing) {
        indexTimer->start();
        return;
    }
    if (keepRunning && !pauseIndexing)
        flushCache();
    for (int j=0; keepRunning && !pauseIndexing && j<finishedLids.size(); j++) {
        resourceTable.setIndexNeeded(finishedLids[j], false);
    }

    if (endMsgNeeded) {
        QLOG_DEBUG() << "Indexing completed";
    }

    indexTimer->setInterval(global.maxIndexInterval);
    indexTimer->start();
}



// This indexes the actual note.
void IndexRunner::indexNote(qint32 lid, Note &n) {
    if (n.title.isSet()) {
        QLOG_DEBUG() << "Indexing note: " << n.title;
    }

    QString content = "";
    if (n.content.isSet())
        content = n.content; //.replace(QString("\n"), QString(" "));


    // Start looking through the note
    qint32 startPos =content.indexOf(QChar('<'));
    qint32 endPos = content.indexOf(QChar('>'),startPos)+1;
    content.remove(startPos,endPos-startPos);

    // Remove encrypted text
    while (keepRunning && !pauseIndexing && content.indexOf("<en-crypt") > 0) {
        startPos = content.indexOf("<en-crypt");
        endPos = content.indexOf("</en-crypt>") + 11;
        content = content.mid(0,startPos)+content.mid(endPos);
    }

    // Remove any XML tags
    while (keepRunning && !pauseIndexing && content.indexOf(QChar('<'))>=0) {
        startPos = content.indexOf(QChar('<'));
        endPos = content.indexOf(QChar('>'),startPos)+1;
        content.remove(startPos,endPos-startPos);
    };

    if (!keepRunning || pauseIndexing) {
        indexTimer->start();
        return;
    }

    // Get the content as an HTML doc.
    textDocument->setHtml(content);
    QString title  = "";
    if (n.title.isSet())
        title = n.title;
    content = textDocument->toPlainText() + " " + title;

    IndexRecord *rec = new IndexRecord();
    rec->content = content;
    rec->source = "text";
    rec->weight = 100;
    rec->lid = lid;
    if (indexHash->contains(lid)) {
        delete indexHash->value(lid);
        indexHash->remove(lid);
    }
    indexHash->insert(lid, rec);
}





// Index any resources
void IndexRunner::indexRecognition(qint32 lid, Resource &r) {

    if (!keepRunning || pauseIndexing) {
        indexTimer->start();
        return;
    }

    // Make sure we have something to look through.
    Data recognition;
    if (r.recognition.isSet())
        recognition = r.recognition;
    if (!recognition.body.isSet())
        return;

    QDomDocument doc;
    QString emsg;
    doc.setContent(recognition.body, &emsg);

    // look for text tags
    QDomNodeList anchors = doc.documentElement().elementsByTagName("t");

    for (unsigned int i=0; keepRunning && !pauseIndexing && i<anchors.length(); i++) {
        QApplication::processEvents();
        QDomElement enmedia = anchors.at(i).toElement();
        QString weight = enmedia.attribute("w");
        QString text = enmedia.text();
        if (text != "") {
            IndexRecord *rec = new IndexRecord();
            rec->weight = weight.toInt();
            rec->lid = lid;
            rec->content = text;
            rec->source = "recognition";
            if (indexHash->contains(lid)) {
                delete indexHash->value(lid);
                indexHash->remove(lid);
            }
            indexHash->insert(lid, rec);
        }
    }
}


// Index any PDFs that are attached.  Basically it turns the PDF into text and adds it the same
// way as a note's body
void IndexRunner::indexPdf(qint32 lid, Resource &r) {
    if (!keepRunning || pauseIndexing) {
        indexTimer->start();
        return;
    }
    ResourceTable rtable(db);
    qint32 reslid = rtable.getLid(r.guid);
    if (lid <= 0) {
        indexTimer->start();
        return;
    }
    QString file = global.fileManager.getDbaDirPath() + QString::number(reslid) +".pdf";

    QString text = "";
    Poppler::Document *doc = Poppler::Document::load(file);
    if (doc == NULL || doc->isEncrypted() || doc->isLocked()) {
        indexTimer->start();
        return;
    }
    for (int i=0; keepRunning && !pauseIndexing && i<doc->numPages(); i++) {
        QRectF rect;
        text = text + doc->page(i)->text(rect) + QString(" ");
    }
    IndexRecord *rec = new IndexRecord();
    rec->content = text;
    rec->source = "recognition";
    rec->weight = 100;
    rec->lid = lid;
    if (indexHash->contains(lid)) {
        delete indexHash->value(lid);
        indexHash->remove(lid);
    }
    indexHash->insert(lid, rec);
}




// Index any files that are attached.
void IndexRunner::indexAttachment(qint32 lid, Resource &r) {
    if (!officeFound)
        return;
    QLOG_DEBUG() << "indexing attachment to note " << lid;
    if (!keepRunning || pauseIndexing) {
        indexTimer->start();
        return;
    }
    ResourceTable rtable(db);
    qint32 reslid = rtable.getLid(r.guid);
    if (lid <= 0) {
        indexTimer->start();
        return;
    }
    QLOG_DEBUG() << "Resource " << reslid;
    QString extension = "";
    ResourceAttributes attributes;
    if (r.attributes.isSet())
        attributes = r.attributes;
    if (attributes.fileName.isSet()) {
        extension = attributes.fileName;
        int i = extension.indexOf(".");
        extension = extension.mid(i);
    }
    if (extension != ".doc"  && extension != ".xls"  && extension != ".ppt" &&
        extension != ".docx" && extension != ".xlsx" && extension != ".pptx" &&
        extension != ".pps"  && extension != ".pdf"  && extension != ".odt"  &&
        extension != ".odf"  && extension != ".ott"  && extension != ".odm"  &&
        extension != ".html" && extension != ".txt"  && extension != ".oth"  &&
        extension != ".ods"  && extension != ".ots"  && extension != ".odg"  &&
        extension != ".otg"  && extension != ".odp"  && extension != ".otp"  &&
        extension != ".odb"  && extension != ".oxt"  && extension != ".htm"  &&
        extension != ".docm")
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

    QString outDir = global.fileManager.getTmpDirPath();

    QProcess sofficeProcess;
    QString cmd = "soffice --headless --convert-to txt:\"Text\" --outdir "
                    +outDir + " "
                    +file;

    sofficeProcess.start(cmd,
                         QIODevice::ReadWrite|QIODevice::Unbuffered);

    QLOG_DEBUG() << "Starting soffice ";
    sofficeProcess.waitForStarted();
    QLOG_DEBUG() << "Waiting for completion";
    sofficeProcess.waitForFinished();
    int rc = sofficeProcess.exitCode();
    QLOG_DEBUG() << "soffice Errors:" << sofficeProcess.readAllStandardError();
    QLOG_DEBUG() << "soffice Output:" << sofficeProcess.readAllStandardOutput();
    QLOG_DEBUG() << "return code:" << rc;
    if (rc == 255) {
        QLOG_ERROR() << "soffice not found.  Disabling attachment indexing.";
        this->officeFound = false;
        return;
    }
    QFile txtFile(outDir+QString::number(reslid) +".txt");
    if (txtFile.open(QIODevice::ReadOnly)) {
        QString text;
        text = txtFile.readAll();
        NSqlQuery sql(db);
        sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, 'recognition', :content)");
        sql.bindValue(":lid", lid);
        sql.bindValue(":weight", 100);
        sql.bindValue(":content", text);
        QLOG_DEBUG() << "Adding note resource to index DB";
        sql.exec();
        txtFile.close();
    }
    QDir dir;
    dir.remove(outDir+QString::number(reslid) +".txt");
}


void IndexRunner::flushCache() {
    if (indexHash->size() <= 0)
        return;
    QDateTime start = QDateTime::currentDateTimeUtc();
    NSqlQuery sql(db);
    db->lockForWrite();
    sql.exec("begin");
    QHash<qint32, IndexRecord*>::iterator i;

    // Start adding words to the index.  Every 200 sql insertions we do a commit
    int commitCount = 200;

    for (i=indexHash->begin(); keepRunning && !pauseIndexing && i!=indexHash->end(); ++i) {
        qint32 lid = i.key();
        IndexRecord *rec = i.value();
        qint32 weight = rec->weight;
        QString source = rec->source;
        QString content = rec->content;
        delete rec;

        // Delete any old content
        sql.prepare("Delete from SearchIndex where lid=:lid and source=:source");
        sql.bindValue(":lid", lid);
        sql.bindValue(":source", source);
        sql.exec();

        // Add the new content.  it is basically a text version of the note with a weight of 100.
        sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, :source, :content)");
        sql.bindValue(":lid", lid);
        sql.bindValue(":weight", weight);
        sql.bindValue(":source", source);
        sql.bindValue(":content", content);
        sql.exec();
        commitCount--;
        if (commitCount <= 0) {
            sql.exec("commit");
            commitCount = 200;
        }
    }
    indexHash->clear();
    sql.exec("commit");

    sql.finish();
    db->unlock();
    QDateTime finish = QDateTime::currentDateTimeUtc();

    QLOG_DEBUG() << "Index Cache Flush Complete: " <<
                    finish.toMSecsSinceEpoch() - start.toMSecsSinceEpoch()
                    << " milliseconds.";
}

