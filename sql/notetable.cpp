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
NoteTable::NoteTable(DatabaseConnection *db)
{
    this->db = db;
}



// Given a note's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void NoteTable::updateGuid(qint32 lid, Guid &guid) {
    QLOG_TRACE() << "Entering NoteTable::updateNoteGuid()";

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", guid);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.exec();
    db->unlock();

    QLOG_TRACE() << "Leaving NoteTable::updateNoteGuid()";
}




// Synchronize a new note with what is in the database.  We basically
// just delete the old one & give it a new entry
void NoteTable::sync(Note &note, qint32 account) {
    sync(0, note, account);
}



// Synchronize a new note with what is in the database.  We basically
// just delete the old one & give it a new entry
void NoteTable::sync(qint32 lid, const Note &note, qint32 account) {
   // QLOG_TRACE() << "Entering NoteTable::sync()";

    if (lid > 0) {
        NSqlQuery query(db);

        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();

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

    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":data", guid);
    query.exec();
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
}


// Given a note's lid, return the guid
QString NoteTable::getGuid(qint32 lid) {

    NSqlQuery query(db);
    QString retval = "";
    db->lockForRead();
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next())
        retval = query.value(0).toString();
    query.finish();
    db->unlock();
    return retval;
}


// Given a note's GUID, we return the LID
qint32 NoteTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new note to the database
qint32 NoteTable::add(qint32 l, const Note &t, bool isDirty, qint32 account) {
    db->lockForWrite();

    ResourceTable resTable(db);
    ConfigStore cs(db);
    NSqlQuery query(db);
    NSqlQuery trans(db);
    trans.exec("begin");
    qint32 position;
    qint32 lid = l;
    qint32 notebookLid = account;

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    if (lid <= 0)
        lid = cs.incrementLidCounter();

    QLOG_DEBUG() << "Adding note("<<lid<<") " << (t.title.isSet() ? t.title : "title is empty");
    if (t.guid.isSet()) {
        QString guid = t.guid;
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_GUID);
        query.bindValue(":data", guid);
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":data", true);
    query.exec();

    if (t.title.isSet()) {
        query.bindValue(":lid", lid);
        QString title = t.title;
        query.bindValue(":key", NOTE_TITLE);
        query.bindValue(":data", title);
        query.exec();
    }

    if (t.content.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT);
        QByteArray b;
        QString content = t.content;
        b.append(content.toAscii());
        query.bindValue(":data", b);
        query.exec();
    }

    if (t.contentHash.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT_HASH);
        QByteArray contentHash = t.contentHash;
        query.bindValue(":data", contentHash);
        query.exec();
    }

    if (t.contentLength.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CONTENT_LENGTH);
        qint32 len = t.contentLength;
        query.bindValue(":data", len);
        query.exec();
    }

    if (t.updateSequenceNum.isSet()) {
        query.bindValue(":lid", lid);
        qint32 usn = t.updateSequenceNum;
        query.bindValue(":key", NOTE_UPDATE_SEQUENCE_NUMBER);
        query.bindValue(":data", usn);
        query.exec();
    }

    if (isDirty) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ISDIRTY);
        query.bindValue(":data", isDirty);
        query.exec();
    }

    if (t.created.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_CREATED_DATE);
        qlonglong date = t.created;
        query.bindValue(":data",date);
        query.exec();
    }

    if (t.updated.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_UPDATED_DATE);
        qlonglong date = t.updated;
        query.bindValue(":data", date);
        query.exec();
    }

    if (t.deleted.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_DELETED_DATE);
        qlonglong date = t.deleted;
        query.bindValue(":data", date);
        query.exec();
    }

    if (t.active.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ACTIVE);
        bool active = t.active;
        query.bindValue(":data", active);
        query.exec();
    }

    if (t.notebookGuid.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_NOTEBOOK_LID);
        NotebookTable notebookTable(db);
        LinkedNotebookTable linkedTable(db);
        if (account > 0)
            notebookLid = account;
        else
            notebookLid = 0;
        if (notebookLid <= 0)
            notebookLid = notebookTable.getLid(t.notebookGuid);
        if (notebookLid <= 0) {
            notebookLid = linkedTable.getLid(t.notebookGuid);
        }

        // If not found, we insert one to avoid problems.  We'll probably get the real data later
        if (notebookLid <= 0) {
            notebookLid = cs.incrementLidCounter();
            Notebook notebook;
            notebook.guid = t.notebookGuid;
            notebook.name = "<Missing Notebook>";
            notebookTable.add(notebookLid, notebook, false, false);
        }
        query.bindValue(":data", notebookLid);
        query.exec();
    }

    QList<QString> tagGuids;
    if (t.tagGuids.isSet())
        tagGuids = t.tagGuids;
    for (int i=0; i<tagGuids.size(); i++) {
        TagTable tagTable(db);
        qint32 tagLid = tagTable.getLid(tagGuids[i]);
        if (tagLid == 0) {
            // create a dummy tag to avoid later problems
            Tag newTag;
            newTag.guid = tagGuids[i];
            newTag.name = "";
            tagLid = cs.incrementLidCounter();
            tagTable.add(tagLid, newTag, false, 0);
        }

        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_TAG_LID);
        query.bindValue(":data", tagLid);
        query.exec();
    }

    QList<Resource> resources;
    if (t.resources.isSet())
        resources = t.resources;
    for (int i=0; i<resources.size(); i++) {
        qint32 resLid;
        resLid = 0;
        Resource r;
        r = resources[i];
        resLid = resTable.getLid(t.guid,resources[i].guid);
        if (resLid == 0)
            resLid = cs.incrementLidCounter();
        resTable.add(resLid, r, isDirty, lid);

        if (r.mime.isSet()) {
            QString mime = r.mime;
            if (!mime.startsWith("image/") && mime != "vnd.evernote.ink") {
                query.bindValue(":lid", lid);
                query.bindValue(":key", NOTE_HAS_ATTACHMENT);
                query.bindValue(":data", true);
                query.exec();
            }
        }
    }

    if (t.attributes.isSet()) {
        NoteAttributes na = t.attributes;
        if (na.subjectDate.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SUBJECT_DATE);
            qlonglong ts = na.subjectDate;
            query.bindValue(":data",ts);
            query.exec();
        }
        if (na.latitude.isSet()) {
            double lat = na.latitude;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_LATITUDE);
            query.bindValue(":data", lat);
            query.exec();
        }
        if (na.longitude.isSet()) {
            double lon = na.longitude;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_LONGITUDE);
            query.bindValue(":data", lon);
            query.exec();
        }
        if (na.altitude.isSet()) {
            double alt = na.altitude;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_ALTITUDE);
            query.bindValue(":data", alt);
            query.exec();
        }
        if (na.author.isSet()) {
            QString author = na.author;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_AUTHOR);
            query.bindValue(":data", author);
            query.exec();
        }
        if (na.source.isSet()) {
            QString source = na.source;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE);
            query.bindValue(":data", source);
            query.exec();
        }
        if (na.sourceURL.isSet()) {
            QString sourceURL = na.sourceURL;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_URL);
            query.bindValue(":data", sourceURL);
            query.exec();
        }
        if (na.sourceApplication.isSet()) {
            QString sourceApplication = na.sourceApplication;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SOURCE_APPLICATION);
            query.bindValue(":data", sourceApplication);
            query.exec();
        }
        if (na.shareDate.isSet()) {
            double date = na.shareDate;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_SHARE_DATE);
            query.bindValue(":data",date);
            query.exec();
        }
        if (na.placeName.isSet()) {
            QString placename = na.placeName;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_PLACE_NAME);
            query.bindValue(":data", placename);
            query.exec();
        }
        if (na.contentClass.isSet()) {
            QString cc = na.contentClass;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_CONTENT_CLASS);
            query.bindValue(":data", cc);
            query.exec();
        }
        if (na.reminderTime.isSet()) {
            double rt = na.reminderTime;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_TIME);
            query.bindValue(":data",rt);
            query.exec();
        }
        if (na.reminderDoneTime.isSet()) {
            double rt = na.reminderDoneTime;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
            query.bindValue(":data", rt);
            query.exec();
        }
        if (na.reminderOrder.isSet()) {
            bool rt = na.reminderOrder;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_ORDER);
            query.bindValue(":data", rt);
            query.exec();
        }
        db->unlock();
    }

    // No determine some attributes of the note based upon the content
    // This should probably happen every time a note changes? Or at least something simular:
    QString content;
    if (t.content.isSet())
        content = t.content;
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
    query.finish();
    trans.exec("commit");
    trans.finish();

    updateNoteList(lid, t, isDirty, account);
    db->unlock();
    return lid;
}



// Add a stub for a note.  More information about the note will be added later.  This can
// happen during a sync if a resource appears before the note itself
qint32 NoteTable::addStub(QString noteGuid) {
    db->lockForWrite();
    ConfigStore cs(db);
    NSqlQuery query(db);

    qint32 lid = 0;
    lid = getLid(noteGuid);

    if (lid <= 0)
        lid = cs.incrementLidCounter();

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":data", noteGuid);
    query.exec();
    query.finish();
    db->unlock();
    return lid;
}



// Update the note listing table
bool NoteTable::updateNoteList(qint32 lid, const Note &t, bool isDirty, qint32 notebook) {

    if (lid <= 0)
        return false;

    db->lockForWrite();
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
        notebookLid = notebookTable.addStub(t.notebookGuid);
    else {
        Notebook notebook;
        notebookTable.get(notebook, notebookLid);
        notebookName = notebook.name;
    }
    // Now let's update the user table
    NSqlQuery query(db);

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

    QString title = "";
    if (t.title.isSet())
        title = t.title;
    query.bindValue(":title", title);

    if (t.attributes.isSet()) {
        NoteAttributes na = t.attributes;
        if (na.author.isSet()) {
            QString author = na.author;
            query.bindValue(":author",author);
        } else {
            query.bindValue(":author", "");
        }
        if (na.subjectDate.isSet()) {
            double sd = na.subjectDate;
            query.bindValue(":dateSubject", sd);
        } else {
            query.bindValue(":dateSubject", 0);
        }
        if (na.source.isSet()) {
            QString source = na.source;
            query.bindValue(":source", source);
        } else {
            query.bindValue(":source", "");
        }
        if (na.sourceURL.isSet()) {
            QString url = na.sourceURL;
            query.bindValue(":sourceUrl", url);
        } else {
            query.bindValue(":sourceUrl", "");
        }
        if (na.sourceApplication.isSet()) {
            QString sa = na.sourceApplication;
            query.bindValue(":sourceApplication", sa);
        } else {
            query.bindValue(":sourceApplication", "");
        }
        if (na.latitude.isSet()) {
            double lat = na.latitude;
            query.bindValue(":latitude", lat);
        } else {
            query.bindValue(":latitude", 0);
        }
        if (na.longitude.isSet()) {
            double lon = na.longitude;
            query.bindValue(":longitude",lon);
        } else {
            query.bindValue(":longitude", 0);
        }
        if (na.altitude.isSet()) {
            double alt = na.altitude;
            query.bindValue(":altitude", alt);
        } else {
            query.bindValue(":altitude", 0);
        }
        if (na.reminderOrder.isSet()) {
            qint64 order = na.reminderOrder;
            query.bindValue(":reminderOrder", order);
        } else {
            query.bindValue(":reminderOrder", 0);
        }
        if (na.reminderTime.isSet()) {
            qlonglong rt = na.reminderTime;
            query.bindValue(":reminderTime", rt);
        } else {
            query.bindValue(":reminderTime", 0);
        }
        if (na.reminderDoneTime.isSet()) {
            qlonglong rt = na.reminderDoneTime;
            query.bindValue(":reminderDoneTime", rt);
        } else {
            query.bindValue(":reminderDoneTime", 0);
        }

    }

    qlonglong created = 0;
    qlonglong updated = 0;
    qlonglong deleted = 0;
    if (t.created.isSet())
        created = t.created;
    query.bindValue(":dateCreated", created);

    if (t.updated.isSet())
        updated = t.updated;
    query.bindValue(":dateUpdated", updated);

    if (t.deleted.isSet())
        deleted = t.deleted;
    query.bindValue(":dateDeleted", deleted);

    bool hasEncryption;
    QString content;
    if (t.content.isSet())
        content = t.content;
    if (content.indexOf("<en-crypt") > 0)
        hasEncryption = true;
    else
        hasEncryption = false;
    query.bindValue(":hasEncryption", hasEncryption);
    bool hasTodo;
    if (content.indexOf("<en-todo") > 0)
        hasTodo = true;
    else
        hasTodo = false;
    query.bindValue(":hasTodo", hasTodo);
    query.bindValue(":isDirty", isDirty);
    qlonglong size = content.length();

    QList<Resource> reslist;
    if (t.resources.isSet())
        reslist = t.resources;
    for (int i=0; i<reslist.size(); i++) {
        if (reslist[i].data.isSet()) {
        Data d = reslist[i].data;
        if (d.size.isSet())
            size+=d.size;
        }
    }
    query.bindValue(":size", size);
    query.bindValue(":notebook", notebookName);
    query.bindValue(":notebookLid", notebookLid);

    QString tagName;
    QList<QString> tagNames;

    // Normal sort is below.  If user has problems with non-ASCII characters
    // they can choose to bypass the name sorting
    if (!global.nonAsciiSortBug) {
        QStringList sortedNames;
        if (t.tagNames.isSet())
            tagNames = t.tagNames;
        for (int i=0; i<tagNames.size(); i++) {
            sortedNames.append(tagNames[i].toLower());
        }
        sortedNames.sort();

        TagTable tagTable(db);

        // We search the table to get the name in the correct case.
        // We lowercased them above to sort properly without regards
        // to case.  Now, for the note list we need the correct case
        for (int i=0; i<sortedNames.size(); i++) {
            Tag currentTag;
            qint32 tagLid = tagTable.findByName(sortedNames[i], account);
            tagTable.get(currentTag, tagLid);
            if (currentTag.name.isSet()) {
                if (i>0) {
                    tagName = tagName+", ";
                }
                tagName = tagName + currentTag.name;
            }
        }
    } else {
        // Users have experienced bugs with the above because of non-ASCII characters.
        // This enables them to bypass the bug at the cost of not sorting tags.
        if (t.tagNames.isSet())
            tagNames = t.tagNames;
        for (int i=0; i<tagNames.size(); i++) {
            if (i>0)
                tagName = tagName+", ";
            tagName = tagName + tagNames[i];
        }
    }

    query.bindValue(":tags", tagName);

    if (!query.exec()) {
        QLOG_ERROR() << "Error inserting into NoteTable: " << query.lastError();
        query.finish();
        db->unlock();
        return false;
    }
    query.finish();
    db->unlock();
    return true;
}



// Update the name of a notebook in the note list table
bool NoteTable::updateNotebookName(qint32 lid, QString name) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update NoteTable set notebook=:name where notebooklid=:lid");
    query.bindValue(":name", name);
    query.bindValue(":lid", lid);
    bool retval = query.exec();
    query.finish();
    db->unlock();
    return retval;
}


// Return a note structure given the LID
bool NoteTable::get(Note &note, qint32 lid,bool loadResources, bool loadBinary) {

    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    NoteAttributes na;
    QList<QString> tagGuids;
    QList<QString> tagNames;
    if (note.attributes.isSet()) {
        na = note.attributes;
    }
    query.exec();
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (NOTE_GUID):
            note.guid = query.value(1).toString();
            break;
        case (NOTE_UPDATE_SEQUENCE_NUMBER):
            note.updateSequenceNum = query.value(1).toInt();
            break;
        case (NOTE_ACTIVE):
            note.active = query.value(1).toBool();
            break;
        case (NOTE_DELETED_DATE):
            note.active = query.value(1).toLongLong();
            break;
        case (NOTE_ATTRIBUTE_SOURCE_URL):
            na.sourceURL = query.value(1).toString();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_SOURCE_APPLICATION):
            na.sourceApplication = query.value(1).toString();
            note.attributes = na;
            break;
        case (NOTE_CONTENT_LENGTH):
            note.contentLength = query.value(1).toLongLong();
            break;
        case (NOTE_ATTRIBUTE_LONGITUDE):
            na.longitude = query.value(1).toFloat();
            note.attributes = na;
            break;
        case (NOTE_TITLE):
            note.title = query.value(1).toString();
            break;
        case (NOTE_ATTRIBUTE_SOURCE):
            na.source = query.value(1).toString();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_ALTITUDE):
            na.altitude = query.value(1).toFloat();
            note.attributes = na;
            break;
        case (NOTE_NOTEBOOK_LID): {
            qint32 notebookLid = query.value(1).toInt();
            NotebookTable ntable(db);
            QString notebookGuid;
            ntable.getGuid(notebookGuid, notebookLid);
            note.notebookGuid = notebookGuid;
            break;
        }
        case (NOTE_UPDATED_DATE):
            note.updated = query.value(1).toLongLong();
            break;
        case (NOTE_CREATED_DATE):
            note.created = query.value(1).toLongLong();
            break;
        case (NOTE_ATTRIBUTE_SUBJECT_DATE):
            na.subjectDate = query.value(1).toLongLong();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_LATITUDE):
            na.latitude = query.value(1).toFloat();
            note.attributes = na;
            break;
        case (NOTE_CONTENT):
            note.content = query.value(1).toByteArray().data();
            break;
        case (NOTE_CONTENT_HASH):
            note.contentHash = query.value(1).toByteArray();
            break;
        case (NOTE_ATTRIBUTE_AUTHOR):
            na.author = query.value(1).toString();
            note.attributes = na;
            break;
        case (NOTE_ISDIRTY):
            break;
        case (NOTE_ATTRIBUTE_SHARE_DATE) :
            na.shareDate = query.value(1).toLongLong();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_PLACE_NAME) :
            na.placeName = query.value(1).toString();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_CONTENT_CLASS) :
            na.contentClass = query.value(1).toString();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_REMINDER_ORDER) :
            na.reminderOrder = query.value(1).toLongLong();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_REMINDER_DONE_TIME) :
            na.reminderDoneTime = query.value(1).toLongLong();
            note.attributes = na;
            break;
        case (NOTE_ATTRIBUTE_REMINDER_TIME) :
            na.reminderTime = query.value(1).toLongLong();
            note.attributes = na;
            break;
        case (NOTE_TAG_LID) :
            TagTable tagTable(db);
            qint32 tagLid = query.value(1).toInt();
            Tag tag;
            tagTable.get(tag, tagLid);
            if (tag.guid.isSet())
                tagGuids.append(tag.guid);
            if (tag.name.isSet())
                tagNames.append(tag.name);
            break;
        }
    }
    query.finish();
    if (tagGuids.size() > 0) {
        note.tagGuids = tagGuids;
        note.tagNames = tagNames;
    }

    ResourceTable resTable(db);
    QLOG_TRACE() << "Fetching Resources? " << loadResources << " With binary? " << loadBinary;

    QList<Resource> resources;
    resTable.getAllResources(resources, lid, loadResources, loadBinary);
    note.resources = resources;
        QLOG_TRACE() << "Fetched resources";

    db->unlock();
    if (note.guid.isSet())
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
bool NoteTable::isIndexNeeded(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    db->unlock();
    return retval;
}



// Return if a note is dirty given its lid
bool NoteTable::isDirty(qint32 lid) {
    db->lockForRead();
    NSqlQuery query(db);
    bool retval = false;
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    db->unlock();
    return retval;
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
    NSqlQuery query(db);
    bool retval = false;
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_GUID);
    query.exec();
    if (query.next())
        retval =  true;
    query.finish();
    db->unlock();
    return retval;
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
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:tagLid");
    query.bindValue(":key", NOTE_TAG_LID);
    query.bindValue(":tagLid", tagLid);
    query.exec();
    while (query.next()) {
        values.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return values.size();
}



// Find the note LIDs for a tag
qint32 NoteTable::findNotesByTag(QList<qint32> &values, QString data) {
    TagTable tagTable(db);
    qint32 tagLid = tagTable.getLid(data);
    return findNotesByTag(values, tagLid);
}



// Find the note LIDs for a tag
qint32 NoteTable::findNotesByTag(QList<qint32> &values, string data) {
    return findNotesByTag(values, QString::fromStdString(data));
}


// Update the user note list with the proper tag names
void NoteTable::updateNoteListTags(qint32 noteLid, QString tags) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update NoteTable set tags=:tags where lid=:lid");
    query.bindValue(":lid", noteLid);
    query.bindValue(":tags", tags);
    query.exec();
    query.finish();
    db->unlock();
}



// Update the user's notebook name list
void NoteTable::updateNoteListNotebooks(QString guid, QString name) {
    db->lockForWrite();
    NotebookTable notebookTable(db);
    qint32 notebookLid;
    notebookLid = notebookTable.getLid(guid);
    NSqlQuery sql2(db);
    sql2.prepare("Update NoteTable set notebook=:note where lid=:lid");
    sql2.bindValue(":note", name);
    sql2.bindValue(":lid", notebookLid);
    sql2.exec();
    sql2.finish();
    db->unlock();
}



// Get a list of notes for a given tag
qint32 NoteTable::getNotesWithTag(QList<qint32> &retval, QString tag) {
    db->lockForRead();
    NSqlQuery query(db);
    TagTable tagTable(db);
    qint32 tagLid = tagTable.getLid(tag);
    query.prepare("Select lid data from DataStore where data=:tag and key=:key");
    query.bindValue(":tag", tagLid);
    query.bindValue(":key", NOTE_TAG_LID);
    query.exec();
    while(query.next()) {
        retval.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return retval.size();
}


// Set if a note needs to be indexed
void NoteTable::setIndexNeeded(qint32 lid, bool indexNeeded) {
    if (lid <= 0)
        return;

    // If it is already set to this value, then we don't need to
    // do anything.
    if (this->isIndexNeeded(lid) == indexNeeded)
        return;

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.exec();

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":data", indexNeeded);
    query.exec();
    query.finish();
    db->unlock();
}



// Set if a note needs to be indexed
qint32 NoteTable::getIndexNeeded(QList<qint32> &lids) {
    NSqlQuery query(db);
    lids.clear();
    qlonglong delayTime = QDateTime::currentDateTime().currentMSecsSinceEpoch()-300000;
    db->lockForRead();
    query.prepare("Select lid, data from DataStore where key=:key and lid in (select lid from datastore where key=:key2 and data='true')");
    query.bindValue(":key", NOTE_UPDATED_DATE);
    query.bindValue(":key2", NOTE_INDEX_NEEDED);
    query.exec();
    while (query.next()) {
        qlonglong dt = query.value(1).toLongLong();
        if (delayTime > dt)
            lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return lids.size();
}



// Update the notebook for a note
void NoteTable::updateNotebook(qint32 noteLid, qint32 notebookLid, bool setAsDirty) {
    Notebook book;
    NotebookTable notebookTable(db);
    notebookTable.get(book, notebookLid);

    if (book.guid.isSet()) {
        NSqlQuery query(db);
        db->lockForWrite();
        query.prepare("Update DataStore set data=:notebookLid where lid=:lid and key=:key;");
        query.bindValue(":notebookLid", notebookLid);
        query.bindValue(":lid", noteLid);
        query.bindValue(":key", NOTE_NOTEBOOK_LID);
        query.exec();

        if (setAsDirty) {
            setDirty(noteLid, setAsDirty);
        }

        QString bookName = book.name;
        query.prepare("Update NoteTable set notebook=:name where lid=:lid");
        query.bindValue(":name", bookName);
        query.bindValue(":lid", noteLid);
        query.exec();

        query.prepare("Update NoteTable set notebookLid=:nlid where lid=:lid");
        query.bindValue(":nlid", notebookLid);
        query.bindValue(":lid", noteLid);
        query.exec();
        query.finish();
        db->unlock();
    }
}




// Update the URL for a note
void NoteTable::updateUrl(qint32 noteLid, QString url, bool setAsDirty=false) {
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}




// Update the title for a note
void NoteTable::updateTitle(qint32 noteLid, QString title, bool setAsDirty=false) {
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}




// Update the author for a note
void NoteTable::updateAuthor(qint32 noteLid, QString author, bool setAsDirty=false) {
    if (noteLid <=0)
        return;
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}



// Update the subject/update/deleted/created date for a note
void NoteTable::updateDate(qint32 lid, Timestamp ts, qint32 key, bool isDirty = false) {
    if (lid <= 0)
        return;

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update DataStore set data=:ts where lid=:lid and key=:key;");
    query.bindValue(":ts",ts);
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
    query.finish();
    db->unlock();
}



void NoteTable::removeTag(qint32 lid, qint32 tag, bool isDirty = false) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from DataStore where lid=:lid and key=:key and data=:tag");
    query.bindValue(":lid", lid);
    query.bindValue(":key",NOTE_TAG_LID);
    query.bindValue(":tag:", tag);
    query.exec();
    query.finish();
    db->unlock();
    if (isDirty) {
        setDirty(lid, isDirty);
    }
    rebuildNoteListTags(lid);
}


void NoteTable::addTag(qint32 lid, qint32 tag, bool isDirty = false) {
    if (lid <= 0)
        return;

    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->lockForWrite();

    if (isDirty) {
        setDirty(lid, isDirty);
    }
    rebuildNoteListTags(lid);
}


bool NoteTable::hasTag(qint32 noteLid, qint32 tagLid) {
    NSqlQuery query(db);
    db->lockForRead();
    bool retval = false;
    query.prepare("select lid from DataStore where lid=:lid and key=:key and data=:tag");
    query.bindValue(":lid", noteLid);
    query.bindValue(":key",NOTE_TAG_LID);
    query.bindValue(":tag:", tagLid);
    query.exec();
    if (query.next())
        retval =  true;
    query.finish();
    db->unlock();
    return retval;
}


void NoteTable::rebuildNoteListTags(qint32 lid) {
    // update the note list
    QStringList tagNames;
    TagTable tagTable(db);
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_TAG_LID);
    query.exec();
    while (query.next()) {
        qint32 tagLid = query.value(0).toInt();
        Tag t;
        tagTable.get(t, tagLid);
        if (t.name.isSet())
            tagNames.append(t.name);
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
    query.finish();
    db->unlock();
}



QString NoteTable::getNoteListTags(qint32 lid) {
    db->lockForRead();
    QString retval = "";
    NSqlQuery query(db);
    query.prepare("select tags from NoteTable where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next()) {
        retval = query.value(0).toString();
    }
    query.finish();
    db->unlock();
    return retval;
}


void NoteTable::setDirty(qint32 lid, bool dirty) {
    if (lid <=0)
        return;
    qint64 dt = QDateTime::currentMSecsSinceEpoch();

    db->lockForWrite();
    NSqlQuery query(db);

    // If it is setting it as dirty, we need to update the
    // update date &  time.
    if (dirty) {
        query.prepare("Delete from DataStore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_UPDATED_DATE);
        query.exec();

        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :value)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_UPDATED_DATE);
        query.bindValue(":value", dt);
        query.exec();

        query.prepare("Update NoteTable set dateUpdated=:value where lid=:lid");
        query.bindValue(":lid", lid);
        query.bindValue(":value", dt);
        query.exec();
    }

    // If it is already set to the value, then we don't
    // need to do anything more.
    if (isDirty(lid) == dirty)
        return;

    // If we got here, then the current dirty state doesn't match
    // what the caller wants.
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
    }
    query.finish();
    db->unlock();
}



bool NoteTable::isDeleted(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_ACTIVE);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next()) {
        db->unlock();
        bool active = query.value(0).toBool();
        query.finish();
        return !active;
    }
    query.finish();
    db->unlock();
    return false;
}



void NoteTable::deleteNote(qint32 lid, bool isDirty=true) {
    if (lid <=0)
        return;

    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}



void NoteTable::restoreNote(qint32 lid, bool isDirty=true) {
    if (lid <=0)
        return;

    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}




qint32 NoteTable::getAllDeleted(QList<qint32> &lids) {
    db->lockForRead();
    lids.clear();
    NSqlQuery query(db);
    query.prepare("select lid from DataStore where key=:key and data='false'");
    query.bindValue(":key", NOTE_ACTIVE);
    query.exec();

    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return lids.size();
}



void NoteTable::expunge(qint32 lid) {
    Note note;
    this->get(note, lid, true, false);
    ResourceTable resTable(db);
    QList<Resource> resources;
    if (note.resources.isSet())
        resources = note.resources;
    for (int i=0; i<resources.size(); i++) {
        resTable.expunge(resources[i].guid);
    }

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.prepare("delete from NoteTable where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
    db->unlock();
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
    NSqlQuery query(db);
    qint32 notebookLid;
    NotebookTable notebookTable(db);
    notebookLid = notebookTable.getLid(guid);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:notebookLid");
    query.bindValue(":key", NOTE_NOTEBOOK_LID);
    query.bindValue(":notebookLid", notebookLid);
    query.exec();
    while (query.next()) {
        notes.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return notes.size();
}


qint32 NoteTable::findNotesByNotebook(QList<qint32> &notes, string guid) {
    return findNotesByNotebook(notes, QString::fromStdString(guid));
}


void NoteTable::updateNoteContent(qint32 lid, QString content, bool isDirty) {
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}



qint32 NoteTable::findNotesByTitle(QList<qint32> &lids, QString title) {
    NSqlQuery query(db);
    lids.clear();

    db->lockForRead();
    query.prepare("select lid from notetable where title like :title order by datecreated;");
    query.bindValue(":title", title);
    query.exec();

    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return lids.size();
}


qint32 NoteTable::getCount() {
    qint32 retval = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select count(lid) from DataStore where key=:key and lid not in (select lid from datastore where key=:key2 and data = 'true')");
    query.bindValue(":key", NOTE_GUID);
    query.bindValue(":key2", NOTE_EXPUNGED_FROM_TRASH);
    query.exec();
    if (query.next())
        retval =  query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;

}



qint32 NoteTable::getUnindexedCount() {
    qint32 retval = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select count(lid) from DataStore where key=:key and data='true' and lid not in (select lid from datastore where key=:key2 and data = 'true')");
    query.bindValue(":key", NOTE_INDEX_NEEDED);
    query.bindValue(":key2", NOTE_EXPUNGED_FROM_TRASH);
    query.exec();
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
}



qint32 NoteTable::duplicateNote(qint32 oldLid, bool keepCreatedDate) {
    if (oldLid <=0)
        return -1;

    ConfigStore cs(db);
    qint32 newLid = cs.incrementLidCounter();
    db->lockForWrite();

    NSqlQuery query(db);
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
    query.finish();
    db->unlock();
    return newLid;
}






// Get all dirty lids
qint32 NoteTable::getAllDirty(QList<qint32> &lids) {
    NSqlQuery query(db);
    db->lockForRead();
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data = 'true'");
    query.bindValue(":key", NOTE_ISDIRTY);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return lids.size();
}



// Get all dirty lids
qint32 NoteTable::getAllDirty(QList<qint32> &lids, qint32 linkedNotebookLid) {
    NSqlQuery query(db);
    lids.clear();
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data = 'true' and lid in (select lid from datastore where key=:notebookKey and data=:notebookLid)");
    query.bindValue(":key", NOTE_ISDIRTY);
    query.bindValue(":notebookKey", NOTE_NOTEBOOK_LID);
    query.bindValue(":notebookLid", linkedNotebookLid);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return lids.size();
}







// Get the notebook lid for a note
qint32 NoteTable::getNotebookLid(qint32 noteLid) {
    qint32 retval = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_NOTEBOOK_LID);
    query.bindValue(":lid", noteLid);
    query.exec();
    if (query.next()) {
        retval = query.value(0).toInt();
    }
    query.finish();
    db->unlock();
    return retval;
}




// Update the USN
void NoteTable::setUpdateSequenceNumber(qint32 lid, qint32 usn) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", usn);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_UPDATE_SEQUENCE_NUMBER);
    query.exec();
    query.finish();
    db->unlock();
}



void NoteTable::updateEnmediaHash(qint32 lid, QByteArray oldHash, QByteArray newHash, bool isDirty) {
    QString content;
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}



void NoteTable::setThumbnail(qint32 lid, QString filename) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update notetable set thumbnail=:thumbnail where lid=:lid");
    query.bindValue(":thumbnail", filename);
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
    db->unlock();
}


void NoteTable::reindexAllNotes() {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from datastore where key=:indexKey");
    query.bindValue(":indexKey", NOTE_INDEX_NEEDED);
    query.exec();

    query.prepare("insert into datastore (lid, key, data) select lid, :indexKey, 'true' from datastore where key=:key;");
    query.bindValue(":indexKey", NOTE_INDEX_NEEDED);
    query.bindValue(":key", NOTE_GUID);
    query.exec();
    query.finish();
    db->unlock();
}


void NoteTable::resetGeography(qint32 lid, bool isDirty) {
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();

    if (isDirty)
        this->setDirty(lid, isDirty);
}




void NoteTable::setGeography(qint32 lid, double longitude, double latitude, double altitude, bool isDirty) {
    this->resetGeography(lid, isDirty);

    db->lockForWrite();
    NSqlQuery query(db);
    query.prepare("insert into datastore (lid,key,data) values(:lid, :key, :data)");

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_LONGITUDE);
    query.bindValue(":data", longitude);
    query.exec();

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
    query.finish();
    db->unlock();

    if (isDirty)
        this->setDirty(lid, isDirty);
}



void NoteTable::setThumbnailNeeded(qint32 lid, bool value) {
    if (lid >=0)
        return;

    // If it is already set to this value, then we don't need to
    // do anything.
    if (isThumbnailNeeded(lid) == value)
        return;

    db->lockForWrite();
    NSqlQuery query(db);
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
    query.finish();
    db->unlock();
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
    bool retval = false;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
    query.exec();
    if (query.next()) {
        retval = query.value(0).toBool();
    }
    query.finish();
    db->unlock();
    return retval;
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
    qint32 retval = -1;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select lid from datastore where data='true' and key=:key limit 1;");
    query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
    query.exec();
    if (query.next()) {
        retval = query.value(0).toInt();
    }
    query.finish();
    db->unlock();
    return retval;
}



qint32 NoteTable::getThumbnailsNeededCount() {
    qint32 retval = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select count(lid)from datastore where data='true' and key=:key;");
    query.bindValue(":key", NOTE_THUMBNAIL_NEEDED);
    query.exec();
    if (query.next()) {
        retval = query.value(0).toInt();
    }
    query.finish();
    db->unlock();
    return retval;
}



void NoteTable::setReminderCompleted(qint32 lid, bool completed) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
    query.exec();

    if (completed) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, datetime('now'))");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTE_ATTRIBUTE_REMINDER_DONE_TIME);
        query.exec();
    }
    query.finish();
    db->unlock();
}



void NoteTable::removeReminder(qint32 lid) {
    NSqlQuery query(db);
    db->lockForWrite();
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
    query.finish();
    db->unlock();
}


void NoteTable::getAllReminders(QList< QPair<qint32,qlonglong>* > *reminders) {
    NSqlQuery query(db);
    db->lockForRead();
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
    query.finish();
    db->unlock();
}


// Return if a note is dirty given its lid
bool NoteTable::isPinned(qint32 lid) {
    bool retval = false;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISPINNED);
    query.exec();
    if (query.next())
        retval =  query.value(0).toBool();
    query.finish();
    db->unlock();
    return retval;
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

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISPINNED);
    query.exec();

    if (!value) {
        query.prepare("Update NoteTable set isPinned='false' where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
        QLOG_DEBUG() << query.lastError();
        query.finish();
        return;
    }

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, 'true')");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_ISPINNED);
    query.exec();

    query.prepare("Update NoteTable set isPinned='true' where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.lastError();
    query.finish();
    db->unlock();

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


void NoteTable::getAllPinned(QList< QPair< qint32, QString > > &lids) {
    NSqlQuery query(db);
    lids.clear();
    db->lockForRead();
    query.prepare("Select lid, data from DataStore where key=:titlekey and lid in (select lid from datastore where key=:key and data='true') order by data");
    query.bindValue(":titlekey", NOTE_TITLE);
    query.bindValue(":key", NOTE_ISPINNED);
    query.exec();
    while (query.next()) {
        QPair<qint32, QString> pair;
        pair.first = query.value(0).toInt();
        pair.second = query.value(1).toString();
        lids.append(pair);
    }
    query.finish();
    db->unlock();
    return;
}


void NoteTable::getRecentlyUpdated(QList< QPair< qint32, QString > > &lids) {
    NSqlQuery query(db);
    lids.clear();
    db->lockForRead();
    query.prepare("select lid, title from notetable order by dateupdated desc limit 10");
    query.exec();
    while (query.next()) {
        QPair<qint32, QString> pair;
        pair.first = query.value(0).toInt();
        pair.second = query.value(1).toString();
        lids.append(pair);
    }
    query.finish();
    db->unlock();
    return;
}





// Get all lids
void NoteTable::getAll(QList<qint32> &lids) {
    NSqlQuery query(db);
    lids.empty();
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:guid");
    query.bindValue(":guid", NOTE_GUID);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());

    }
    query.finish();
    db->unlock();
}



// Get all lids
void NoteTable::setTitleColor(qint32 lid, QString color) {
    NSqlQuery query(db);
    QString c = color;
    if (c == "white")
        c = "";
    db->lockForWrite();
    query.prepare("Update NoteTable set titleColor=:color where lid=:lid");
    query.bindValue(":color", c);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("Delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTE_TITLE_COLOR);
    query.bindValue(":lid", lid);
    query.exec();

    if (c == "")
        return;
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :value)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTE_TITLE_COLOR);
    query.bindValue(":value", c);
    query.exec();
    query.finish();
    db->unlock();
}
