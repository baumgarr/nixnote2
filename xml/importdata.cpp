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
#include "sql/resourcetable.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include "sql/tagtable.h"
#include "sql/searchtable.h"
#include "sql/usertable.h"
#include "global.h"

extern Global global;


//***********************************************************
//* Constructor.  If full == true then this is a restore.
//* if full=false then this is considered an import.  The
//* only real difference is that an import automatically
//* gets a new guid and is always considered "dirty"
//* where an import will keep the guid and will obey the
//* <dirty> tag in the note.
//***********************************************************
ImportData::ImportData(bool full)
{
    importTags = false;
    importNotebooks = false;
    backup = full;
    createTags = false;
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
    if (!xmlFile.open(QIODevice::ReadOnly)) {
        lastError = 16;
        errorMessage = "Cannot open file.";
    }

    reader = new QXmlStreamReader(&xmlFile);
    QSqlQuery query(*global.db);
    //query.exec("begin");
    while (!reader->atEnd()) {
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
                errorMessage = "This is an export file, not a backup file";
                return;
            }
            if (type.toLower() == "export" && backup) {
                lastError = 5;
                errorMessage = "This is a backup file, not an export file";
                return;
            }
        }
        if (reader->name().toString().toLower() == "synchronization" && reader->isStartElement() && backup) {
            processSynchronizationNode();
        }
        if (reader->name().toString().toLower() == "note" && reader->isStartElement()) {
            processNoteNode();
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
    //query.exec("commit");

    // Now we do what is a "ahem" hack.  We need to
    // go through all of the notes & rebuild the NoteTable.  This
    // is because we may have gotten notes & tags before the notebook
    // & tag names existed.  There is probably a more efficient way
    // to do this, but since we don't really do this often this works
    // as well as any awy.

    NoteTable noteTable;
    for (qint32 i=0; i<noteList.size(); i++) {
        qint32 lid = noteTable.getLid(noteList[i]);
        if (lid > 0) {
            Note note;
            bool dirty = noteTable.isDirty(lid);
            noteTable.get(note, lid, false, false);
            noteTable.updateNoteList(lid, note, dirty);
        }
    }

}


//***********************************************************
//* Process a <note> tag
//***********************************************************
void ImportData::processNoteNode() {
    Note note;
    QUuid uuid;
    QString newGuid = uuid.createUuid().toString().replace("{", "").replace("}", "");
    note.guid = newGuid.toStdString();
    NoteMetaData meta;
    note.__isset.guid = true;
    bool noteIsDirty = false;

    bool atEnd = false;
    while(!atEnd) {
        QString name = reader->name().toString().toLower();
        if (name == "guid" && !reader->isEndElement()) {
            note.guid = textValue().toStdString();
            note.__isset.guid = true;
            noteList.append(QString::fromStdString(note.guid));
        }
        if (name == "updatesequencenumber" && !reader->isEndElement()) {
            note.updateSequenceNum = textValue().toLong();
            note.__isset.updateSequenceNum = true;
        }
        if (name == "title" && !reader->isEndElement()) {
            note.title = textValue().toStdString();
            note.__isset.title = true;
        }
        if (name == "created" && !reader->isEndElement()) {
            note.created = longLongValue();
            note.__isset.created = true;
        }
        if (name == "updated" && !reader->isEndElement()) {
            note.updated = longLongValue();
            note.__isset.updated = true;
        }
        if (name == "deleted" && !reader->isEndElement()) {
            note.deleted = longLongValue();
            note.__isset.deleted = true;
        }
        if (name == "active" && !reader->isEndElement()) {
            note.active = booleanValue();
            note.__isset.active = true;
        }
        if (name == "notebookguid" && !reader->isEndElement()) {
            note.notebookGuid = textValue().toStdString();
            note.__isset.notebookGuid = true;
        }
        if (name == "dirty" && !reader->isEndElement()) {
            noteIsDirty = booleanValue();
        }
        if (name == "content" && !reader->isEndElement()) {
            note.content = textValue().toStdString();
            note.__isset.content = true;
        }
        if (name == "titlecolor" && !reader->isEndElement()) {
            meta.setColor(intValue());
        }
        if (name == "notetags" && (createTags || backup) && !reader->isEndElement()) {
            QStringList names, guids;
            processNoteTagList(guids, names);
            for (qint32 i=0; i<guids.size(); i++) {
                note.tagGuids.push_back(guids[i].toStdString());
                note.tagNames.push_back(names[i].toStdString());
                note.__isset.tagGuids = true;
                note.__isset.tagNames = true;
            }
        }
        if (name == "noteattributes" && !reader->isEndElement()) {
            processNoteAttributes(note.attributes);
            note.__isset.attributes = true;
        }
        if (name == "noteresource" && !reader->isEndElement()) {
            Resource newRes;
            processResource(newRes);
            note.resources.push_back(newRes);
            note.__isset.resources = true;
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
    NoteTable noteTable;
    if (backup)
        noteTable.add(0,note, noteIsDirty);
    else {
        note.updateSequenceNum = 0;
        if (notebookGuid != NULL)
            note.notebookGuid = notebookGuid.toStdString();
        for (unsigned int i=0; i<note.resources.size(); i++) {
             note.resources[i].updateSequenceNum = 0;
        }
        noteTable.add(0,note, true);
        if (metaData.contains(QString(note.guid.c_str()))) {
            QLOG_ERROR() << "ERROR IN IMPORTING DATA:  Metadata not yet supported";
        }
    }
    return;
}


//***********************************************************
//* Process a <noteresource> node.
//***********************************************************
void ImportData::processResource(Resource &resource) {
    bool atEnd = false;
    bool isDirty;

    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "guid") {
                resource.guid = textValue().toStdString();
                resource.__isset.guid = true;
            }
            if (!backup) {
                QUuid uuid;
                QString g =  uuid.createUuid().toString().replace("{","").replace("}","");
                resource.guid = g.toStdString();
                resource.__isset.guid = true;
            }
            if (name == "noteguid") {
                QString tx = textValue();
                resource.noteGuid = tx.toStdString();
                resource.__isset.noteGuid = true;
            }
            if (name == "updatesequencenumber") {
                resource.updateSequenceNum = intValue();
                resource.__isset.updateSequenceNum = true;
            }
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
            if (name == "dirty")
                isDirty = booleanValue();
            if (name == "data") {
                processData("Data", resource.data);
                resource.__isset.data = true;
            }
            if (name == "alternatedata") {
                processData("AlternateData", resource.data);
                resource.__isset.alternateData = true;
            }
            if (name == "recognitiondata") {
                processData("RecognitionData", resource.recognition);
                resource.__isset.recognition = true;
            }
            if (name == "noteresourceattributes") {
                processResourceAttributes(resource.attributes);
                resource.__isset.attributes = true;
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
    nodeName.toLower();
    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "size") {
                data.size = intValue();
                data.__isset.size = true;
            }
            if (name == "body") {
                QString x = textValue();
                QByteArray bin = QByteArray::fromHex(x.toLocal8Bit());
                data.body.clear();
                data.body.append(bin.data(), bin.size());
                data.__isset.body = true;
            }
            if (name == "bodyhash") {
               QByteArray hexData = textValue().toLocal8Bit();
                QByteArray binData = QByteArray::fromHex(hexData);
                data.bodyHash.clear();
                data.bodyHash.append(binData.data(), binData.size());
                data.__isset.bodyHash = true;
            }
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
    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "cameramake") {
                attributes.cameraMake = textValue().toStdString();
                attributes.__isset.cameraMake = true;
            }
            if (name == "cameramodel") {
                attributes.cameraModel = textValue().toStdString();
                attributes.__isset.cameraModel =true;
            }
            if (name == "filename") {
                attributes.fileName = textValue().toStdString();
                attributes.__isset.fileName = true;
            }
            if (name == "recotype") {
                attributes.recoType = textValue().toStdString();
                attributes.__isset.fileName = true;
            }
            if (name  == "sourceurl") {
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
            if (name == "clientwillindex") {
                attributes.clientWillIndex = booleanValue();
                attributes.__isset.clientWillIndex =true;
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
    TagTable tagTable;
    for (qint32 i=0; i<guidList.size(); i++) {
        qint32 lid = tagTable.getLid(guidList[i]);
        if (lid == 0) {
            Tag newTag;
            newTag.guid = guidList[i].toStdString();
            newTag.name = "newtag";
            newTag.__isset.name = true;
            newTag.__isset.guid = true;
            tagTable.add(0, newTag, false, 0);
            names.push_back(QString::fromStdString(newTag.name));
        } else {
            Tag tag;
            tagTable.get(tag, lid);
            names.push_back(QString::fromStdString(tag.name));
        }
    }
}


//***********************************************************
//* Process an <attributes> node
//***********************************************************
void ImportData::processNoteAttributes(NoteAttributes &attributes) {

    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "author" && !reader->isEndElement()) {
                attributes.author = textValue().toStdString();
                attributes.__isset.author = true;
            }
            if (name == "sourceurl" && !reader->isEndElement()) {
                attributes.sourceURL = textValue().toStdString();
                attributes.__isset.sourceURL = true;
            }
            if (name == "source" && !reader->isEndElement()) {
                attributes.source = textValue().toStdString();
                attributes.__isset.source = true;
            }
            if (name == "sourceapplication" && !reader->isEndElement()) {
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
            if (name == "subjectdate" && !reader->isEndElement()) {
                attributes.subjectDate = longLongValue();
                attributes.__isset.subjectDate = true;
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
    bool atEnd = false;
    UserTable userTable;
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
    SavedSearch search;
    bool searchIsDirty = false;
    SearchTable searchTable;

    bool atEnd = false;

    // Keep going until we hit </savedsearch>
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "guid")  {
                search.guid = textValue().toStdString();
                search.__isset.guid = true;
            }
            if (name == "name") {
                search.name = textValue().toStdString();
                search.__isset.name = true;
            }
            if (name == "updatesequencenumber") {
                search.updateSequenceNum = intValue();
                search.__isset.updateSequenceNum = true;
            }
            if (name == "query") {
                search.query = textValue().toStdString();
                search.__isset.query = true;
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
    LinkedNotebook  linkedNotebook;
    bool linkedNotebookIsDirty=false;

    QLOG_ERROR() << "Linked notebook database support not implemented yet";

    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "guid") {
                linkedNotebook.guid = textValue().toStdString();
                linkedNotebook.__isset.guid = true;
            }
            if (name == "shardid") {
                linkedNotebook.shardId = textValue().toStdString();
                linkedNotebook.__isset.shardId = true;
            }
            if (name == "updatesequencenumber") {
                linkedNotebook.updateSequenceNum = intValue();
                linkedNotebook.__isset.updateSequenceNum = true;
            }
            if (name == "sharekey") {
                linkedNotebook.shareKey = textValue().toStdString();
                linkedNotebook.__isset.shareKey = true;
            }
            if (name == "sharename") {
                linkedNotebook.shareName = textValue().toStdString();
                linkedNotebook.__isset.shareName = true;
            }
            if (name == "uri") {
                linkedNotebook.uri = textValue().toStdString();
                linkedNotebook.__isset.uri = true;
            }
            if (name == "username") {
                linkedNotebook.username = textValue().toStdString();
                linkedNotebook.__isset.username = true;
            }
            if (name == "dirty") {
                if (booleanValue())
                    linkedNotebookIsDirty = true;
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

    SharedNotebook sharedNotebook;
    bool sharedNotebookIsDirty = false;

    QLOG_ERROR() << "Shared notebook database support not implemented yet";

    bool atEnd = false;
    while(!atEnd) {
        if (reader->isStartElement()) {
            QString name = reader->name().toString().toLower();
            if (name == "id") {
                sharedNotebook.id = intValue();
                sharedNotebook.__isset.id = true;
            }
            if (name == "userid") {
                sharedNotebook.userId = intValue();
                sharedNotebook.__isset.userId = true;
            }
            if (name == "email") {
                sharedNotebook.email = textValue().toStdString();
                sharedNotebook.__isset.email =true;
            }
            if (name == "notebookguid") {
                sharedNotebook.notebookGuid = textValue().toStdString();
                sharedNotebook.__isset.notebookGuid = true;
            }
            if (name == "sharekey") {
                sharedNotebook.shareKey = textValue().toStdString();
                sharedNotebook.__isset.shareKey = true;
            }
            if (name == "username") {
                sharedNotebook.username = textValue().toStdString();
                sharedNotebook.__isset.username = true;
            }
            if (name == "servicecreated") {
                sharedNotebook.serviceCreated = longValue();
                sharedNotebook.__isset.serviceCreated = true;
            }
            if (name == "dirty") {
                if (booleanValue())
                    sharedNotebookIsDirty = true;
                else
                    sharedNotebookIsDirty = false;
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

    Notebook notebook;
    bool notebookIsDirty = false;
    bool notebookIsLocal = false;
    bool notebookIsReadOnly = false;

//    notebookIcon = null;
    bool atEnd = false;

    // Loop through until we hit </notebook>
    while(!atEnd) {
        if (backup || importNotebooks) {
            if (reader->isStartElement()) {
                QString name = reader->name().toString().toLower();
                if (name == "guid") {
                    notebook.guid = textValue().toStdString();
                    notebook.__isset.guid = true;
                }
                if (name == "name") {
                    notebook.name = textValue().toStdString();
                    notebook.__isset.name = true;
                }
                if (name == "updatesequencenumber") {
                    notebook.updateSequenceNum = intValue();
                    notebook.__isset.updateSequenceNum = true;
                }
                if (name == "servicecreated") {
                    notebook.serviceCreated = longValue();
                    notebook.__isset.serviceCreated = true;
                }
                if (name == "serviceupdated") {
                    notebook.serviceUpdated = longValue();
                    notebook.__isset.serviceUpdated = true;
                }
                if (name == "defaultnotebook") {
                    notebook.defaultNotebook = booleanValue();
                    notebook.__isset.defaultNotebook = true;
                }
                if (name == "dirty") {
                    if (booleanValue())
                        notebookIsDirty = true;
                }
                if (name == "localnotebook") {
                    if (booleanValue())
                        notebookIsLocal = true;
                }
                if (name == "readonly") {
                    if (booleanValue())
                        notebookIsReadOnly = true;
                }
                if (name == "publishingpublicdescription") {
                    notebook.publishing.publicDescription = textValue().toStdString();
                    notebook.publishing.__isset.publicDescription = true;
                }
                if (name == "publishinguri") {
                    notebook.publishing.uri = textValue().toStdString();
                    notebook.publishing.__isset.uri = true;
                }
                if (name == "publishingorder") {
                    //notebook->publishing.order =
                      //      NoteSortOrder.;
                    QLOG_DEBUG() << "!!!!!!!!!!!! PublishingOrder not completed in import";
                }
                if (name == "PublishingAscending") {
                    if (booleanValue())
                        notebook.publishing.ascending = true;
                    else
                        notebook.publishing.ascending = false;
                    notebook.publishing.__isset.ascending = true;
                }
                if (name == "icon") {
                    //byte[] b = textValue().getBytes();   // data binary
                    //QByteArray hexData = new QByteArray(b);
                    //QByteArray binData = new QByteArray(QByteArray.fromHex(hexData));
                    //notebookIcon = new QIcon(QPixmap.fromImage(QImage.fromData(binData)));
                }
                if (name == "stack") {
                    notebook.stack = textValue().toStdString();
                    notebook.__isset.stack = true;
                }
            }
        }
        reader->readNext();
        QString endName = reader->name().toString().toLower();
        if (endName == "notebook" && reader->isEndElement())
            atEnd = true;
    }

    // We are at the end.  We should have a valid notebook now
    NotebookTable notebookTable;

    // Check if there is a notebook by this name already.
    // If one exists, we treat this as an update
    qint32 lid = notebookTable.findByName(notebook.name);
    if (lid == 0)
        lid = notebookTable.getLid(notebook.guid);
    if (lid == 0) {
        notebookTable.add(lid,notebook,notebookIsDirty, notebookIsLocal);
    } else {
        notebookTable.sync(lid, notebook);
        notebookTable.setDirty(lid, notebookIsDirty);
    }
    return;
}



//***********************************************************
//* This will take a <tag> node and parse out the values.
//***********************************************************
void ImportData::processTagNode() {
    Tag tag;
    bool tagIsDirty = false;
    bool atEnd = false;

    // Loop through until we are at the end of the </tag>
    while(!atEnd) {
        if (backup || importTags) {
            if (reader->isStartElement()) {
                QString name = reader->name().toString().toLower();
                if (name == "guid") {
                    tag.guid = textValue().toStdString();
                    tag.__isset.guid = true;
                }
                if (name == "name") {
                    tag.name = textValue().toStdString();
                    tag.__isset.name = true;
                }
                if (name == "updatesequencenumber") {
                    tag.updateSequenceNum = intValue();
                    tag.__isset.updateSequenceNum =true;
                }
                if (name == "parentguid") {
                    tag.parentGuid = textValue().toStdString();
                    tag.__isset.parentGuid = true;
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
    TagTable tagTable;
    QString name(tag.name.c_str());

    // Check if we have a tag by this name already.  If we
    // do then we treat this as an update.
    qint32 lid = tagTable.findByName(name,0);
    if (lid == 0)
        lid = tagTable.getLid(tag.guid);
    if (lid == 0)
        tagTable.add(0, tag,tagIsDirty, 0);
    else {
        tagTable.sync(lid, tag, 0);
        tagTable.setDirty(lid,tagIsDirty);
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
