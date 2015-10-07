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



#include "importenex.h"
#include "sql/notebooktable.h"
#include "sql/tagtable.h"
#include "sql/notetable.h"
#include "sql/nsqlquery.h"

#include <QMessageBox>
#include <QPushButton>


extern Global global;

ImportEnex::ImportEnex(QObject *parent) : QObject(parent)
{   
    importTags = false;
    importNotebooks = false;

    NotebookTable t(global.db);
    QString name = tr("Imported Notes");
    qint32 lid=t.findByName(name);
    if (lid == 0) {
        // We have a new notebook to add
        Notebook book;
        book.name = name;
        bool isSynchronized = true;
        QUuid uuid;
        notebookGuid =  uuid.createUuid().toString().replace("{","").replace("}","");
        book.guid = notebookGuid;
        t.add(0,book,true, !isSynchronized);
    } else {
        t.getGuid(notebookGuid, lid);
    }

    TagTable tt(global.db);
    tt.getNameMap(tagList);
    progress = new QProgressDialog();
    progress->setVisible(false);
    stopNow = false;
}




void ImportEnex::import(QString file) {
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
    QTextStream *countReader = new QTextStream(&scanFile);
    int recCnt = 0;

    QMessageBox mb;
    mb.setWindowTitle(tr("Scanning File"));
    mb.setText(QString::number(recCnt) + tr(" notes found."));
    QPushButton *cancelButton = mb.addButton(QMessageBox::Cancel);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(canceled()));
    mb.show();
    QCoreApplication::processEvents();

    while (!countReader->atEnd() && !stopNow) {
        QString line = countReader->readLine();
        if (line.contains("<note>")) {
            recCnt++;
            mb.setText(QString::number(recCnt) + tr(" notes found."));
            QCoreApplication::processEvents();
        }
    }

    progress->setMaximum(recCnt);
    progress->setMinimum(0);
    progress->setWindowTitle(tr("Importing Notes"));
    progress->setLabelText(tr("Importing Notes"));
    progress->setWindowModality(Qt::ApplicationModal);
    connect(progress, SIGNAL(canceled()), this, SLOT(canceled()));
    progress->setVisible(true);
    mb.close();
    progress->show();


    NSqlQuery query(global.db);
    query.exec("begin");
    recCnt = 0;
    while (!reader->atEnd() && !stopNow) {
        reader->readNext();
        if (reader->hasError()) {
            errorMessage = reader->errorString();
            QLOG_ERROR() << "************************* ERROR READING BACKUP " << errorMessage;
            lastError = 16;
            return;
        }

        if (reader->name().toString().toLower() == "en-export" && reader->isStartElement()) {
            QXmlStreamAttributes attributes = reader->attributes();
            QString version = attributes.value("version").toString();
            QString application = attributes.value("application").toString();
            if (version != "5.x") {
                lastError = 1;
                errorMessage = "Unknown export version = " +version;
                return;
            }
            if (application.toLower() != "evernote/windows") {
                lastError = 2;
                errorMessage = "This export is from an unknown application = " +application;
                return;
            }
        }
        if (reader->name().toString().toLower() == "note" && reader->isStartElement()) {
            recCnt++;
            progress->setValue(recCnt);
            QLOG_DEBUG() << "Importing Note " << recCnt;
            processNoteNode();
        }
    }
    xmlFile.close();
    query.exec("commit");
    progress->hide();
}





//***********************************************************
//* Process a <note> tag
//***********************************************************
void ImportEnex::processNoteNode() {
    Note note;
    QUuid uuid;
    QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
    note.guid = newGuid;
    note.active = true;
    QList<Resource> resources;
    QList<QString> tagNames;

    bool atEnd = false;
    while(!atEnd) {
        QString name = reader->name().toString().toLower();
        if (name == "title" && !reader->isEndElement()) {
            note.title = textValue();
        }
        if (name == "created" && !reader->isEndElement()) {
            note.created = datetimeValue();
        }
        if (name == "updated" && !reader->isEndElement()) {
            note.updated = datetimeValue();
        }
        if (name == "deleted" && !reader->isEndElement()) {
            note.deleted = datetimeValue();
        }
        if (name == "active" && !reader->isEndElement()) {
            note.active = booleanValue();
        }
        if (name == "content" && !reader->isEndElement()) {
            note.content = textValue();
        }
        if (name == "note-attributes" && !reader->isEndElement()) {
            NoteAttributes na;
            note.attributes = na;
            processNoteAttributes(note.attributes);
        }
        if (name == "resource" && !reader->isEndElement()) {
            Resource newRes;
            processResource(newRes);
            newRes.noteGuid = note.guid;
            newRes.updateSequenceNum = 0;
            resources.append(newRes);
        }
        if (name == "tag" && !reader->isEndElement()) {
           tagNames.append(textValue());
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "note" && reader->isEndElement())
            atEnd = true;
    }


    // Loop through the resources & make sure they all have the
    // proper guid for this note
    for (int i=0; i<resources.size(); i++) {
        Resource *r = &resources[i];
        r->noteGuid = note.guid;
    }

    // Loop through the tag names & find any matching tags.
    if (note.tagNames.isSet()) {
        QList<QString> tagGuids;
        for (int i=0; i<tagNames.size(); i++) {
            QString tagGuid = tagList[tagNames[i]];
            if (tagGuid != "") {
                tagGuids.append(tagGuid);
            } else {
                QUuid uuid;
                QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
                Tag newTag;
                newTag.name = tagNames[i];
                newTag.guid = g;
                TagTable tt(global.db);
                tt.add(0, newTag, true, 0);
                tagList.insert(tagNames[i], g);
            }
        }
        note.tagGuids = tagGuids;
    }
    note.resources = resources;
    note.tagNames = tagNames;

    NoteTable noteTable(global.db);
    note.updateSequenceNum = 0;
    note.notebookGuid = notebookGuid;

    if (!note.created.isSet() == false) {
        note.created = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    if (note.updated.isSet() == false) {
        note.updated = note.created;
    }
    if (metaData.contains(note.guid)) {
        QLOG_ERROR() << "ERROR IN IMPORTING DATA:  Metadata not yet supported";
    }

    noteTable.add(0,note, true);
    return;
}








//***********************************************************
//* Process a <noteresource> node.
//***********************************************************
void ImportEnex::processResource(Resource &resource) {
    bool atEnd = false;

    resource.active = true;

    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            QUuid uuid;
            QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
            resource.guid = g;
            if (name == "active") {
                resource.active =  booleanValue();
            }
            if (name == "mime") {
                resource.mime = textValue();
            }
            if (name == "duration") {
                resource.duration = shortValue();
            }
            if (name == "height") {
                resource.height = shortValue();
            }
            if (name == "width") {
                resource.width = shortValue();
            }
            if (name == "data") {
                Data d;
                resource.data = d;
                processData("Data", resource.data);
            }
            if (name == "alternate-data") {
                Data d;
                resource.alternateData = d;
                processData("AlternateData", resource.data);
            }
            if (name == "recognition-data") {
                Data d;
                resource.recognition = d;
                processData("RecognitionData", resource.recognition);
            }
            if (name == "resource-attributes") {
                ResourceAttributes ra;
                resource.attributes = ra;
                processResourceAttributes(resource.attributes);
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "resource" && reader->isEndElement())
            atEnd = true;
    }
}



//***********************************************************
//* Process any type of data node
//***********************************************************
void ImportEnex::processData(QString nodeName, Data &data) {
    nodeName.toLower();

    QString x = textValue();
    QByteArray ba;
    ba.append(x);
    QByteArray bin = QByteArray::fromBase64(ba);
    data.body.clear();
    data.body = bin;
    data.size = bin.length();

    QCryptographicHash md5hash(QCryptographicHash::Md5);
    QByteArray hash = md5hash.hash(bin, QCryptographicHash::Md5);
    data.bodyHash = hash;
}



//***********************************************************
//* Process a node that has the <noteresourceattribute>
//***********************************************************
void ImportEnex::processResourceAttributes(ResourceAttributes &attributes) {
    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "camera-make") {
                attributes.cameraMake = textValue();
            }
            if (name == "camera-model") {
                attributes.cameraModel = textValue();
            }
            if (name == "file-name") {
                attributes.fileName = textValue();
            }
            if (name == "reco-type") {
                attributes.recoType = textValue();
            }
            if (name  == "source-url") {
                attributes.sourceURL = textValue();
            }
            if (name == "altitude") {
                attributes.altitude = doubleValue();
            }
            if (name == "longitude") {
                attributes.longitude = doubleValue();
            }
            if (name == "altitude") {
                attributes.latitude = doubleValue();
            }
            if (name == "timestamp") {
                attributes.timestamp = longValue();
            }
            if (name == "attachment") {
                attributes.attachment = booleanValue();
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "resource-attributes" && reader->isEndElement())
            atEnd = true;
    }
}



//***********************************************************
//* Process an <attributes> node
//***********************************************************
void ImportEnex::processNoteAttributes(NoteAttributes &attributes) {
    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "author" && !reader->isEndElement()) {
                attributes.author = textValue();
            }
            if (name == "source-url" && !reader->isEndElement()) {
                attributes.sourceURL = textValue();
            }
            if (name == "source" && !reader->isEndElement()) {
                attributes.source = textValue();
            }
            if (name == "source-application" && !reader->isEndElement()) {
                attributes.sourceApplication = textValue();
            }
            if (name == "altitude" && !reader->isEndElement()) {
                attributes.altitude = doubleValue();
            }
            if (name == "longitude" && !reader->isEndElement()) {
                attributes.longitude = doubleValue();
            }
            if (name == "latitude" && !reader->isEndElement()) {
                attributes.latitude = doubleValue();
            }
            if (name == "subject-date" && !reader->isEndElement()) {
                attributes.subjectDate = datetimeValue();
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "note-attributes" && reader->isEndElement())
            atEnd = true;
    }

    return;
}







//***********************************************************
//* take a node's value and return the text
//***********************************************************
QString ImportEnex::textValue() {
    reader->readNext();
    return reader->text().toString();
}


//***********************************************************
//* take a node's value and return the integer value
//***********************************************************
qint32 ImportEnex::intValue() {
    return textValue().toLong();
}


//***********************************************************
//* take a node's value and return a long value
//***********************************************************
long ImportEnex::longValue() {
    return textValue().toLong();
}


//***********************************************************
//* take a node's value and return longlong.  This is useful
//* for date values.
//***********************************************************
qlonglong ImportEnex::longLongValue() {
    return textValue().toLongLong();
}


//***********************************************************
//* take a node's value and return a double value
//***********************************************************
double ImportEnex::doubleValue() {
    return textValue().toDouble();
}


//***********************************************************
//* take a node's value and return true/false based upon
//* its value
//***********************************************************
bool ImportEnex::booleanValue() {
    QString value = textValue();
    value.toLower();
    if (value == "1" || value == "true")
        return true;
    else
        return false;
}


//***********************************************************
//* Take a node's text & return the short value
//***********************************************************
short ImportEnex::shortValue() {
    return textValue().toShort();
}


//***********************************************************
//* take a node's value and return a long value
//***********************************************************
qlonglong ImportEnex::datetimeValue() {
    QString value = textValue();
    QString year = value.mid(0,4);
    QString month = value.mid(4,2);
    QString day = value.mid(6,2);

    QString hour = value.mid(9,2);
    QString minute = value.mid(11,2);
    QString second = value.mid(13,2);

    QDate date;
    date.setDate(year.toInt(), month.toInt(), day.toInt());
    QTime time;
    time.setHMS(hour.toInt(), minute.toInt(), second.toInt());

    QDateTime dt;
    dt.setDate(date);
    dt.setTime(time);
    dt.setTimeSpec(Qt::UTC);

    return dt.toMSecsSinceEpoch();
}


//***********************************************************
//* Set the notebook GUID for any imported notes.  This is
//* only useful when importing a note.  When doing a restore
//* we don't use this.
//***********************************************************
void ImportEnex::setNotebookGuid(QString g) {
    notebookGuid = g;
}

//***********************************************************
//* This is used to retrieve any errors that occur when
//* tying to restore or import from a nnex file
//***********************************************************
QString ImportEnex::getErrorMessage() {
    return errorMessage;
}



void ImportEnex::canceled() {
    stopNow = true;
}
