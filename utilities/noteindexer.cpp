/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2015 Randy Baumgarte

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


#include "noteindexer.h"

#include "global.h"
#include "sql/notetable.h"
#include "sql/nsqlquery.h"
#include "sql/resourcetable.h"
#include <QTextDocument>
#include <QtXml>
#if QT_VERSION < 0x050000
#include <poppler-qt4.h>
#else
#include <poppler-qt5.h>
#endif

extern Global global;
using namespace Poppler;


NoteIndexer::NoteIndexer()
{
}


void NoteIndexer::indexNote(qint32 lid) {
    NoteTable ntable(global.db);
    Note n;
    ntable.get(n, lid,false,false);

    if (n.title.isSet()) {
        QLOG_DEBUG() << "Indexing note: " << n.title;
    }

    QString content = "";
    if (n.content.isSet())
        content = n.content;


    // Start looking through the note
    qint32 startPos =content.indexOf(QChar('<'));
    qint32 endPos = content.indexOf(QChar('>'),startPos)+1;
    content.remove(startPos,endPos-startPos);

    // Remove encrypted text
    while (content.contains("<en-crypt")) {
        startPos = content.indexOf("<en-crypt");
        endPos = content.indexOf("</en-crypt>") + 11;
        content = content.mid(0,startPos)+content.mid(endPos);
    }

    // Remove any XML tags
    while (content.contains(QChar('<'))) {
        startPos = content.indexOf(QChar('<'));
        endPos = content.indexOf(QChar('>'),startPos)+1;
        content.remove(startPos,endPos-startPos);
    };

    // Get the content as an HTML doc.
    QTextDocument textDocument;
    textDocument.setHtml(content);
    QString title  = "";
    if (n.title.isSet())
        title = n.title;
    content = textDocument.toPlainText() + " " + title;
    this->addTextIndex(lid, content);
}


void NoteIndexer::addTextIndex(int lid, QString content) {
    // Delete any old content
    NSqlQuery sql(global.db);
    sql.prepare("Delete from SearchIndex where lid=:lid and source=:source");
    sql.bindValue(":lid", lid);
    sql.bindValue(":source", "text");
    sql.exec();

    // Add the new content.  it is basically a text version of the note with a weight of 100.
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, :source, :content)");
    sql.bindValue(":lid", lid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":source", "text");
    sql.bindValue(":content", content);
    sql.exec();

    sql.prepare("Delete from DataStore where lid=:lid and key=:key");
    sql.bindValue(":lid", lid);
    sql.bindValue(":key", NOTE_INDEX_NEEDED);
    sql.exec();
}




void NoteIndexer::indexResource(qint32 lid, DatabaseConnection *db) {
    // Since this can be called from multiple threads, we need to know which DB connection we are using.

    QLOG_DEBUG() << "Fetching resource for index using " << db->getConnectionName();
    Resource r;
    ResourceTable resourceTable(db);
    resourceTable.get(r, lid, false);

    NSqlQuery sql(db);

    // Delete the old index
    QLOG_DEBUG() << "Deleting old resource from index";
    sql.prepare("Delete from SearchIndex where lid=:lid");
    sql.bindValue(":lid", lid);
    sql.exec();

    QLOG_DEBUG() << "Adding attributes to index.";
    if (r.attributes.isSet()) {
        ResourceAttributes a = r.attributes;
        if (a.fileName.isSet()) {
            sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, :source, :content)");
            sql.bindValue(":lid", lid);
            sql.bindValue(":weight", 100);
            sql.bindValue(":source", "recognition");
            sql.bindValue(":content", QString(a.fileName));
            sql.exec();
        }
        if (a.sourceURL.isSet()) {
            sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, :source, :content)");
            sql.bindValue(":lid", lid);
            sql.bindValue(":weight", 100);
            sql.bindValue(":source", "recognition");
            sql.bindValue(":content", QString(a.sourceURL));
            sql.exec();
        }
    }

    indexRecognition(lid, r);
    QString mime = "";
    if (r.mime.isSet())
        mime = r.mime;
    if (mime == "application/pdf")
        this->indexPdf(lid);
//    else {
//        if (mime.startsWith("application", Qt::CaseInsensitive))
//            indexAttachment(noteLid, r);
//   }

    QLOG_DEBUG() << "Resetting index needed.";
    sql.prepare("delete from DataStore where lid=:lid and key=:key");
    sql.bindValue(":lid", lid);
    sql.bindValue(":key", RESOURCE_INDEX_NEEDED);
    sql.exec();
}


// Index any resources
void NoteIndexer::indexRecognition(qint32 reslid, Resource &r) {

    if (!r.noteGuid.isSet() || !r.guid.isSet())
        return;

    ResourceTable rtable(global.db);
    if (reslid <= 0)
        return;

    NSqlQuery sql(global.db);

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

    sql.exec("begin;");
    for (unsigned int i=0;  i<anchors.length(); i++) {
        QApplication::processEvents();
        QDomElement enmedia = anchors.at(i).toElement();
        QString weight = enmedia.attribute("w");
        QString text = enmedia.text();
        if (text != "") {
            // Add the new content.  it is basically a text version of the note with a weight of 100.
            sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, :source, :content)");
            sql.bindValue(":lid", reslid);
            sql.bindValue(":weight", weight);
            sql.bindValue(":source", "recognition");
            sql.bindValue(":content", text);
            sql.exec();
        }
    }
    sql.exec("commit");
}



// Index any PDFs that are attached.  Basically it turns the PDF into text and adds it the same
// way as a note's body
void NoteIndexer::indexPdf(qint32 reslid) {

    NSqlQuery sql(global.db);
    if (reslid <= 0)
        return;

    QString file = global.fileManager.getDbaDirPath() + QString::number(reslid) +".pdf";

    QString text = "";
    Poppler::Document *doc = Poppler::Document::load(file);
    if (doc == NULL || doc->isEncrypted() || doc->isLocked())
        return;

    for (int i=0; i<doc->numPages(); i++) {
        QRectF rect;
        text = text + doc->page(i)->text(rect) + QString(" ");
    }

    // Add the new content.  it is basically a text version of the note with a weight of 100.
    sql.prepare("Insert into SearchIndex (lid, weight, source, content) values (:lid, :weight, :source, :content)");
    sql.bindValue(":lid", reslid);
    sql.bindValue(":weight", 100);
    sql.bindValue(":source", "recognition");
    sql.bindValue(":content", text);
    sql.exec();
}
