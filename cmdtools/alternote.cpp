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
#include <QtXml>
#include <QString>

#include "sql/notetable.h"
#include "global.h"
#include "filters/filtercriteria.h"
#include "filters/filterengine.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include "sql/tagtable.h"
#include "utilities/nuuid.h"

#include "alternote.h"


extern Global global;

AlterNote::AlterNote(QObject *parent) :
    QObject(parent)
{
    addTagNames.clear();
    delTagNames.clear();
    notebook.clear();
    lids.clear();
    query="";
}



// Do the alter.  If the notebook is not found we create it.
// If a tag is not found for an add, we add it.  If a tag is
// not found for a deleteTag, we just ignore it.
int AlterNote::alterNote() {

    // If a query is specified, we find the matching notes.
    if (query != "") {
        FilterCriteria *filter = new FilterCriteria();
        global.filterCriteria.append(filter);
        global.filterPosition = 0;
        FilterEngine engine;
        filter->setSearchString(query);
        QList<qint32> lids;
        engine.filter(filter, &lids);
        this->lids.append(lids);
    }

    NotebookTable bookTable(global.db);
    TagTable tagTable(global.db);
    NoteTable noteTable(global.db);

    // Loop through each note requested.
    for (int i=0; i<lids.size(); i++) {
        qint32 lid = lids[i];

        // Do the notebook request
        if (notebook != "") {
            qint32 notebookLid = bookTable.findByName(notebook);
            if (notebookLid<0) {
                Notebook book;
                book.name = notebook;
                NUuid uuid;
                book.guid = uuid.create();
                notebookLid = bookTable.add(0,book,true);
            }
            if (noteTable.getNotebookLid(lid) != notebookLid)
                noteTable.updateNotebook(lid, notebookLid, true);
        }

        // Add the tags
        for (int j=0; j<addTagNames.size(); j++) {
            qint32 tagLid = tagTable.findByName(addTagNames[j],0);
            if (tagLid <= 0) {
                Tag t;
                t.name = addTagNames[j];
                NUuid uuid;
                t.guid = uuid.create();
                tagLid = tagTable.add(0,t,true,0);
            }
            if (!noteTable.hasTag(lid,tagLid))
                noteTable.addTag(lid, tagLid, true);
        }

        // Remove any tags specified
        for (int j=0; j<delTagNames.size(); j++) {
            qint32 tagLid = tagTable.findByName(delTagNames[j],0);
            if (tagLid > 0 && noteTable.hasTag(lid,tagLid)) {
                noteTable.removeTag(lid, tagLid, true);
            }
        }
    }

    return 0;
}



QString AlterNote::wrap() {

    QString returnValue;
    QXmlStreamWriter *writer = new QXmlStreamWriter(&returnValue);
    writer->setAutoFormatting(true);
    writer->setCodec("UTF-8");
    writer->writeStartDocument();
    writer->writeDTD("<!DOCTYPE NixNote-Query>");
    writer->writeStartElement("nixnote-alternote");
    writer->writeAttribute("version", "2");
    writer->writeAttribute("application", "NixNote");
    writer->writeAttribute("applicationVersion", "2.x");
    writer->writeStartElement("AlterNote");
    for (int i=0; i<lids.size(); i++)
        writer->writeTextElement("id", QString::number(lids[i]));
    writer->writeTextElement("Notebook", notebook);
    writer->writeTextElement("Query", query);
    for (int i=0; i<addTagNames.size(); i++)
        writer->writeTextElement("AddTag", addTagNames[i]);
    for (int i=0; i<delTagNames.size(); i++)
        writer->writeTextElement("DelTag", delTagNames[i]);
    writer->writeEndElement();
    writer->writeEndElement();
    writer->writeEndDocument();
    return returnValue;
}




void AlterNote::unwrap(QString data) {
    lastError = 0;
    QXmlStreamReader reader(data);
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.hasError()) {
            errorMessage = reader.errorString();
            QLOG_ERROR() << "************************* ERROR READING IMPORT " << errorMessage;
            lastError = 16;
            return;
        }
        if (reader.name().toString().toLower() == "alternote" && !reader.isEndElement()) {
            reader.readNext();
            while(reader.name().toString().toLower() != "alternote") {
                if (reader.name().toString().toLower() == "id" && reader.isStartElement()) {
                    reader.readNext();
                    lids.append(reader.text().toString().toInt());
                } else if (reader.name().toString().toLower() == "notebook" && reader.isStartElement()) {
                    reader.readNext();
                    notebook = reader.text().toString();
                } else if (reader.name().toString().toLower() == "addtag" && reader.isStartElement()) {
                    reader.readNext();
                    addTagNames.append(reader.text().toString());
                } else if (reader.name().toString().toLower() == "deltag" && reader.isStartElement()) {
                    reader.readNext();
                    delTagNames.append(reader.text().toString());
                } else if (reader.name().toString().toLower() == "query" && reader.isStartElement()) {
                    reader.readNext();
                    query = reader.text().toString();
                } else
                    reader.readNext();
            }
        }
    }
}
