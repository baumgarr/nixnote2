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


#include "importdata.h"
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
ImportData::ImportData(bool full, bool cmdline, QObject *parent) : QObject(parent)
{
    importTags = false;
    importNotebooks = false;
    backup = full;
    createTags = false;
    stopNow = false;
    this->cmdline = cmdline;

    // get the
    if (!full) {
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
    }
    if (!this->cmdline) {
        progress = new QProgressDialog();
        progress->setVisible(false);
    }
}


//***********************************************************
//* This is the main entry point for an import.  It is passed
//* the file which contains the export data.  It then
//* opens up the file, checks the validity of the data, and
//* then begins to parse through all of the crap.
//***********************************************************
void ImportData::import(QString file) {
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

    QTextStream *countReader = new QTextStream(&scanFile);

    int recCnt = 0;
    QMessageBox mb;
    if (!cmdline) {
        mb.setWindowTitle(tr("Scanning File"));
        mb.setText(QString::number(recCnt) + tr(" notes found."));
        QPushButton *cancelButton = mb.addButton(QMessageBox::Cancel);
        connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
        mb.show();
        QCoreApplication::processEvents();
    }

    while (!countReader->atEnd() && !stopNow) {
        QString line = countReader->readLine();
        if (line.contains("<note>", Qt::CaseInsensitive)) {
            recCnt++;
            if (!cmdline) {
                mb.setText(QString::number(recCnt) + tr(" notes found."));
                QCoreApplication::processEvents();
            }
        }
    }

    notebookData.clear();
    if (!cmdline) {
        progress->setMaximum(recCnt);
        progress->setMinimum(0);
        if (backup) {
            progress->setWindowTitle(tr("Importing"));
            progress->setLabelText(tr("Importing Notes"));
        } else {
            progress->setWindowTitle(tr("Restore"));
            progress->setLabelText(tr("Restoring Notes"));
        }
        progress->setWindowModality(Qt::ApplicationModal);
        connect(progress, SIGNAL(canceled()), this, SLOT(cancel()));
        progress->setVisible(true);
        mb.close();
        progress->show();
    }
    recCnt = 0;

    reader = new QXmlStreamReader(&xmlFile);
    NSqlQuery query(global.db);
    query.exec("begin");
    int commitCount = 100;
    while (!reader->atEnd() && !stopNow) {
        if (commitCount <=  0) {
            query.exec("commit");
            commitCount = 100;
        }
        commitCount--;
        reader->readNext();
        if (reader->hasError()) {
            errorMessage = reader->errorString();
            QLOG_ERROR() << "************************* ERROR READING BACKUP " << errorMessage;
            lastError = 16;
            return;
        }
        if (reader->name().toString().toLower() == "nevernote-export" && reader->isStartElement()) {
            QXmlStreamAttributes attributes = reader->attributes();
            QString version = attributes.value("version").toString();
            QString type = attributes.value("exportType").toString();
            QString application = attributes.value("application").toString();
            if (version != "0.85" && version != "0.86"
                    && version != "0.95") {
                lastError = 1;
                errorMessage = "Unknown backup version = " +version;
                return;
            }
            if (application.toLower() != "nevernote") {
                lastError = 2;
                errorMessage = "This backup is from an unknown application = " +application;
                return;
            }
            if (type.toLower() == "backup" && !backup) {
                lastError = 4;
                errorMessage = "This is backup file, not an export file";
                if (!cmdline)
                    progress->hide();
                return;
            }
            if (type.toLower() == "export" && backup) {
                lastError = 5;
                errorMessage = "This is an export file, not a backup file";
                return;
            }
        }
        if (reader->name().toString().toLower() == "synchronization" && reader->isStartElement() && backup) {
            processSynchronizationNode();
        }
        if (reader->name().toString().toLower() == "note" && reader->isStartElement()) {
            processNoteNode();
            recCnt++;
            if (!cmdline)
                progress->setValue(recCnt);
        }
        if (reader->name().toString().toLower() == "notebook" && reader->isStartElement() && (backup || importNotebooks)) {
            processNotebookNode();
        }
        if (reader->name().toString().toLower() == "tag" && reader->isStartElement() && (backup || importTags)) {
            processTagNode();
        }
        if (reader->name().toString().toLower() == "savedsearch" && reader->isStartElement() && backup) {
            processSavedSearchNode();
        }
        if (reader->name().toString().toLower() == "linkednotebook" && reader->isStartElement() && backup) {
            processLinkedNotebookNode();
        }
        if (reader->name().toString().toLower() == "SharedNotebook" && reader->isStartElement() && backup) {
            processSharedNotebookNode();
        }
    }
    xmlFile.close();
    query.exec("commit");

    // Now we do what is a "ahem" hack.  We need to
    // go through all of the notes & rebuild the NoteTable.  This
    // is because we may have gotten notes & tags before the notebook
    // & tag names existed.  There is probably a more efficient way
    // to do this, but since we don't really do this often this works
    // as well as any other way.

    NoteTable noteTable(global.db);
    for (qint32 i=0; i<noteList.size(); i++) {
        qint32 lid = noteTable.getLid(noteList[i]);
        if (lid > 0) {
            Note note;
            bool dirty = noteTable.isDirty(lid);
            noteTable.get(note, lid, false, false);
            noteTable.updateNoteList(lid, note, dirty, 0);
        }
    }
    query.exec("commit");
    if (!this->cmdline)
        progress->hide();
}


//***********************************************************
//* Process a <note> tag
//***********************************************************
void ImportData::processNoteNode() {
    QLOG_DEBUG() << "Processing Note Node";
    Note note;
    QUuid uuid;
    QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
    note.guid = newGuid;
    NoteMetaData meta;
    bool noteIsDirty = false;

    bool atEnd = false;
    while(!atEnd) {
        QString name = reader->name().toString().toLower();
        if (name == "guid" && !reader->isEndElement() && backup) {
            note.guid = textValue();
            noteList.append(note.guid);
        }
        if (name == "updatesequencenumber" && !reader->isEndElement()) {
            note.updateSequenceNum = textValue().toLong();
        }
        if (name == "title" && !reader->isEndElement()) {
            note.title = textValue();
        }
        if (name == "created" && !reader->isEndElement()) {
            note.created = longLongValue();
        }
        if (name == "updated" && !reader->isEndElement()) {
            note.updated = longLongValue();
        }
        if (name == "deleted" && !reader->isEndElement()) {
            note.deleted = longLongValue();
        }
        if (name == "active" && !reader->isEndElement()) {
            note.active = booleanValue();
        }
        if (name == "notebookguid" && !reader->isEndElement()) {
            note.notebookGuid = textValue();
        }
        if (name == "dirty" && !reader->isEndElement()) {
            noteIsDirty = booleanValue();
        }
        if (name == "content" && !reader->isEndElement()) {
            note.content = textValue();
        }
        if (name == "titlecolor" && !reader->isEndElement()) {
            meta.setColor(intValue());
        }
        if (name == "notetags" && (createTags || backup) && !reader->isEndElement()) {
            QStringList names, guids;
            processNoteTagList(guids, names);
            QList<QString> tagGuids;
            QList<QString> tagNames;
            for (qint32 i=0; i<guids.size(); i++) {
                tagGuids.append(guids[i]);
                tagNames.append(names[i]);
            }
            note.tagNames = tagNames;
            note.tagGuids = tagGuids;
        }
        if (name == "noteattributes" && !reader->isEndElement()) {
            processNoteAttributes(note.attributes);
        }
        if (name == "noteresource" && !reader->isEndElement()) {
            Resource newRes;
            processResource(newRes);
            newRes.noteGuid = note.guid;
            if (!backup)
                newRes.updateSequenceNum = 0;
            QList<Resource> resources;
            if (note.resources.isSet())
                resources = note.resources;
            resources.append(newRes);
            note.resources = resources;
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "note" && reader->isEndElement())
            atEnd = true;
    }

    // Loop through the resources & make sure they all have the
    // proper guid for this note
    QList<Resource> resources;
    if (note.resources.isSet())
        resources = note.resources;
    for (int i=0; i<resources.size(); i++) {
        resources[i].noteGuid = note.guid;
    }
    note.resources = resources;
    NoteTable noteTable(global.db);
    if (backup)
        noteTable.add(0,note, noteIsDirty);
    else {
        note.updateSequenceNum = 0;
        if (notebookGuid != NULL)
            note.notebookGuid = notebookGuid;
        noteTable.add(0,note, true);
        if (metaData.contains(note.guid)) {
            QLOG_ERROR() << "ERROR IN IMPORTING DATA:  Metadata not yet supported";
        }
    }
    return;
}


//***********************************************************
//* Process a <noteresource> node.
//***********************************************************
void ImportData::processResource(Resource &resource) {
    QLOG_DEBUG() << "Processing Resource Node";

    bool atEnd = false;
    //bool isDirty = false;

    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "guid") {
                resource.guid = textValue();
            }
            if (!backup) {
                QUuid uuid;
                QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
                resource.guid = g;
            }
            if (name == "noteguid") {
                QString tx = textValue();
                resource.noteGuid = tx;
            }
            if (name == "updatesequencenumber") {
                resource.updateSequenceNum = intValue();
            }
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
//            if (name == "dirty")
//                isDirty = booleanValue();
            if (name == "data") {
                Data data;
                resource.data = data;
                processData("Data", resource.data);
            }
            if (name == "alternatedata") {
                processData("AlternateData", resource.data);
            }
            if (name == "recognitiondata") {
                processData("RecognitionData", resource.recognition);
            }
            if (name == "noteresourceattributes") {
                processResourceAttributes(resource.attributes);
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "noteresource" && reader->isEndElement())
            atEnd = true;
    }
}



//***********************************************************
//* Process any type of data node
//***********************************************************
void ImportData::processData(QString nodeName, Data &data) {
    QLOG_DEBUG() << "Processing Data Node";
    nodeName.toLower();
    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
//            if (name == "size") {
//                data.size = intValue();
//                data.__isset.size = true;
//            }
            if (name == "body") {
                QString x = textValue();
                QByteArray bin = QByteArray::fromHex(x.toLocal8Bit());
                data.body.clear();
                data.body = bin;

                QCryptographicHash md5hash(QCryptographicHash::Md5);
                QByteArray hash = md5hash.hash(bin, QCryptographicHash::Md5);
                QLOG_DEBUG() << "Actual:" << hash.toHex();

                data.bodyHash.clear();
                data.bodyHash = hash;


                data.size = bin.size();
            }
//            if (name == "bodyhash") {
//                QByteArray hexData = textValue().toLocal8Bit();
//                QLOG_DEBUG() << hexData;
//                //QByteArray hexData;
//                //hexData.append(textValue());
//                QByteArray binData = QByteArray::fromHex(hexData);
//                QLOG_DEBUG() << QByteArray::fromHex(hexData);
//                data.bodyHash.clear();
//                data.bodyHash.append(binData.data(), binData.size());
//                data.__isset.bodyHash = true;
//            }
        }
        reader->readNext();
        QString nName = reader->name().toString().toLower();
        if (nName == nodeName.toLower() && reader->isEndElement())
            atEnd = true;
    }
}



//***********************************************************
//* Process a node that has the <noteresourceattribute>
//***********************************************************
void ImportData::processResourceAttributes(ResourceAttributes &attributes) {
    QLOG_DEBUG() << "Processing Resource Attributes Node";

    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "cameramake") {
                attributes.cameraMake = textValue();
            }
            if (name == "cameramodel") {
                attributes.cameraModel = textValue();
            }
            if (name == "filename") {
                attributes.fileName = textValue();
            }
            if (name == "recotype") {
                attributes.recoType = textValue();
            }
            if (name  == "sourceurl") {
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
            if (name == "clientwillindex") {
                attributes.clientWillIndex = booleanValue();
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "noteresourceattribute" && reader->isEndElement())
            atEnd = true;
    }
}


//* Process the  list of tags for a note
void ImportData::processNoteTagList(QStringList &guidList, QStringList &names) {
    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "guid")
                guidList.append(textValue());
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "notetags" && reader->isEndElement())
            atEnd = true;
    }

    // Now look for any matches with existing ones.  If they don't
    // exist we create a dummy record
    TagTable tagTable(global.db);
    for (qint32 i=0; i<guidList.size(); i++) {
        qint32 lid = tagTable.getLid(guidList[i]);
        if (lid == 0) {
            Tag newTag;
            newTag.guid = guidList[i];
            newTag.name = "newtag";
            tagTable.add(0, newTag, false, 0);
            names.append(newTag.name);
        } else {
            Tag tag;
            tagTable.get(tag, lid);
            names.append(tag.name);
        }
    }
}


//***********************************************************
//* Process an <attributes> node
//***********************************************************
void ImportData::processNoteAttributes(NoteAttributes &attributes) {
    QLOG_DEBUG() << "Processing Note Attribute Node";


    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "author" && !reader->isEndElement()) {
                attributes.author = textValue();
            }
            if (name == "sourceurl" && !reader->isEndElement()) {
                attributes.sourceURL = textValue();
            }
            if (name == "source" && !reader->isEndElement()) {
                attributes.source = textValue();
            }
            if (name == "sourceapplication" && !reader->isEndElement()) {
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
            if (name == "subjectdate" && !reader->isEndElement()) {
                attributes.subjectDate = longLongValue();
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "noteattributes" && reader->isEndElement())
            atEnd = true;
    }

    return;
}


//***********************************************************
//* Process a <synchronization> node
//***********************************************************
void ImportData::processSynchronizationNode() {
    QLOG_DEBUG() << "Processing Synchronization Node";

    bool atEnd = false;
    UserTable userTable(global.db);
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "updatesequencenumber") {
                userTable.updateLastSyncNumber(intValue());
            }
            if (name == "lastsequencedate")
                userTable.updateLastSyncDate(longValue());
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "synchronization" && reader->isEndElement())
            atEnd = true;
    }

}


//***********************************************************
//* Process a <savedsearch> node.
//***********************************************************
void ImportData::processSavedSearchNode() {
    QLOG_DEBUG() << "Processing Saved Search Node";

    SavedSearch search;
    bool searchIsDirty = false;
    SearchTable searchTable(global.db);

    bool atEnd = false;

    // Keep going until we hit </savedsearch>
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "guid")  {
                search.guid = textValue();
            }
            if (name == "name") {
                search.name = textValue();
            }
            if (name == "updatesequencenumber") {
                search.updateSequenceNum = intValue();
            }
            if (name == "query") {
                search.query = textValue();
            }
            if (name == "dirty") {
                if (booleanValue())
                    searchIsDirty = true;
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "savedsearch" && reader->isEndElement())
            atEnd = true;
    }

    // Add this search to the table
    searchTable.add(0,search, searchIsDirty);
    return;
}


//***********************************************************
//* Process a shared notebook
//***********************************************************
void ImportData::processLinkedNotebookNode() {
    QLOG_DEBUG() << "Processing Linked Notebook Node";

    LinkedNotebook  linkedNotebook;
//    bool linkedNotebookIsDirty = false;

    QLOG_ERROR() << "Linked notebook database support not implemented yet";

    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "guid") {
                linkedNotebook.guid = textValue();
            }
            if (name == "shardid") {
                linkedNotebook.shardId = textValue();
            }
            if (name == "updatesequencenumber") {
                linkedNotebook.updateSequenceNum = intValue();
            }
            if (name == "sharekey") {
                linkedNotebook.shareKey = textValue();
            }
            if (name == "sharename") {
                linkedNotebook.shareName = textValue();
            }
            if (name == "uri") {
                linkedNotebook.uri = textValue();
            }
            if (name == "username") {
                linkedNotebook.username = textValue();
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "linkednotebook" && reader->isEndElement())
            atEnd = true;		}
    return;
}



//***********************************************************
//* Import a shared notebook
//***********************************************************
void ImportData::processSharedNotebookNode() {
    QLOG_DEBUG() << "Processing Shared Notebook Node";


    SharedNotebook sharedNotebook;
    //bool sharedNotebookIsDirty = false;

    QLOG_ERROR() << "Shared notebook database support not implemented yet";

    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "id") {
                sharedNotebook.id = intValue();
            }
            if (name == "userid") {
                sharedNotebook.userId = intValue();
            }
            if (name == "email") {
                sharedNotebook.email = textValue();
            }
            if (name == "notebookguid") {
                sharedNotebook.notebookGuid = textValue();
            }
            if (name == "sharekey") {
                sharedNotebook.shareKey = textValue();
            }
            if (name == "username") {
                sharedNotebook.username = textValue();
            }
            if (name == "servicecreated") {
                sharedNotebook.serviceCreated = longValue();
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "sharednotebook" && reader->isEndElement())
            atEnd = true;		}
    return;
}



//***********************************************************
//* Process a <notebook> node
//***********************************************************
void ImportData::processNotebookNode() {
    QLOG_DEBUG() << "Processing Notebook Node";


    Notebook notebook;
    bool notebookIsDirty = false;
    bool notebookIsLocal = false;
    Publishing publishing;
//    bool notebookIsReadOnly = false;

//    notebookIcon = null;
    bool atEnd = false;

    // Loop through until we hit </notebook>
    while(!atEnd) {
        if (backup || importNotebooks) {
            if (reader->isStartElement()) {
                QString name = reader->name().toString().toLower();
                if (name == "guid") {
                    notebook.guid = textValue();
                }
                if (name == "name") {
                    notebook.name = textValue();
                }
                if (name == "updatesequencenumber") {
                    notebook.updateSequenceNum = intValue();
                }
                if (name == "servicecreated") {
                    notebook.serviceCreated = longValue();
                }
                if (name == "serviceupdated") {
                    notebook.serviceUpdated = longValue();
                }
                if (name == "defaultnotebook") {
                    notebook.defaultNotebook = booleanValue();
                }
                if (name == "dirty") {
                    if (booleanValue())
                        notebookIsDirty = true;
                }
                if (name == "localnotebook") {
                    if (booleanValue())
                        notebookIsLocal = true;
                }
                if (name == "publishingpublicdescription") {
                    publishing.publicDescription = textValue();
                }
                if (name == "publishinguri") {
                    publishing.uri = textValue();
                }
                if (name == "publishingorder") {
                    //notebook->publishing.order =
                      //      NoteSortOrder.;
                    QLOG_DEBUG() << "!!!!!!!!!!!! PublishingOrder not completed in import";
                }
                if (name == "PublishingAscending") {
                    if (booleanValue())
                        publishing.ascending = true;
                    else
                        publishing.ascending = false;
                }
                if (name == "icon") {
                    //byte[] b = textValue().getBytes();   // data binary
                    //QByteArray hexData = new QByteArray(b);
                    //QByteArray binData = new QByteArray(QByteArray.fromHex(hexData));
                    //notebookIcon = new QIcon(QPixmap.fromImage(QImage.fromData(binData)));
                }
                if (name == "stack") {
                    notebook.stack = textValue();
                }
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "notebook" && reader->isEndElement())
            atEnd = true;
    }
    notebook.publishing = publishing;

    // We are at the end.  We should have a valid notebook now
    NotebookTable notebookTable(global.db);

    // Check if there is a notebook by this name already.
    // If one exists, we treat this as an update
    qint32 lid = notebookTable.findByName(notebook.name);
    if (lid <= 0) {
        lid = notebookTable.getLid(notebook.guid);
    }
    if (lid <= 0) {
        notebookTable.add(lid,notebook,notebookIsDirty, notebookIsLocal);
    } else {
        qint32 oldLid = notebookTable.getLid(notebook.guid);
        if (oldLid != lid)
            notebookTable.merge(oldLid, lid);
        notebookTable.sync(lid, notebook);
//        notebookTable.updateGuid(lid, notebook.guid);
        notebookTable.setDirty(lid, notebookIsDirty);
    }
    return;
}



//***********************************************************
//* This will take a <tag> node and parse out the values.
//***********************************************************
void ImportData::processTagNode() {
    QLOG_DEBUG() << "Processing Tag Node";

    Tag tag;
    bool tagIsDirty = false;
    bool atEnd = false;

    // Loop through until we are at the end of the </tag>
    while(!atEnd) {
        if (backup || importTags) {
            if (reader->isStartElement()) {
                QString name = reader->name().toString().toLower();
                if (name == "guid") {
                    tag.guid = textValue();
                }
                if (name == "name") {
                    tag.name = textValue();
                }
                if (name == "updatesequencenumber") {
                    tag.updateSequenceNum = intValue();
                }
                if (name == "parentguid") {
                    tag.parentGuid = textValue();
                }
                if (name == "Dirty") {
                    if (booleanValue())
                        tagIsDirty = true;
                    else
                        tagIsDirty = false;
                }
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "tag" && reader->isEndElement())
            atEnd = true;
    }

    // We have a good tag, now let's save it to the database
    TagTable tagTable(global.db);
    QString name(tag.name);

    // Check if we have a tag by this name already.  If we
    // do then we treat this as an update.
    qint32 lid = tagTable.findByName(name,0);
    if (lid <= 0) {
        lid = tagTable.getLid(tag.guid);
    }
    if (lid <= 0) {
        tagTable.add(0, tag,tagIsDirty, 0);
    } else {
//        qint32 oldLid = tagTable.getLid(tag.guid);
//        if (oldLid != lid)
//            tagTable.merge(oldLid, lid);
        tagTable.sync(lid, tag, 0);
//        notebookTable.updateGuid(lid, notebook.guid);
        tagTable.setDirty(lid, tagIsDirty);
    }
    return;
}




//***********************************************************
//* take a node's value and return the text
//***********************************************************
QString ImportData::textValue() {
    reader->readNext();
    return reader->text().toString();
}


//***********************************************************
//* take a node's value and return the integer value
//***********************************************************
qint32 ImportData::intValue() {
    return textValue().toLong();
}


//***********************************************************
//* take a node's value and return a long value
//***********************************************************
long ImportData::longValue() {
    return textValue().toLong();
}


//***********************************************************
//* take a node's value and return longlong.  This is useful
//* for date values.
//***********************************************************
qlonglong ImportData::longLongValue() {
    return textValue().toLongLong();
}


//***********************************************************
//* take a node's value and return a double value
//***********************************************************
double ImportData::doubleValue() {
    return textValue().toDouble();
}


//***********************************************************
//* take a node's value and return true/false based upon
//* its value
//***********************************************************
bool ImportData::booleanValue() {
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
short ImportData::shortValue() {
    return textValue().toShort();
}


//***********************************************************
//* Set the notebook GUID for any imported notes.  This is
//* only useful when importing a note.  When doing a restore
//* we don't use this.
//***********************************************************
void ImportData::setNotebookGuid(QString g) {
    notebookGuid = g;
}

//***********************************************************
//* This is used to retrieve any errors that occur when
//* tying to restore or import from a nnex file
//***********************************************************
QString ImportData::getErrorMessage() {
    return errorMessage;
}


//**********************************************************
//* The "Cancel" button was pressed
//**********************************************************
void ImportData::cancel() {
    stopNow = true;
}
