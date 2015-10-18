#include "batchimport.h"

#include <QStringList>
#include <QString>
#include "sql/resourcetable.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include "sql/tagtable.h"
#include "sql/searchtable.h"
#include "sql/usertable.h"
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
    errorMessage = "";

    lastError = 0;
    QFile xmlFile(fileName);
    QFile scanFile(fileName);
    if (!xmlFile.open(QIODevice::ReadOnly) || !scanFile.open(QIODevice::ReadOnly)) {
        lastError = 16;
        errorMessage = "Cannot open file.";
        return;
    }

    reader = new QXmlStreamReader(&xmlFile);
    while (!reader->atEnd()) {
        reader->readNext();
        QLOG_DEBUG() << reader->name().toString();
        if (reader->hasError()) {
            errorMessage = reader->errorString();
            QLOG_ERROR() << "************************* ERROR READING BACKUP " << errorMessage;
            lastError = 16;
            return;
        }
        if (reader->name().toString().toLower() == "noteadd" && reader->isStartElement()) {
            addNoteNode();
        }
    }
    xmlFile.close();
}


//***********************************************************
//* Process a <note> tag
//***********************************************************
void BatchImport::addNoteNode() {
    Note note;
    note.title = QString(tr("Untitled Note"));
    QUuid uuid;
    QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
    note.guid = newGuid;
    QStringList tagNames;
    QStringList tagGuids;
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
        if (endName == "noteadd" && reader->isEndElement()) {
            atEnd = true;
            note.tagGuids = tagGuids;
            note.tagNames = tagNames;
            NoteTable ntable(global.db);

            if (!note.notebookGuid.isSet()) {
                NotebookTable bookTable(global.db);
                QString book = bookTable.getDefaultNotebookGuid();
                note.notebookGuid = book;
            }

            ntable.add(0, note, true);
        }
    }
    return;
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
