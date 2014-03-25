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
        book.name = name.toStdString();
        bool isSynchronized = true;
        QUuid uuid;
        notebookGuid =  uuid.createUuid().toString().replace("{","").replace("}","");
        book.guid = notebookGuid.toStdString();
        book.__isset.name = true;
        book.__isset.guid = true;
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
    mb.setWindowTitle(tr("Scaning File"));
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


    NSqlQuery query(*global.db);
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
    note.guid = newGuid.toStdString();
    note.__isset.guid = true;
    note.active = true;
    note.__isset.active = true;

    bool atEnd = false;
    while(!atEnd) {
        QString name = reader->name().toString().toLower();
        if (name == "title" && !reader->isEndElement()) {
            note.title = textValue().toStdString();
            note.__isset.title = true;
        }
        if (name == "created" && !reader->isEndElement()) {
            note.created = datetimeValue();
            note.__isset.created = true;
        }
        if (name == "updated" && !reader->isEndElement()) {
            note.updated = datetimeValue();
            note.__isset.updated = true;
        }
        if (name == "deleted" && !reader->isEndElement()) {
            note.deleted = datetimeValue();
            note.__isset.deleted = true;
        }
        if (name == "active" && !reader->isEndElement()) {
            note.active = booleanValue();
            note.__isset.active = true;
        }
        if (name == "content" && !reader->isEndElement()) {
            note.content = textValue().toStdString();
            note.__isset.content = true;
        }
        if (name == "note-attributes" && !reader->isEndElement()) {
            processNoteAttributes(note.attributes);
            note.__isset.attributes = true;
        }
        if (name == "resource" && !reader->isEndElement()) {
            Resource newRes;
            processResource(newRes);
            newRes.noteGuid = note.guid;
            newRes.updateSequenceNum = 0;
            note.resources.push_back(newRes);
            note.__isset.resources = true;
        }
        if (name == "tag" && !reader->isEndElement()) {
           note.__isset.tagNames = true;
           note.tagNames.push_back(textValue().toStdString());
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "note" && reader->isEndElement())
            atEnd = true;
    }

    // Loop through the resources & make sure they all have the
    // proper guid for this note
    for (unsigned int i=0; i<note.resources.size(); i++) {
        note.resources[i].noteGuid = note.guid;
    }

    // Loop through the tag names & find any matching tags.
    if (note.__isset.tagNames) {
        note.tagGuids.clear();
        for (unsigned int i=0; i<note.tagNames.size(); i++) {
            QString tagGuid = tagList[QString::fromStdString(note.tagNames[i])];
            if (tagGuid != "") {
                note.tagGuids.push_back(tagGuid.toStdString());
                note.__isset.tagGuids = true;
            } else {
                QUuid uuid;
                QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
                Tag newTag;
                newTag.name = note.tagNames[i];
                newTag.guid = g.toStdString();
                newTag.__isset.name = true;
                newTag.__isset.guid = true;
                TagTable tt(global.db);
                tt.add(0, newTag, true, 0);
                tagList.insert(QString::fromStdString(note.tagNames[i]), g);
            }
        }
    }

    NoteTable noteTable(global.db);
    note.updateSequenceNum = 0;
    note.__isset.updateSequenceNum = true;
    note.notebookGuid = notebookGuid.toStdString();
    note.__isset.notebookGuid = true;

    if (note.__isset.created == false) {
        note.created = QDateTime::currentDateTime().toMSecsSinceEpoch();
        note.__isset.created = true;
    }
    if (note.__isset.updated == false) {
        note.updated = note.created;
        note.__isset.updated = true;
    }
    if (metaData.contains(QString(note.guid.c_str()))) {
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
    resource.__isset.active = true;

    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
//            if (name == "guid") {
//                resource.guid = textValue().toStdString();
//                resource.__isset.guid = true;
//            }
            QUuid uuid;
            QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
            resource.guid = g.toStdString();
            resource.__isset.guid = true;
//            if (name == "noteguid") {
//                QString tx = textValue();
//                resource.noteGuid = tx.toStdString();
//                resource.__isset.noteGuid = true;
//            }
//            if (name == "updatesequencenumber") {
//                resource.updateSequenceNum = intValue();
//                resource.__isset.updateSequenceNum = true;
//            }
            if (name == "active") {
                resource.active =  booleanValue();
                resource.__isset.active = true;
            }
            if (name == "mime") {
                resource.mime = textValue().toStdString();
                resource.__isset.mime = true;
            }
            if (name == "duration") {
                resource.duration = shortValue();
                resource.__isset.duration =true;
            }
            if (name == "height") {
                resource.height = shortValue();
                resource.__isset.height = true;
            }
            if (name == "width") {
                resource.width = shortValue();
                resource.__isset.width = true;
            }
            if (name == "data") {
                processData("Data", resource.data);
                resource.__isset.data = true;
            }
            if (name == "alternate-data") {
                processData("AlternateData", resource.data);
                resource.__isset.alternateData = true;
            }
            if (name == "recognition-data") {
                processData("RecognitionData", resource.recognition);
                resource.__isset.recognition = true;
            }
            if (name == "resource-attributes") {
                processResourceAttributes(resource.attributes);
                resource.__isset.attributes = true;
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
    data.body.append(bin.data(), bin.size());
    data.__isset.body = true;

    data.size = bin.length();
    data.__isset.size = true;

    QCryptographicHash md5hash(QCryptographicHash::Md5);
    QByteArray hash = md5hash.hash(bin, QCryptographicHash::Md5);
    string hashstring(hash.constData(), hash.size());
    data.bodyHash = hashstring;
    data.__isset.bodyHash = true;
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
                attributes.cameraMake = textValue().toStdString();
                attributes.__isset.cameraMake = true;
            }
            if (name == "camera-model") {
                attributes.cameraModel = textValue().toStdString();
                attributes.__isset.cameraModel =true;
            }
            if (name == "file-name") {
                attributes.fileName = textValue().toStdString();
                attributes.__isset.fileName = true;
            }
            if (name == "reco-type") {
                attributes.recoType = textValue().toStdString();
                attributes.__isset.fileName = true;
            }
            if (name  == "source-url") {
                attributes.sourceURL = textValue().toStdString();
                attributes.__isset.sourceURL = true;
            }
            if (name == "altitude") {
                attributes.altitude = doubleValue();
                attributes.__isset.altitude = true;
            }
            if (name == "longitude") {
                attributes.longitude = doubleValue();
                attributes.__isset.longitude = true;
            }
            if (name == "altitude") {
                attributes.latitude = doubleValue();
                attributes.__isset.latitude = true;
            }
            if (name == "timestamp") {
                attributes.timestamp = longValue();
                attributes.__isset.timestamp = true;
            }
            if (name == "attachment") {
                attributes.attachment = booleanValue();
                attributes.__isset.attachment = true;
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
                attributes.author = textValue().toStdString();
                attributes.__isset.author = true;
            }
            if (name == "source-url" && !reader->isEndElement()) {
                attributes.sourceURL = textValue().toStdString();
                attributes.__isset.sourceURL = true;
            }
            if (name == "source" && !reader->isEndElement()) {
                attributes.source = textValue().toStdString();
                attributes.__isset.source = true;
            }
            if (name == "source-application" && !reader->isEndElement()) {
                attributes.sourceApplication = textValue().toStdString();
                attributes.__isset.sourceApplication = true;
            }
            if (name == "altitude" && !reader->isEndElement()) {
                attributes.altitude = doubleValue();
                attributes.__isset.altitude = true;
            }
            if (name == "longitude" && !reader->isEndElement()) {
                attributes.longitude = doubleValue();
                attributes.__isset.longitude = true;
            }
            if (name == "latitude" && !reader->isEndElement()) {
                attributes.latitude = doubleValue();
                attributes.__isset.latitude = true;
            }
            if (name == "subject-date" && !reader->isEndElement()) {
                attributes.subjectDate = datetimeValue();
                attributes.__isset.subjectDate = true;
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
