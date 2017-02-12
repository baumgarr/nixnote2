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


#include "batchimport.h"

#include <QStringList>
#include <QString>
#include "sql/resourcetable.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include "sql/tagtable.h"
#include "sql/searchtable.h"
#include "sql/usertable.h"
#include "utilities/crossmemorymapper.h"
#include "utilities/mimereference.h"
#include "global.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QPushButton>

extern Global global;


//***********************************************************
//* Constructor.  If full == true then this is a restore.
//* if full=false then this is considered an import.  The
//* only real difference is that an import automatically
//* gets a new guid and is always considered "dirty"
//* where an import will keep the guid and will obey the
//* <dirty> tag in the note.
//***********************************************************
BatchImport::BatchImport(QObject *parent) : QObject(parent)
{
}


//***********************************************************
//* This is the main entry point for an import.  It is passed
//* the file which contains the export data.  It then
//* opens up the file, checks the validity of the data, and
//* then begins to parse through all of the crap.
//***********************************************************
void BatchImport::import(QString file) {
    fileName = file;
    qint32 newLid = -1;
    errorMessage = "";

    lastError = 0;
    QFile xmlFile(fileName);
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        lastError = 16;
        errorMessage = "Cannot open file.";
        return;
    }

    reader = new QXmlStreamReader(&xmlFile);
    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->hasError()) {
            errorMessage = reader->errorString();
            QLOG_ERROR() << "************************* ERROR READING IMPORT " << errorMessage;
            lastError = 16;
            return;
        }
        if (reader->name().toString().toLower() == "noteadd" && reader->isStartElement()) {
            newLid = addNoteNode();
        }
    }
    xmlFile.close();

    QString id = file;
    int pos = id.lastIndexOf(".");
    if (pos>0)
        id = id.mid(0,pos);
    pos = id.lastIndexOf(QDir::separator());
    if (pos>0)
        id = id.mid(pos+1);
    CrossMemoryMapper sharedMemory(id);
    if (!sharedMemory.attach())
        return;

    QString response = QString::number(newLid);
    sharedMemory.write(response.toAscii());
    sharedMemory.detach();
}


//***********************************************************
//* Process a <note> tag
//***********************************************************
qint32 BatchImport::addNoteNode() {
    qint32 newLid = -1;
    Note note;
    note.title = QString(tr("Untitled Note"));
    QUuid uuid;
    qint32 tempLid = 0;
    QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
    note.guid = newGuid;
    QStringList tagNames;
    QStringList tagGuids;
    QStringList resourceList;
    QString resourceDelimiter;
    QString newNoteBody = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")+
           QString("<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">")+
           QString("<en-note style=\"word-wrap: break-word; -webkit-nbsp-mode: space; -webkit-line-break: after-white-space;\"><br/></en-note>");
    note.active = true;
    note.content = newNoteBody;
    note.created = QDateTime::currentMSecsSinceEpoch();
    note.updated = QDateTime::currentMSecsSinceEpoch();

    bool atEnd = false;
    while(!atEnd) {
        QString name = reader->name().toString().toLower();
        if (name == "lid" && !reader->isEndElement()) {
            tempLid = intValue();
        }
        if (name == "title" && !reader->isEndElement()) {
            note.title = textValue();
        }
        if (name == "created" && !reader->isEndElement()) {
            QString dateString = textValue();
            //QDateTime date = QDateTime::fromString("2010-10-25T10:28:58.570Z", "yyyy-MM-ddTHH:mm:ss.zzzZ");
            QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-ddTHH:mm:ss.zzzZ");
            note.created = date.toMSecsSinceEpoch();
        }
        if (name == "updated" && !reader->isEndElement()) {
            QString dateString = textValue();
            QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-ddTHH:mm:ss.zzzZ");
            note.updated = date.toMSecsSinceEpoch();
        }
        if (name == "reminder" && !reader->isEndElement()) {
            QString dateString = textValue();
            QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-ddTHH:mm:ss.zzzZ");
            if (date >= QDateTime::currentDateTime()) {
                if (!note.attributes.isSet()) {
                    NoteAttributes na;
                    note.attributes = na;
                }
                note.attributes->reminderTime = date.toMSecsSinceEpoch();
            }
        }
        if (name == "notebook" && !reader->isEndElement()) {
            QString notebookName = textValue();
            NotebookTable notebookTable(global.db);
            qint32 lid = notebookTable.findByName(notebookName);
            QString notebookGuid;

            // Do we need to add the notebook?
            if (lid == 0) {
                Notebook book;
                book.name = notebookName;
                QUuid uuid;
                QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
                book.guid = newGuid;
                notebookGuid = newGuid;
                lid = notebookTable.add(0, book, true, false);
            } else {
                notebookTable.getGuid(notebookGuid, lid);
            }
            note.notebookGuid = notebookGuid;
        }
        if (name == "content" && !reader->isEndElement()) {
            note.content = textValue();
        }
        if (name == "attachmentdelimiter" && !reader->isEndElement()) {
            resourceDelimiter = textValue();
        }
        if (name == "attachment" && !reader->isEndElement()) {
            resourceList.append(textValue());
        }
        if (name == "tag" && !reader->isEndElement()) {
            QString tagName = textValue();
            TagTable tagTable(global.db);
            qint32 tagLid = tagTable.findByName(tagName, 0);
            QString tagGuid;

            // Do we need to add the tag?
            if (tagLid == 0) {
                Tag tag;
                tag.name = tagName;
                QUuid uuid;
                tagGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
                tag.guid = tagGuid;
                tagTable.add(0, tag, true, 0);
            } else {
                tagTable.getGuid(tagGuid, tagLid);
            }
            tagNames.append(tagName);
            tagGuids.append(tagGuid);
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "noteadd" && reader->isEndElement() && tempLid > 0) {
//            global.db = new DatabaseConnection("nixnote");  // Startup the database
            Note newNote;

            // Fetch the existing note
            NoteTable noteTable(global.db);
            if (!noteTable.get(newNote, tempLid, true, true)) {
                return -1;
            }


            // Append the text to the existing note
            newNote.content->replace("</en-note>", "<br/>");

            // Chop off the beginning of the new text to remove the <en-note stuff
            int startOfNote = note.content->indexOf("<en-note");
            note.content = note.content->mid(startOfNote+9);

            // Append the two notes
            newNote.content = newNote.content + note.content;

            // Start adding the resources
            qint32 noteLid = tempLid;
            if (resourceList.size() > 0) {
                note.resources = QList<Resource>();
                for (int i=0; i<resourceList.size(); i++) {
                    QString filename = resourceList[i];
                    QFile file(filename);
                    if (file.exists()) {
                        file.open(QIODevice::ReadOnly);
                        QByteArray ba = file.readAll();
                        file.close();
                        MimeReference mimeRef;
                        QString extension = filename;
                        int endPos = filename.lastIndexOf(".");
                        if (endPos != -1)
                            extension = extension.mid(endPos);
                        QString mime =  mimeRef.getMimeFromExtension(extension);
                        Resource newRes;
                        bool attachment = true;
                        if (mime == "application/pdf" || mime.startsWith("image/"))
                            attachment = false;
                        AddNote newNote;
                        newNote.createResource(newRes, 0, ba, mime, attachment, QFileInfo(filename).fileName(), noteLid);
                        QByteArray hash;
                        if (newRes.data.isSet()) {
                            Data d = newRes.data;
                            if (d.bodyHash.isSet())
                                hash = d.bodyHash;
                        }
                        QString mediaString = "<en-media hash=\""+hash.toHex()+"\" type=\""+mime+"\"/>";
                        if (note.content->contains(resourceDelimiter)) {
                            note.content = note.content->replace(note.content->indexOf(resourceDelimiter),
                                                                resourceDelimiter.size(), mediaString);
                        } else {
                            note.content = note.content->replace("</en-note>","<br>"+mediaString+"</en-note>");
                        }
                        note.resources->append(newRes);
                    }
                }
            }

            noteTable.updateNoteContent(tempLid, newNote.content, true);
            noteTable.add(tempLid,newNote,true);
            return tempLid;
        }
        if (endName == "noteadd" && reader->isEndElement() && tempLid == 0) {
            atEnd = true;
            note.tagGuids = tagGuids;
            note.tagNames = tagNames;
            NoteTable ntable(global.db);

            if (!note.notebookGuid.isSet()) {
                NotebookTable bookTable(global.db);
                QString book = bookTable.getDefaultNotebookGuid();
                note.notebookGuid = book;
            }

            // Start adding the resources
            qint32 noteLid = ntable.addStub(note.guid);
            if (resourceList.size() > 0) {
                note.resources = QList<Resource>();
                for (int i=0; i<resourceList.size(); i++) {
                    QString filename = resourceList[i];
                    QFile file(filename);
                    if (file.exists()) {
                        file.open(QIODevice::ReadOnly);
                        QByteArray ba = file.readAll();
                        file.close();
                        MimeReference mimeRef;
                        QString extension = filename;
                        int endPos = filename.lastIndexOf(".");
                        if (endPos != -1)
                            extension = extension.mid(endPos);
                        QString mime =  mimeRef.getMimeFromExtension(extension);
                        Resource newRes;
                        bool attachment = true;
                        if (mime == "application/pdf" || mime.startsWith("image/"))
                            attachment = false;
                        AddNote newNote;
                        newNote.createResource(newRes, 0, ba, mime, attachment, QFileInfo(filename).fileName(), noteLid);
                        QByteArray hash;
                        if (newRes.data.isSet()) {
                            Data d = newRes.data;
                            if (d.bodyHash.isSet())
                                hash = d.bodyHash;
                        }
                        QString mediaString = "<en-media hash=\""+hash.toHex()+"\" type=\""+mime+"\"/>";
                        if (note.content->contains(resourceDelimiter)) {
                            note.content = note.content->replace(note.content->indexOf(resourceDelimiter),
                                                                resourceDelimiter.size(), mediaString);
                        } else {
                            note.content = note.content->replace("</en-note>","<br>"+mediaString+"</en-note>");
                        }
                        note.resources->append(newRes);
                    }
                }
            }
            ntable.expunge(noteLid);  // delete the stub.
            newLid = ntable.add(0, note, true);
        }
    }
    return newLid;
}



QString BatchImport::textValue() {
    reader->readNext();
    return reader->text().toString();
}

qint32 BatchImport::intValue() {
    return textValue().toLong();
}


long BatchImport::longValue() {
    return textValue().toLong();
}


qlonglong BatchImport::longlongValue() {
    return textValue().toLongLong();
}


double BatchImport::doubleValue() {
    return textValue().toDouble();
}


bool BatchImport::booleanValue() {
    QString value = textValue();
    value.toLower();
    if (value == "1" || value == "true")
        return true;
    else
        return false;
}


short BatchImport::shortValue() {
    return textValue().toShort();
}
