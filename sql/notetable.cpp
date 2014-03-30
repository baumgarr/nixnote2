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

#include "notetable.h"
#include "resourcetable.h"
#include "configstore.h"
#include "notebooktable.h"
#include "linkednotebooktable.h"
#include "sql/nsqlquery.h"
#include "tagtable.h"
#include "global.h"

#include <QSqlTableModel>
#include <QtXml>
#include "html/tagscanner.h"

extern Global global;

// Default constructor
NoteTable::NoteTable(QSqlDatabase *db)
{
    this->db = db;
}



// Given a note's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void NoteTable::updateGuid(qint32 lid, Guid &guid) {
    QLOG_TRACE() << "Entering NoteTable::updateNoteGuid()";

    NSqlQuery query(*db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.exec();

    QLOG_TRACE() << "Leaving NoteTable::updateNoteGuid()";
}




// Synchronize a new note with what is in the database.  We basically
// just delete the old one & give it a new entry
void NoteTable::sync(Note &note, qint32 account) {
    sync(0, note, account);
}



// Synchronize a new note with what is in the database.  We basically
// just delete the old one & give it a new entry
void NoteTable::sync(qint32 lid, Note &note, qint32 account) {
   // QLOG_TRACE() << "Entering NoteTable::sync()";

    if (lid > 0) {
        NSqlQuery query(*db);

        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();

        ResourceTable resTable(db);
        resTable.expungeByNote(lid);
    } else {
        ConfigStore cs(db);
        lid = cs.incrementLidCounter();
    }

    add(lid, note, false, account);
    setThumbnailNeeded(lid, true);

    //QLOG_TRACE() << "Leaving NoteTable::sync()";
}




// Given a note's GUID, we return the LID
qint32 NoteTable::getLid(QString guid) {

    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":data", guid);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}


// Given a note's lid, return the guid
QString NoteTable::getGuid(qint32 lid) {

    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next())
        return query.value(0).toString();
    else
        return "";
}


// Given a note's GUID, we return the LID
qint32 NoteTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new note to the database
qint32 NoteTable::add(qint32 l, Note &t, bool isDirty, qint32 account) {
    ResourceTable resTable(db);
    ConfigStore cs(db);
    NSqlQuery query(*db);
    NSqlQuery trans(*db);
    trans.exec("begin");
    qint32 position;
    qint32 lid = l;
    qint32 notebookLid = account;

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    if (lid <= 0)
        lid = cs.incrementLidCounter();

    QLOG_DEBUG() << "Adding note("<<lid<<") " << QString::fromStdString(t.title);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":data", QString::fromStdString(t.guid));
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":data", true);
    query.exec();

    if (t.__isset.title) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_TITLE);
        query.bindValue(":data", QString::fromStdString(t.title.c_str()));
        query.exec();
    }

    if (t.__isset.content) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT);
        QByteArray b;
        b.append(QString::fromStdString(t.content).toAscii());
        query.bindValue(":data", b);
        query.exec();
    }

    if (t.__isset.contentHash) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT_HASH);
        query.bindValue(":data", QString::fromStdString(t.contentHash));
        query.exec();
    }

    if (t.__isset.contentLength) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT_LENGTH);
        query.bindValue(":data", t.contentLength);
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":data", t.updateSequenceNum);
    query.exec();

    if (isDirty) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":data", isDirty);
        query.exec();
    }

    if (t.__isset.created) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CREATED_DATE);
        query.bindValue(":data", QVariant::fromValue(t.created));
        query.exec();
    }

    if (t.__isset.updated) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_UPDATED_DATE);
        query.bindValue(":data", QVariant::fromValue(t.updated));
        query.exec();
    }

    if (t.__isset.deleted) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_DELETED_DATE);
        query.bindValue(":data", QVariant::fromValue(t.deleted));
        query.exec();
    }

    if (t.__isset.active) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ACTIVE);
        query.bindValue(":data", QVariant::fromValue(t.active));
        query.exec();
    }

    if (t.__isset.notebookGuid) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_NOTEBOOK_LID);
        NotebookTable notebookTable(db);
        LinkedNotebookTable linkedTable(db);
        if (account > 0)
            notebookLid = account;
        else
            notebookLid = 0;
        if (notebookLid <= 0)
            notebookLid = notebookTable.getLid(QString::fromStdString(t.notebookGuid));
        if (notebookLid <= 0) {
            notebookLid = linkedTable.getLid(QString::fromStdString(t.notebookGuid));
        }

        // If not found, we insert one to avoid problems.  We'll probably get the real data later
        if (notebookLid <= 0) {
            notebookLid = cs.incrementLidCounter();
            Notebook notebook;
            notebook.guid = t.notebookGuid;
            notebook.name = "<Missing Notebook>";
            notebook.__isset.guid = true;
            notebook.__isset.name = true;
            notebookTable.add(notebookLid, notebook, false, false);
        }
        query.bindValue(":data", notebookLid);
        query.exec();
    }

    for (unsigned int i=0; t.__isset.tagGuids && i<t.tagGuids.size(); i++) {
        TagTable tagTable(db);
        qint32 tagLid = tagTable.getLid(t.tagGuids.at(i));
        if (tagLid == 0) {
            // create a dummy tag to avoid later problems
            Tag newTag;
            newTag.guid = t.tagGuids.at(i);
            newTag.name = "";
            newTag.__isset.guid = true;
            newTag.__isset.name = true;
            tagLid = cs.incrementLidCounter();
            tagTable.add(tagLid, newTag, false, 0);
        }

        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_TAG_LID);
        query.bindValue(":data", tagLid);
        query.exec();
    }

    for (unsigned int i=0; t.__isset.resources && i<t.resources.size(); i++) {
        qint32 resLid;
        resLid = 0;
        Resource *r;
        r = &t.resources[i];
        resLid = resTable.getLid(t.guid,t.resources[i].guid);
        if (resLid == 0)
            resLid = cs.incrementLidCounter();
        resTable.add(resLid, t.resources[i], isDirty, lid);

        if (r->__isset.mime) {
            QString mime = QString::fromStdString(r->mime);
            if (!mime.startsWith("image/") && mime != "vnd.evernote.ink") {
                query.bindValue(":lid", lid);
                query.bindValue(":key", NOTE_HAS_ATTACHMENT);
                query.bindValue(":data", true);
                query.exec();
            }
        }
    }

    if (t.__isset.attributes) {
        if (t.attributes.__isset.subjectDate) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SUBJECT_DATE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.subjectDate));
            query.exec();
        }
        if (t.attributes.__isset.latitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_LATITUDE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.latitude));
            query.exec();
        }
        if (t.attributes.__isset.latitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_LONGITUDE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.longitude));
            query.exec();
        }
        if (t.attributes.__isset.altitude) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_ALTITUDE);
            query.bindValue(":data", QVariant::fromValue(t.attributes.altitude));
            query.exec();
        }
        if (t.attributes.__isset.author) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
            query.bindValue(":data", QString::fromStdString(t.attributes.author));
            query.exec();
        }
        if (t.attributes.__isset.source) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
            query.bindValue(":data", QString::fromStdString(t.attributes.source));
            query.exec();
        }
        if (t.attributes.__isset.sourceURL) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_URL);
            query.bindValue(":data", QString::fromStdString(t.attributes.sourceURL));
            query.exec();
        }
        if (t.attributes.__isset.sourceApplication) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
            query.bindValue(":data", QString::fromStdString(t.attributes.sourceApplication));
            query.exec();
        }
        if (t.attributes.__isset.shareDate) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SHARE_DATE);
            query.bindValue(":data",QVariant::fromValue(t.attributes.shareDate));
            query.exec();
        }
        if (t.attributes.__isset.placeName) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_PLACE_NAME);
            query.bindValue(":data", QString::fromStdString(t.attributes.placeName));
            query.exec();
        }
        if (t.attributes.__isset.contentClass) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
            query.bindValue(":data", QString::fromStdString(t.attributes.contentClass));
            query.exec();
        }
        if (t.attributes.__isset.reminderTime) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
            query.bindValue(":data",QVariant::fromValue(t.attributes.reminderTime));
            query.exec();
        }
        if (t.attributes.__isset.reminderDoneTime) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
            query.bindValue(":data",QVariant::fromValue(t.attributes.reminderDoneTime));
            query.exec();
        }
        if (t.attributes.__isset.reminderOrder) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_ORDER);
            query.bindValue(":data",QVariant::fromValue(t.attributes.reminderOrder));
            query.exec();
        }
    }

    // No determine some attributes of the note based upon the content
    // This should probably happen every time a note changes? Or at least something simular:
    QString content;
    if (t.__isset.content)
        content = QString::fromStdString(t.content);
    else
        content = "";

    position = content.indexOf("<en-crypt");
    if (position > 0) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_HAS_ENCRYPT);
        query.bindValue(":data", true);
        query.exec();
    }
    position = content.indexOf("<en-todo");
    if (position > 0) {
        position = content.indexOf("<en-todo checked=\"true\"");
        if (position > 0) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_HAS_TODO_COMPLETED);
            query.bindValue(":data", true);
            query.exec();
        }
        position = qMax(content.indexOf("<en-todo checked=\"false\""), content.indexOf("<en-todo>"));
        if (position > 0) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_HAS_TODO_UNCOMPLETED);
            query.bindValue(":data", true);
            query.exec();
        }
    }
    trans.exec("commit");

    updateNoteList(lid, t, isDirty, account);
    return lid;
}



qint32 NoteTable::addStub(QString noteGuid) {
    ConfigStore cs(db);
    NSqlQuery query(*db);

    qint32 lid = 0;
    lid = getLid(noteGuid);

    if (lid <= 0)
        lid = cs.incrementLidCounter();

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":data", noteGuid);
    query.exec();
}


bool NoteTable::updateNoteList(qint32 lid, Note &t, bool isDirty, qint32 notebook) {

    if (lid <= 0)
        return false;

    NotebookTable notebookTable(db);
    LinkedNotebookTable linkedNotebookTable(db);
    QString notebookName = "";

    qint32 account = 0;   // By default we search in our account.  This is used later to find tag names
    qint32 notebookLid = notebook;
    if (notebookLid <= 0)
        notebookLid = notebookTable.getLid(t.notebookGuid);
    if (notebookLid <=0) {
        notebookLid = linkedNotebookTable.getLid(t.notebookGuid);
        if (notebookLid > 0)
            account = notebookLid;
    }
    if (notebookLid <=0)
        notebookLid = notebookTable.addStub(QString::fromStdString(t.notebookGuid));
    else {
        Notebook notebook;
        notebookTable.get(notebook, notebookLid);
        notebookName = QString::fromStdString(notebook.name);
    }
    // Now let's update the user table
    NSqlQuery query(*db);

    query.prepare("Delete from NoteTable where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare(QString("Insert into NoteTable (lid, title, author, ") +
                  QString("dateCreated, dateUpdated, dateSubject, dateDeleted, source, sourceUrl, sourceApplication, ") +
                  QString("latitude, longitude, altitude, reminderOrder, reminderTime, reminderDoneTime, hasEncryption, hasTodo, isDirty, size, notebook, notebookLid, tags) ") +
                  QString("Values (:lid, :title, :author, ") +
                  QString(":dateCreated, :dateUpdated, :dateSubject, :dateDeleted, :source, :sourceUrl, :sourceApplication, ") +
                  QString(":latitude, :longitude, :altitude, :reminderOrder, :reminderTime, :reminderDoneTime, :hasEncryption, :hasTodo, :isDirty, :size, :notebook, :notebookLid, :tags) ")) ;

    query.bindValue(":lid", lid);

    if (t.__isset.title)
        query.bindValue(":title", QString::fromStdString(t.title));
    else
        query.bindValue(":title", "");
    if (t.__isset.attributes && t.attributes.__isset.author)
        query.bindValue(":author", QString::fromStdString(t.attributes.author));
    else
        query.bindValue(":author", "");
    if (t.__isset.created)
        query.bindValue(":dateCreated", QVariant::fromValue(t.created));
    else
        query.bindValue(":dateCreated", 0);
    if (t.__isset.updated)
        query.bindValue(":dateUpdated", QVariant::fromValue(t.updated));
    else
        query.bindValue(":dateUpdated", 0);
    if (t.__isset.attributes && t.attributes.__isset.subjectDate)
        query.bindValue(":dateSubject", QVariant::fromValue(t.attributes.subjectDate));
    else
        query.bindValue(":dateSubject", 0);
    if (t.__isset.deleted)
        query.bindValue(":dateDeleted", QVariant::fromValue(t.deleted));
    else
        query.bindValue(":dateDeleted", 0);
    if (t.__isset.attributes && t.attributes.__isset.source)
        query.bindValue(":source", QString::fromStdString(t.attributes.source));
    else
        query.bindValue(":source", "");
    if (t.__isset.attributes && t.attributes.__isset.sourceURL)
        query.bindValue(":sourceUrl", QString::fromStdString(t.attributes.sourceURL));
    else
        query.bindValue(":sourceUrl", "");
    if (t.__isset.attributes && t.attributes.__isset.sourceApplication)
        query.bindValue(":sourceApplication", QString::fromStdString(t.attributes.sourceApplication));
    else
        query.bindValue(":sourceApplication", "");
    if (t.__isset.attributes && t.attributes.__isset.latitude)
        query.bindValue(":latitude", QVariant::fromValue(t.attributes.latitude));
    else
        query.bindValue(":latitude", 0);
    if (t.__isset.attributes && t.attributes.__isset.longitude)
        query.bindValue(":longitude", QVariant::fromValue(t.attributes.longitude));
    else
        query.bindValue(":longitude", 0);
    if (t.__isset.attributes && t.attributes.__isset.altitude)
        query.bindValue(":altitude", QVariant::fromValue(t.attributes.altitude));
    else
        query.bindValue(":altitude", 0);
    if (t.__isset.attributes && t.attributes.__isset.reminderOrder)
        query.bindValue(":reminderOrder", QVariant::fromValue(t.attributes.reminderOrder));
    else
        query.bindValue(":reminderOrder", 0);
    if (t.__isset.attributes && t.attributes.__isset.reminderTime)
        query.bindValue(":reminderTime", QVariant::fromValue(t.attributes.reminderTime));
    else
        query.bindValue(":reminderTime", 0);
    if (t.__isset.attributes && t.attributes.__isset.reminderDoneTime)
        query.bindValue(":reminderDoneTime", QVariant::fromValue(t.attributes.reminderDoneTime));
    else
        query.bindValue(":reminderDoneTime", 0);


    bool hasEncryption;
    if (t.content.find("<en-crypt") != string::npos)
        hasEncryption = true;
    else
        hasEncryption = false;
    query.bindValue(":hasEncryption", hasEncryption);
    bool hasTodo;
    if (t.content.find("<en-todo") != string::npos)
        hasTodo = true;
    else
        hasTodo = false;
    query.bindValue(":hasTodo", hasTodo);
    query.bindValue(":isDirty", isDirty);
    qlonglong size = t.content.length();
    for (unsigned int i=0; i<t.resources.size(); i++) {
        size+=t.resources[i].data.size;
    }
    query.bindValue(":size", size);

    query.bindValue(":notebook", notebookName);
    query.bindValue(":notebookLid", notebookLid);

    QString tagNames;
    QStringList sortedNames;
    for (unsigned int i=0; i<t.tagNames.size(); i++) {
        sortedNames.append(QString::fromStdString(t.tagNames.at(i)).toLower());
    }
    sortedNames.sort();

    TagTable tagTable(db);

    // We search the table to get the name in the correct case.
    // We lowercased them above to sort properly without regards
    // to case.  Now, for the note list we need the correct case
    for (int i=0; i<sortedNames.size(); i++) {
        if (i>0)
            tagNames = tagNames+", ";
        Tag currentTag;
        qint32 tagLid = tagTable.findByName(sortedNames[i], account);
        tagTable.get(currentTag, tagLid);
        tagNames = tagNames + QString::fromStdString(currentTag.name);
    }

    query.bindValue(":tags", tagNames);

    if (!query.exec()) {
        QLOG_ERROR() << "Error inserting into NoteTable: " << query.lastError();
        return false;
    }
    return true;
}




bool NoteTable::updateNotebookName(qint32 lid, QString name) {
    NSqlQuery query(*db);
    query.prepare("Update NoteTable set notebook=:name where notebooklid=:lid");
    query.bindValue(":name", name);
    query.bindValue(":lid", lid);
    return query.exec();
}


// Return a note structure given the LID
bool NoteTable::get(Note &note, qint32 lid,bool loadResources, bool loadBinary) {

    NSqlQuery query(*db);
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);

    query.exec();
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (NOTE_GUID):
            note.guid = query.value(1).toString().toStdString();
            note.__isset.guid = true;
            break;
        case (NOTE_UPDATE_SEQUENCE_NUMBER):
            note.updateSequenceNum = query.value(1).toInt();
            note.__isset.updateSequenceNum = true;
            break;
        case (NOTE_ACTIVE):
            note.active = query.value(1).toBool();
            note.__isset.active = true;
            break;
        case (NOTE_DELETED_DATE):
            note.active = query.value(1).toLongLong();
            note.__isset.deleted = true;
            break;
        case (NOTE_ATTRIBUTE_SOURCE_URL):
            note.attributes.sourceURL = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.sourceURL = true;
            break;
        case (NOTE_ATTRIBUTE_SOURCE_APPLICATION):
            note.attributes.sourceApplication = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.sourceApplication = true;
            break;
        case (NOTE_CONTENT_LENGTH):
            note.contentLength = query.value(1).toLongLong();
            note.__isset.contentLength = true;
            break;
        case (NOTE_ATTRIBUTE_LONGITUDE):
            note.attributes.longitude = query.value(1).toFloat();
            note.__isset.attributes = true;
            note.attributes.__isset.longitude = true;
            break;
        case (NOTE_TITLE):
            note.title = query.value(1).toString().toStdString();
            note.__isset.title = true;
            break;
        case (NOTE_ATTRIBUTE_SOURCE):
            note.attributes.source = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.source = true;
            break;
        case (NOTE_ATTRIBUTE_ALTITUDE):
            note.attributes.altitude = query.value(1).toFloat();
            note.__isset.attributes = true;
            note.attributes.__isset.altitude = true;
            break;
        case (NOTE_NOTEBOOK_LID): {
            qint32 notebookLid = query.value(1).toInt();
            NotebookTable ntable(db);
            QString notebookGuid;
            ntable.getGuid(notebookGuid, notebookLid);
            note.notebookGuid = notebookGuid.toStdString();
            note.__isset.notebookGuid = true;
            break;
        }
        case (NOTE_UPDATED_DATE):
            note.updated = query.value(1).toLongLong();
            note.__isset.updated = true;
            break;
        case (NOTE_CREATED_DATE):
            note.created = query.value(1).toLongLong();
            note.__isset.created = true;
            break;
        case (NOTE_ATTRIBUTE_SUBJECT_DATE):
            note.attributes.subjectDate = query.value(1).toLongLong();
            note.__isset.attributes = true;
            note.attributes.__isset.subjectDate = true;
            break;
        case (NOTE_ATTRIBUTE_LATITUDE):
            note.attributes.latitude = query.value(1).toFloat();
            note.__isset.attributes = true;
            note.attributes.__isset.latitude = true;
            break;
        case (NOTE_CONTENT):
            note.content = query.value(1).toByteArray().data();
            note.__isset.content = true;
            break;
        case (NOTE_CONTENT_HASH):
            note.contentHash = query.value(1).toString().toStdString();
            note.__isset.contentHash = true;
            break;
        case (NOTE_ATTRIBUTE_AUTHOR):
            note.attributes.author = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.author = true;
            break;
        case (NOTE_ISDIRTY):
            break;
        case (NOTE_ATTRIBUTE_SHARE_DATE) :
            note.attributes.shareDate = query.value(1).toLongLong();
            note.__isset.attributes = true;
            note.attributes.__isset.shareDate = true;
            break;
        case (NOTE_ATTRIBUTE_PLACE_NAME) :
            note.attributes.placeName = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.placeName = true;
            break;
        case (NOTE_ATTRIBUTE_CONTENT_CLASS) :
            note.attributes.contentClass = query.value(1).toString().toStdString();
            note.__isset.attributes = true;
            note.attributes.__isset.contentClass = true;
            break;
        case (NOTE_ATTRIBUTE_REMINDER_ORDER) :
            note.attributes.reminderOrder = query.value(1).toLongLong();
            note.__isset.attributes = true;
            note.attributes.__isset.reminderOrder = true;
            break;
        case (NOTE_ATTRIBUTE_REMINDER_DONE_TIME) :
            note.attributes.reminderDoneTime = query.value(1).toLongLong();
            note.__isset.attributes = true;
            note.attributes.__isset.reminderDoneTime = true;
            break;
        case (NOTE_ATTRIBUTE_REMINDER_TIME) :
            note.attributes.reminderTime = query.value(1).toLongLong();
            note.__isset.attributes = true;
            note.attributes.__isset.reminderTime = true;
            break;
        case (NOTE_TAG_LID) :
            TagTable tagTable(db);
            qint32 tagLid = query.value(1).toInt();
            Tag tag;
            tagTable.get(tag, tagLid);
            note.__isset.tagGuids = true;
            note.__isset.tagNames = true;
            note.tagGuids.push_back(tag.guid);
            note.tagNames.push_back(tag.name);
            break;
        }
    }

//    QLOG_DEBUG() << "***************** PERFORMANCE DRAG ***************";
    ResourceTable resTable(db);
    QLOG_TRACE() << "Fetching Resources? " << loadResources << " With binary? " << loadBinary;
//    QList<qint32> resList;

    QList<Resource> resources;
    resTable.getAllResources(resources, lid, loadResources, loadBinary);
    for (int i=0; i<resources.size(); i++) {
        note.__isset.resources = true;
        note.resources.push_back(resources[i]);
    }


//    if (resTable.getResourceList(resList, lid)) {
//        for (int i=0; i<resList.size(); i++) {
//            Resource resource;
//            if (loadResources) {
//                resTable.get(resource, resList[i], loadBinary);
//            } else {
//                QString resGuid = resTable.getGuid(resList[i]);
//                resource.guid = resGuid.toStdString();
//                resource.__isset.guid = true;
//            }
//            note.__isset.resources = true;
//            note.resources.push_back(resource);
//        }
//    }
    QLOG_TRACE() << "Fetched resources";

    /*
    TagScanner test;
    test.setData(QString::fromStdString(note.content));
    QList<TagScannerRecord> retval;
    int k = test.findAll(retval, QString("en-note"));
    */

    if (note.__isset.guid)
        return true;
    else
        return false;
}



// Return a note given the GUID
bool NoteTable::get(Note& note, QString guid,bool loadResources, bool loadBinary) {
    qint32 lid = getLid(guid);
    return get(note, lid, loadResources, loadBinary);
}



// Return a note given the GUID as a std::string
bool NoteTable::get(Note &note, string guid, bool loadResources, bool loadBinary) {
    qint32 lid = getLid(guid);
    return get(note, lid, loadResources, loadBinary);
}



// Return if a note is dirty given its lid
bool NoteTable::isDirty(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}


// Determine if a note is dirty given a guid
bool NoteTable::isDirty(QString guid) {
    qint32 lid = getLid(guid);
    return isDirty(lid);
}


// Determine if a note is dirty given a guid
bool NoteTable::isDirty(string guid) {
    QString g(QString::fromStdString(guid));
    return isDirty(g);
}


// Does this note exist?
bool NoteTable::exists(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}


// Determine if a note is dirty given a guid
bool NoteTable::exists(QString guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}


// Determine if a note is dirty given a guid
bool NoteTable::exists(string guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}



// Find the note LIDs for a tag
qint32 NoteTable::findNotesByTag(QList<qint32> &values, qint32 tagLid) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:tagLid");
    query.bindValue(":key", NOTE_TAG_LID);
    query.bindValue(":tagLid", tagLid);
    query.exec();
    while (query.next()) {
        values.append(query.value(0).toInt());
    }
    return values.size();
}

// Find the note LIDs for a tag
qint32 NoteTable::findNotesByTag(QList<qint32> &values, QString data) {
    TagTable tagTable(db);
    qint32 tagLid = tagTable.getLid(data);
    return findNotesByTag(values, tagLid);
}


qint32 NoteTable::findNotesByTag(QList<qint32> &values, string data) {
    return findNotesByTag(values, QString::fromStdString(data));
}


// Update the user note list with the proper tag names
void NoteTable::updateNoteListTags(qint32 noteLid, QString tags) {
    NSqlQuery query(*db);
    query.prepare("Update NoteTable set tags=:tags where lid=:lid");
    query.bindValue(":lid", noteLid);
    query.bindValue(":tags", tags);
    query.exec();
}



// Update the user's notebook name list
void NoteTable::updateNoteListNotebooks(QString guid, QString name) {
    NotebookTable notebookTable(db);
    qint32 notebookLid;
    notebookLid = notebookTable.getLid(guid);
    NSqlQuery sql2(*db);
    sql2.prepare("Update NoteTable set notebook=:note where lid=:lid");
    sql2.bindValue(":note", name);
    sql2.bindValue(":lid", notebookLid);
    sql2.exec();
}


qint32 NoteTable::getNotesWithTag(QList<qint32> &retval, QString tag) {
    NSqlQuery query(*db);
    TagTable tagTable(db);
    qint32 tagLid = tagTable.getLid(tag);
    query.prepare("Select lid data from DataStore where data=:tag and key=:key");
    query.bindValue(":tag", tagLid);
    query.bindValue(":key", NOTE_TAG_LID);
    query.exec();
    while(query.next()) {
        retval.append(query.value(0).toInt());
    }
    return retval.size();
}


void NoteTable::setIndexNeeded(qint32 lid, bool indexNeeded) {
    if (lid <= 0)
        return;

    NSqlQuery query(*db);
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":data", indexNeeded);
    query.exec();
}


qint32 NoteTable::getIndexNeeded(QList<qint32> &lids) {
    NSqlQuery query(*db);
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data='true'");
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();
    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}


void NoteTable::updateNotebook(qint32 noteLid, qint32 notebookLid, bool setAsDirty) {
    Notebook book;
    NotebookTable notebookTable(db);
    notebookTable.get(book, notebookLid);

    if (book.__isset.guid) {
        NSqlQuery query(*db);
        query.prepare("Update DataStore set data=:notebookLid where lid=:lid and key=:key;");
        query.bindValue(":notebookLid", notebookLid);
        query.bindValue(":lid", noteLid);
        query.bindValue(":key", NOTE_NOTEBOOK_LID);
        query.exec();

        if (setAsDirty) {
            setDirty(noteLid, setAsDirty);
        }

        QString bookName = QString::fromStdString(book.name);
        query.prepare("Update NoteTable set notebook=:name where lid=:lid");
        query.bindValue(":name", bookName);
        query.bindValue(":lid", noteLid);
        query.exec();

        query.prepare("Update NoteTable set notebookLid=:nlid where lid=:lid");
        query.bindValue(":nlid", notebookLid);
        query.bindValue(":lid", noteLid);
        query.exec();
    }
}



void NoteTable::updateUrl(qint32 noteLid, QString url, bool setAsDirty=false) {
    NSqlQuery query(*db);
    query.prepare("Update DataStore set data=:url where lid=:lid and key=:key;");
    query.bindValue(":url", url);
    query.bindValue(":lid", noteLid);
    query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_URL);
    query.exec();

    if (setAsDirty) {
        setDirty(noteLid, setAsDirty);
    }

    query.prepare("Update NoteTable set SourceURl=:url where lid=:lid");
    query.bindValue(":url", url);
    query.bindValue(":lid", noteLid);
    query.exec();

}



void NoteTable::updateTitle(qint32 noteLid, QString title, bool setAsDirty=false) {
    NSqlQuery query(*db);
    query.prepare("Update DataStore set data=:url where lid=:lid and key=:key;");
    query.bindValue(":url", title);
    query.bindValue(":lid", noteLid);
    query.bindValue(":key", NOTE_TITLE);
    query.exec();

    if (setAsDirty) {
        setDirty(noteLid, setAsDirty);
    }

    query.prepare("Update NoteTable set Title=:url where lid=:lid");
    query.bindValue(":url", title);
    query.bindValue(":lid", noteLid);
    query.exec();

}



void NoteTable::updateAuthor(qint32 noteLid, QString author, bool setAsDirty=false) {
    if (noteLid <=0)
        return;
    NSqlQuery query(*db);
    query.prepare("Delete from Datastore where lid=:lid and key=:key");
    query.bindValue(":lid", noteLid);
    query.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
    query.exec();

    query.prepare("Insert into Datastore(lid, key, data) values(:lid, :key, :author);");
    query.bindValue(":author", author);
    query.bindValue(":lid", noteLid);
    query.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
    query.exec();

    if (setAsDirty) {
        setDirty(noteLid, setAsDirty);
    }

    query.prepare("Update NoteTable set Author=:author where lid=:lid");
    query.bindValue(":url", author);
    query.bindValue(":lid", noteLid);
    query.exec();

}


void NoteTable::updateDate(qint32 lid, Timestamp ts, qint32 key, bool isDirty = false) {
    if (lid <= 0)
        return;

    NSqlQuery query(*db);
    query.prepare("Update DataStore set data=:ts where lid=:lid and key=:key;");
    query.bindValue(":ts", QVariant::fromValue(ts));
    query.bindValue(":lid", lid);
    query.bindValue(":key",key);
    query.exec();

    if (isDirty) {
        setDirty(lid, isDirty);
    }

    if (key == NOTE_CREATED_DATE)
        query.prepare("Update NoteTable set dateCreated=:date where lid=:lid");
    if (key == NOTE_UPDATED_DATE)
        query.prepare("Update NoteTable set dateUpdated=:date where lid=:lid");
    if (key == NOTE_DELETED_DATE)
        query.prepare("Update NoteTable set dateDeleted=:date where lid=:lid");
    if (key == NOTE_ATTRIBUTE_SUBJECT_DATE)
        query.prepare("Update NoteTable set dateSubject=:date where lid=:lid");
    if (key == NOTE_ATTRIBUTE_REMINDER_TIME) {
        query.prepare("Delete from Datastore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
        query.exec();

        query.prepare("Delete from Datastore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_ORDER);
        query.exec();

        query.prepare("Delete from Datastore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
        query.exec();

        query.prepare("Insert into Datastore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
        query.bindValue(":data", QVariant::fromValue(ts));
        query.exec();

        query.prepare("Insert into Datastore (lid, key, data) values (:lid, :key, datetime('now'))");
        query.bindValue(":lid", lid);
        query.bindValue(":key",NOTE_ATTRIBUTE_REMINDER_ORDER);
        query.exec();

        query.prepare("Update NoteTable set reminderTime=:date where lid=:lid");
    }

    query.bindValue(":date", QVariant::fromValue(ts));
    query.bindValue(":lid", lid);
    query.exec();
}



void NoteTable::removeTag(qint32 lid, qint32 tag, bool isDirty = false) {
    NSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid and key=:key and data=:tag");
    query.bindValue(":lid", lid);
    query.bindValue(":key",NOTE_TAG_LID);
    query.bindValue(":tag:", tag);
    query.exec();

    if (isDirty) {
        setDirty(lid, isDirty);
    }
    rebuildNoteListTags(lid);
}


void NoteTable::addTag(qint32 lid, qint32 tag, bool isDirty = false) {
    if (lid <= 0)
        return;

    NSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid and key=:key and data=:tag");
    query.bindValue(":lid", lid);
    query.bindValue(":key",NOTE_TAG_LID);
    query.bindValue(":tag:", tag);
    query.exec();

    query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, :tag)");
    query.bindValue(":lid", lid);
    query.bindValue(":key",NOTE_TAG_LID);
    query.bindValue(":tag:", tag);
    query.exec();

    if (isDirty) {
        setDirty(lid, isDirty);
    }
    rebuildNoteListTags(lid);
}


bool NoteTable::hasTag(qint32 noteLid, qint32 tagLid) {
    NSqlQuery query(*db);
    query.prepare("select lid from DataStore where lid=:lid and key=:key and data=:tag");
    query.bindValue(":lid", noteLid);
    query.bindValue(":key",NOTE_TAG_LID);
    query.bindValue(":tag:", tagLid);
    query.exec();
    if (query.next())
        return true;
    return false;
}


void NoteTable::rebuildNoteListTags(qint32 lid) {
    // update the note list
    QStringList tagNames;
    TagTable tagTable(db);
    NSqlQuery query(*db);
    query.prepare("select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_TAG_LID);
    query.exec();
    while (query.next()) {
        qint32 tagLid = query.value(0).toInt();
        Tag t;
        tagTable.get(t, tagLid);
        tagNames.append(QString::fromStdString(t.name));
    }
    qSort(tagNames.begin(), tagNames.end(), caseInsensitiveLessThan);
    QString tagCol;
    for (qint32 i=0; i<tagNames.size(); i++) {
        tagCol = tagCol + tagNames[i];
        if (i<tagNames.size()-1)
            tagCol=tagCol+", ";
    }
    query.prepare("update NoteTable set tags=:tags where lid=:lid");
    query.bindValue(":tags", tagCol);
    query.bindValue(":lid", lid);
    query.exec();
}



QString NoteTable::getNoteListTags(qint32 lid) {

    NSqlQuery query(*db);
    query.prepare("select tags from NoteTable where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next()) {
        return query.value(0).toString();
    }
    return "";
}


void NoteTable::setDirty(qint32 lid, bool dirty) {
    if (lid <=0)
        return;

    NSqlQuery query(*db);
    query.prepare("Update NoteTable set isDirty=:isDirty where lid=:lid");
    query.bindValue(":isDirty", dirty);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();

    if (dirty) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":data", dirty);
        query.exec();
        setIndexNeeded(lid, true);

        query.prepare("Delete from DataStore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_UPDATED_DATE);
        query.exec();

        qint64 dt = QDateTime::currentMSecsSinceEpoch();

        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :value)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_UPDATED_DATE);
        query.bindValue(":value", dt);
        query.exec();

        query.prepare("Update NoteTable set dateUpdated=:value where lid=:lid");
        query.bindValue(":lid", lid);
        query.bindValue(":value", dt);
        query.exec();

        setIndexNeeded(lid, true);
    }
}



bool NoteTable::isDeleted(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next()) {
        bool active = query.value(0).toBool();
        return !active;
    }
    return false;
}



void NoteTable::deleteNote(qint32 lid, bool isDirty=true) {
    if (lid <=0)
        return;

    NSqlQuery query(*db);
    query.prepare("delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_DELETED_DATE);
    query.bindValue(":lid", lid);
    query.exec();

    if (isDirty) {
        query.prepare("delete from DataStore where key=:key and lid=:lid");
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":lid", lid);
        query.exec();
    }

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":data", false);
    query.exec();

    query.prepare("update notetable set dateDeleted=strftime('%s','now') where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();

    if (isDirty) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":data", true);
        query.exec();

    }
}



void NoteTable::restoreNote(qint32 lid, bool isDirty=true) {
    if (lid <=0)
        return;

    NSqlQuery query(*db);
    query.prepare("delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_DELETED_DATE);
    query.bindValue(":lid", lid);
    query.exec();

    if (isDirty) {
        query.prepare("delete from DataStore where key=:key and lid=:lid");
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":lid", lid);
        query.exec();
    }

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":data", true);
    query.exec();

    query.prepare("update notetable set dateDeleted=0 where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();

    if (isDirty) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":data", true);
        query.exec();

    }
}




qint32 NoteTable::getAllDeleted(QList<qint32> &lids) {
    lids.clear();
    NSqlQuery query(*db);
    query.prepare("select lid from DataStore where key=:key and data='false'");
    query.bindValue(":key", NOTE_ACTIVE);
    query.exec();

    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}



void NoteTable::expunge(qint32 lid) {
    Note note;
    this->get(note, lid, true, false);
    ResourceTable resTable(db);
    for (unsigned int i=0; note.__isset.resources && i<note.resources.size(); i++) {
        Resource r = note.resources[i];
        resTable.expunge(r.guid);
    }

    NSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.prepare("delete from NoteTable where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
}


void NoteTable::expunge(string guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}


void NoteTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}





qint32 NoteTable::findNotesByNotebook(QList<qint32> &notes, QString guid) {
    NSqlQuery query(*db);
    qint32 notebookLid;
    NotebookTable notebookTable(db);
    notebookLid = notebookTable.getLid(guid);
    query.prepare("Select lid from DataStore where key=:key and data=:notebookLid");
    query.bindValue(":key", NOTE_NOTEBOOK_LID);
    query.bindValue(":notebookLid", notebookLid);
    query.exec();
    while (query.next()) {
        notes.append(query.value(0).toInt());
    }
    return notes.size();
}


qint32 NoteTable::findNotesByNotebook(QList<qint32> &notes, string guid) {
    return findNotesByNotebook(notes, QString::fromStdString(guid));
}


void NoteTable::updateNoteContent(qint32 lid, QString content, bool isDirty) {
    NSqlQuery query(*db);
    query.prepare("update datastore set data=:content where lid=:lid and key=:key");
    query.bindValue(":content", content);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_CONTENT);
    query.exec();

    query.prepare("update datastore set data=:content where lid=:lid and key=:key");
    query.bindValue(":content", content.length());
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_CONTENT_LENGTH);
    query.exec();

    setDirty(lid, isDirty);

    query.prepare("update datastore set data='true' where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();
}



qint32 NoteTable::findNotesByTitle(QList<qint32> &lids, QString title) {
    NSqlQuery query(*db);
    lids.clear();

    query.prepare("select lid from notetable where title like :title order by datecreated;");
    query.bindValue(":title", title);
    query.exec();

    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}


qint32 NoteTable::getCount() {
    NSqlQuery query(*db);
    query.prepare("Select count(lid) from DataStore where key=:key and lid not in (select lid from datastore where key=:key2 and data = 'true')");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":key2", NOTE_EXPUNGED_FROM_TRASH);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    return 0;

}



qint32 NoteTable::getUnindexedCount() {
    NSqlQuery query(*db);
    query.prepare("Select count(lid) from DataStore where key=:key and data='true' and lid not in (select lid from datastore where key=:key2 and data = 'true')");
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":key2", NOTE_EXPUNGED_FROM_TRASH);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    return 0;

}



qint32 NoteTable::duplicateNote(qint32 oldLid, bool keepCreatedDate) {
    if (oldLid <=0)
        return -1;

    ConfigStore cs(db);
    qint32 newLid = cs.incrementLidCounter();

    NSqlQuery query(*db);
    query.prepare("insert into datastore (lid, key, data) select :newLid, key, data from datastore where lid=:oldLid");
    query.bindValue(":newLid", newLid);
    query.bindValue(":oldLid", oldLid);
    query.exec();

    query.prepare("update datastore set data=:data where lid=:lid and key=:key");
    query.bindValue(":data", QString::number(newLid));
    query.bindValue(":lid", newLid);
    query.bindValue(":key", NOTE_GUID);
    query.exec();

    query.prepare("update datastore set data=:data where lid=:lid and key=:key");
    query.bindValue(":data", 0);
    query.bindValue(":lid", newLid);
    query.bindValue(":key", NOTE_UPDATE_SEQUENCE_NUMBER);
    query.exec();

    Note n;
    get(n, newLid, false, false);
    NotebookTable notebookTable(db);
    qint32 notebookLid = notebookTable.getLid(n.notebookGuid);
    updateNoteList(newLid, n, true, notebookLid);

    setDirty(newLid, true);
    setIndexNeeded(newLid, true);

    if (!keepCreatedDate) {
        qint64 dt = QDateTime::currentMSecsSinceEpoch();
        this->updateDate(newLid, dt, NOTE_CREATED_DATE, true);
    }

    // Update all the resources
    ResourceTable resTable(db);
    QList<qint32> lids;
    resTable.getResourceList(lids, oldLid);
    for (int i=0; i<lids.size(); i++) {
        qint32 newResLid = cs.incrementLidCounter();

        query.prepare("insert into datastore (lid, key,data) select :newLid, key, data from datastore where lid=:oldLid");
        query.bindValue(":newLid", newResLid);
        query.bindValue(":oldLid", lids[i]);
        query.exec();

        query.prepare("update datastore set data=:data where lid=:lid and key=:key");
        query.bindValue(":data", QString::number(newResLid));
        query.bindValue(":lid", newResLid);
        query.bindValue(":key", RESOURCE_GUID);
        query.exec();

        query.prepare("update datastore set data=:data where lid=:lid and key=:key");
        query.bindValue(":data", 0);
        query.bindValue(":lid", newResLid);
        query.bindValue(":key", RESOURCE_UPDATE_SEQUENCE_NUMBER);
        query.exec();

        query.prepare("update datastore set data=:data where lid=:lid and key=:key");
        query.bindValue(":data", newLid);
        query.bindValue(":lid", newResLid);
        query.bindValue(":key", RESOURCE_NOTE_LID);
        query.exec();

        QStringList filter;
        QDir resDir(global.fileManager.getDbaDirPath());
        filter << QString::number(lids[i])+".*";
        QStringList files = resDir.entryList(filter);
        for (int j=0; j<files.size(); j++) {
            QFile file(global.fileManager.getDbaDirPath()+files[j]);
            int pos = files[j].indexOf(".");
            QString type = files[j].mid(pos);
            file.open(QIODevice::ReadOnly);
            file.copy(global.fileManager.getDbaDirPath()+
                      QString::number(newResLid) +type);
            file.close();
        }
    }

    return newLid;
}






// Get all dirty lids
qint32 NoteTable::getAllDirty(QList<qint32> &lids) {
    NSqlQuery query(*db);
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data = 'true'");
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}



// Get all dirty lids
qint32 NoteTable::getAllDirty(QList<qint32> &lids, qint32 linkedNotebookLid) {
    NSqlQuery query(*db);
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data = 'true' and lid in (select lid from datastore where key=:notebookKey and data=:notebookLid)");
    query.bindValue(":key", NOTE_ISDIRTY);
    query.bindValue(":notebookKey", NOTE_NOTEBOOK_LID);
    query.bindValue(":notebookLid", linkedNotebookLid);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}







// Get the notebook lid for a note
qint32 NoteTable::getNotebookLid(qint32 noteLid) {
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_NOTEBOOK_LID);
    query.bindValue(":lid", noteLid);
    query.exec();
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}




// Update the USN
void NoteTable::setUpdateSequenceNumber(qint32 lid, qint32 usn) {
    NSqlQuery query(*db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", usn);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_UPDATE_SEQUENCE_NUMBER);
    query.exec();
}



void NoteTable::updateEnmediaHash(qint32 lid, QByteArray oldHash, QByteArray newHash, bool isDirty) {
    QString content;
    NSqlQuery query(*db);
    query.prepare("Select data from datastore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_CONTENT);
    query.exec();
    if (query.next()) {
        content = query.value(0).toString();

        // Start going through & looking for the old hash
        int pos = content.indexOf("<en-note");
        int endPos;
        int hashPos = -1;
        QString hashString = "hash=\"" +oldHash.toHex() +"\"";
        while (pos>0) {
            endPos = content.indexOf(">", pos);  // Find the matching end of the tag
            hashPos = content.indexOf(hashString, pos);
            if (hashPos < endPos && hashPos > 0) {  // If we found the hash, begin the update
                QString startString = content.mid(0, hashPos);
                QString endString = content.mid(hashPos+hashString.length());
                QString newContent = startString + "hash=\"" +newHash.toHex() +"\"" +endString;
                updateNoteContent(lid, newContent, isDirty);
                pos = -1;
            } else {
                pos = content.indexOf("<", pos+1);
            }
        }
    }
}



void NoteTable::setThumbnail(qint32 lid, QString filename) {
    NSqlQuery query(*db);
    query.prepare("Update notetable set thumbnail=:thumbnail where lid=:lid");
    query.bindValue(":thumbnail", filename);
    query.bindValue(":lid", lid);
    query.exec();
}


void NoteTable::reindexAllNotes() {
    NSqlQuery query(*db);
    query.prepare("delete from datastore where key=:indexKey");
    query.bindValue(":indexKey", NOTE_INDEX_NEEDED);
    query.exec();

    query.prepare("insert into datastore (lid, key, data) select lid, :indexKey, 'true' from datastore where key=:key;");
    query.bindValue(":indexKey", NOTE_INDEX_NEEDED);
    query.bindValue(":key", NOTE_GUID);
    query.exec();
}


void NoteTable::resetGeography(qint32 lid, bool isDirty) {
    NSqlQuery query(*db);
    query.prepare("delete from datastore where lid=:lid and key=:key");

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_LONGITUDE);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_LATITUDE);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_ALTITUDE);
    query.exec();

    if (isDirty)
        this->setDirty(lid, isDirty);
}




void NoteTable::setGeography(qint32 lid, double longitude, double latitude, double altitude, bool isDirty) {
    this->resetGeography(lid, isDirty);

     NSqlQuery query(*db);
    query.prepare("insert into datastore (lid,key,data) values(:lid, :key, :data)");

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_LONGITUDE);
    query.bindValue(":data", longitude);
    query.exec();
    QLOG_DEBUG() << query.lastError();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_LATITUDE);
    query.bindValue(":data", latitude);
    query.exec();

    if (altitude>0.0) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ATTRIBUTE_ALTITUDE);
        query.bindValue(":data", altitude);
        query.exec();
    }

    if (isDirty)
        this->setDirty(lid, isDirty);
}



void NoteTable::setThumbnailNeeded(qint32 lid, bool value) {
    NSqlQuery query(*db);
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
    query.exec();

    if (value) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
        query.bindValue(":data", value);
        query.exec();
    }

}

void NoteTable::setThumbnailNeeded(QString guid, bool value) {
    qint32 lid = getLid(guid);
    setThumbnailNeeded(lid, value);
}

void NoteTable::setThumbnailNeeded(string guid, bool value) {
    qint32 lid = getLid(guid);
    setThumbnailNeeded(lid, value);
}

bool NoteTable::isThumbnailNeeded(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
    query.exec();
    if (query.next()) {
        return query.value(0).toBool();
    }
    return false;
}

bool NoteTable::isThumbnailNeeded(QString guid) {
    qint32 lid = getLid(guid);
    return isThumbnailNeeded(lid);
}

bool NoteTable::isThumbnailNeeded(string guid) {
    qint32 lid = getLid(guid);
    return isThumbnailNeeded(lid);
}

qint32 NoteTable::getNextThumbnailNeeded() {
    NSqlQuery query(*db);
    query.prepare("select lid from datastore where data='true' and key=:key limit 1;");
    query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
    query.exec();
    if (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}



qint32 NoteTable::getThumbnailsNeededCount() {
    NSqlQuery query(*db);
    query.prepare("select count(lid)from datastore where data='true' and key=:key;");
    query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
    query.exec();
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}



void NoteTable::setReminderCompleted(qint32 lid, bool completed) {
    NSqlQuery query(*db);
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
    query.exec();

    if (completed) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, datetime('now'))");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
        query.exec();

//        query.prepare("Update NoteTable set reminderTime=datetime('now') where lid=:lid");
//        query.bindValue(":lid", lid);
//        query.exec();
    }
}



void NoteTable::removeReminder(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
    query.exec();

    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_ORDER);
    query.exec();


    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
    query.exec();

    query.prepare("Update NoteTable set reminderTime=0 where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
}


void NoteTable::getAllReminders(QList< QPair<qint32,qlonglong>* > *reminders) {
    NSqlQuery query(*db);
    query.prepare("select lid,data from datastore where  key=:key1 and lid not in (select lid from datastore where key=:key2)");
    query.bindValue(":key1", NOTE_ATTRIBUTE_REMINDER_TIME);
    query.bindValue(":key2", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
    query.exec();
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        qlonglong dt = query.value(1).toLongLong();
        QPair<qint32, qlonglong> *p = new QPair<qint32, qlonglong>();
        p->first = lid;
        p->second = dt;
        reminders->append(p);
    }
}


// Return if a note is dirty given its lid
bool NoteTable::isPinned(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISPINNED);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}


// Determine if a note is pinned given a guid
bool NoteTable::isPinned(QString guid) {
    qint32 lid = getLid(guid);
    return isPinned(lid);
}


// Determine if a note is pinned a guid
bool NoteTable::isPinned(string guid) {
    QString g(QString::fromStdString(guid));
    return isPinned(g);
}


// Return if a note is dirty given its lid
void NoteTable::pinNote(qint32 lid, bool value) {
    if (value && isPinned(lid))
        return;

    NSqlQuery query(*db);
    query.prepare("Delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISPINNED);
    query.exec();

    if (!value) {
        query.prepare("Update NoteTable set isPinned='false' where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
        QLOG_DEBUG() << query.lastError();
        return;
    }

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, 'true')");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISPINNED);
    query.exec();
    QLOG_DEBUG() << query.lastError();

    query.prepare("Update NoteTable set isPinned='true' where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.lastError();

    setDirty(lid, true);
}


// Determine if a note is pinned given a guid
void NoteTable::pinNote(QString guid, bool value) {
    qint32 lid = getLid(guid);
    pinNote(lid, value);
}


// Determine if a note is pinned a guid
void NoteTable::pinNote(string guid, bool value) {
    QString g(QString::fromStdString(guid));
    pinNote(g, value);
}


// Get all lids
void NoteTable::getAll(QList<qint32> &lids) {
    NSqlQuery query(*db);
    lids.empty();
    query.prepare("Select lid from DataStore where key=:guid");
    query.bindValue(":guid", NOTE_GUID);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());

    }
}
