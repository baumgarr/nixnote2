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


#include "linkednotebooktable.h"
#include "sql/notetable.h"
#include "sql/configstore.h"
#include "sql/notebooktable.h"
#include "sql/sharednotebooktable.h"
#include "sql/nsqlquery.h"
#include "global.h"


// Generic constructor
LinkedNotebookTable::LinkedNotebookTable(DatabaseConnection *db)
{
    this->db = db;
}



// Given a notebook's name as a std::string, we return the lid
qint32 LinkedNotebookTable::findByName(string &name) {
    NotebookTable ntable(db);
    return ntable.findByName(name);
}




// Given a notebook's name as a QString, we return the lid
qint32 LinkedNotebookTable::findByName(QString &name) {
    string n = name.toStdString();
    return findByName(n);
}






// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 LinkedNotebookTable::sync(LinkedNotebook &notebook) {
    return sync(0, notebook);
}



// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 LinkedNotebookTable::sync(qint32 lid, LinkedNotebook &notebook) {
    qint32 lastUSN = 0;
    NotebookTable ntable(db);
    SharedNotebookTable stable(db);

    if (lid == 0 && notebook.shareKey.isSet()) {
        lid = stable.findByShareKey(notebook.shareKey);
    }
    if (lid == 0 && notebook.uri.isSet()) {
        lid = ntable.findByUri(notebook.uri);
    }


    if (lid > 0) {
        lastUSN = getLastUpdateSequenceNumber(lid);
        // Delete the old record
        NSqlQuery query(db);
        db->lockForWrite();
        query.prepare("Delete from DataStore where lid=:lid and key>=3200 and key<3300");
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
        db->unlock();
    }

    if (lid == 0) {
        lid = ntable.getLid(notebook.guid);
        if (lid == 0) {
            ConfigStore cs(db);
            lid = cs.incrementLidCounter();
            NotebookTable ntable(db);

            // Build the dummy notebook entry
            Notebook book;
            book.guid = notebook.guid;
            book.name = notebook.shareName;
            book.updateSequenceNum = notebook.updateSequenceNum;
            book.published = true;
            Publishing publishing;
            publishing.uri = notebook.uri;
            book.publishing = publishing;

            if (notebook.stack.isSet()) {
                book.stack = notebook.stack;
            }

            ntable.sync(lid, book);
        }
    }

    add(lid, notebook, false);
    if (lastUSN > 0) {
        setLastUpdateSequenceNumber(lid, lastUSN);
    }

    return lid;
}




// Given a notebook's GUID, we return the LID
qint32 LinkedNotebookTable::getLid(QString guid) {

    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":data", guid);
    query.bindValue(":key", LINKEDNOTEBOOK_GUID);
    query.exec();
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
}


// Given a notebook's GUID, we return the LID
qint32 LinkedNotebookTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new notebook to the database
qint32 LinkedNotebookTable::add(qint32 l, LinkedNotebook &t, bool isDirty) {
    ConfigStore cs(db);
    qint32 lid = l;
    if (lid == 0) {
        lid = cs.incrementLidCounter();
    }

    NSqlQuery query(db);
    NSqlQuery query2(db);

    db->lockForWrite();
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");

    query.bindValue(":lid", lid);
    query.bindValue(":key", LINKEDNOTEBOOK_GUID);
    QString linkedguid = "";
    if (t.guid.isSet())
        linkedguid = t.guid;
    query.bindValue(":data", linkedguid);
    query.exec();

    if (t.username.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_USERNAME);
        QString username = t.username;
        query.bindValue(":data",username);
        query.exec();
    }

    if (t.shardId.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_SHARD_ID);
        QString shardid = t.shardId;
        query.bindValue(":data",shardid);
        query.exec();
    }

    if (t.shareKey.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_SHARE_KEY);
        QString sharekey = t.shareKey;
        query.bindValue(":data", sharekey);
        query.exec();
    }

    if (t.uri.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_URI);
        QString uri = t.uri;
        query.bindValue(":data", uri);
        query.exec();
    }

    if (t.updateSequenceNum.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_UPDATE_SEQUENCE_NUMBER);
        qint32 usn = t.updateSequenceNum;
        query.bindValue(":data", usn);
        query.exec();
    }

    if (t.noteStoreUrl.isSet()) {
        QString url = t.noteStoreUrl;
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_NOTE_STORE_URL);
        query.bindValue(":data", url);
        query.exec();
    }

    if (t.webApiUrlPrefix.isSet()) {
        query.bindValue(":lid", lid);
        QString api = t.webApiUrlPrefix;
        query.bindValue(":key", LINKEDNOTEBOOK_WEB_API_URL_PREFIX);
        query.bindValue(":data", api);
        query.exec();
    }

    if (t.stack.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_STACK);
        QString stack = t.stack;
        query.bindValue(":data", stack);
        query.exec();
    }

    if (t.businessId.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_BUSINESS_ID);
        qint32 businessid = t.businessId;
        query.bindValue(":data", businessid);
        query.exec();
    }

    if (t.shareName.isSet()) {
        query.bindValue(":lid", lid);
        QString sharename = t.shareName;
        query.bindValue(":key", LINKEDNOTEBOOK_SHARE_NAME);
        query.bindValue(":data", sharename);
        query.exec();

        query2.prepare("Update datastore set data=:name where key=:key and lid=:lid");
        query2.bindValue(":name", sharename);
        query2.bindValue(":key", NOTEBOOK_NAME);
        query2.bindValue(":lid", lid);
        query2.exec();

        query2.prepare("Update notetable set notebook=:name where lid=:lid");
        query2.bindValue(":name", sharename);
        query2.bindValue(":lid", lid);
        query2.exec();
        query2.finish();
    }
    if (isDirty) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", LINKEDNOTEBOOK_IS_DIRTY);
        query.bindValue(":data", true);
        query.exec();
    }

    query.finish();
    db->unlock();
    return lid;
}




// Return a notebook structure given the LID
bool LinkedNotebookTable::get(LinkedNotebook &notebook, qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0)
        return false;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (LINKEDNOTEBOOK_GUID):
            notebook.guid = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_UPDATE_SEQUENCE_NUMBER):
            notebook.updateSequenceNum = query.value(1).toInt();
            break;
        case (LINKEDNOTEBOOK_SHARE_NAME):
            notebook.shareName = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_SHARE_KEY):
            notebook.shareKey = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_USERNAME):
            notebook.username = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_SHARD_ID):
            notebook.shardId = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_URI):
            notebook.uri = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_NOTE_STORE_URL):
            notebook.noteStoreUrl = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_WEB_API_URL_PREFIX):
            notebook.webApiUrlPrefix = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_STACK):
            notebook.stack = query.value(1).toString();
            break;
        case (LINKEDNOTEBOOK_BUSINESS_ID):
            notebook.businessId = query.value(1).toInt();
            break;
        }
    }
    query.finish();
    db->unlock();
    return true;
}



// Return a notebook given the GUID
bool LinkedNotebookTable::get(LinkedNotebook &notebook, QString guid) {
    qint32 lid = getLid(guid);
    return get(notebook, lid);
}



// Return a notebook given the GUID as a std::string
bool LinkedNotebookTable::get(LinkedNotebook &notebook, string guid) {
    qint32 lid = getLid(guid);
    return get(notebook, lid);
}



// Get a list of all notebooks
qint32 LinkedNotebookTable::getAll(QList<qint32> &books) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", LINKEDNOTEBOOK_GUID);
    query.exec();
    while (query.next()) {
        books.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return books.size();
}


// Get all notebooks for a particular stack
qint32 LinkedNotebookTable::getStack(QList<qint32> &retval, QString &stack){
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select distinct lid from DataStore where key=:key and data=:stack");
    query.bindValue(":key", LINKEDNOTEBOOK_STACK);
    query.bindValue(":stack", stack);
    query.exec();
    while (query.next()) {
        retval.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return retval.size();

}



// Get the guid for a particular lid
bool LinkedNotebookTable::getGuid(QString &retval, qint32 lid){
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", LINKEDNOTEBOOK_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    while (query.next()) {
        retval = query.value(0).toString();
        query.finish();
        db->unlock();
        return true;
    }
    query.finish();
    db->unlock();
    return false;

}



bool LinkedNotebookTable::findGuidByName(QString &retval, QString &guid) {
    NotebookTable ntable(db);
    return ntable.findGuidByName(retval, guid);
}


bool LinkedNotebookTable::update(LinkedNotebook &notebook, bool isDirty) {
    LinkedNotebook oldBook;
    qint32 lid = getLid(notebook.guid);
    get(oldBook, lid);
    if (lid <= 0)
        return false;
    expunge(lid);
    add(lid, notebook, isDirty);
    // Rename anything in the note list
    QString oldname = "";
    QString newname = "";
    if (notebook.shareName.isSet())
        newname = notebook.shareName;
    if (oldBook.shareName.isSet())
        oldname = oldBook.shareName;
    if (oldname != newname) {
        NSqlQuery query(db);
        db->lockForWrite();
        query.prepare("Update notetable set notebook=:name where notebooklid=:lid");
        query.bindValue(":name", newname);
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
        db->unlock();
    }
    return true;
}

void LinkedNotebookTable::expunge(qint32 lid) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
    db->unlock();
}


void LinkedNotebookTable::expunge(string guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}


void LinkedNotebookTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}

void LinkedNotebookTable::renameStack(QString oldName, QString newName) {
    QList<qint32> lids;
    findByStack(lids, oldName);
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update Datastore set data=:newname where key=:key and data=:oldname");
    query.bindValue(":newname", newName);
    query.bindValue(":key", LINKEDNOTEBOOK_STACK);
    query.bindValue(":oldName", oldName);
    query.exec();
    query.finish();
    db->unlock();
}


void LinkedNotebookTable::findByStack(QList<qint32> &lids, QString stackName) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", LINKEDNOTEBOOK_STACK);
    query.bindValue(":name", stackName);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
}


bool LinkedNotebookTable::isDeleted(qint32 lid) {
    NotebookTable ntable(db);
    return ntable.isDeleted(lid);
}


void LinkedNotebookTable::getStacks(QStringList &stacks) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select distinct data from DataStore where key=:key");
    query.bindValue(":key", LINKEDNOTEBOOK_STACK);
    query.exec();
    while (query.next()) {
        stacks.append(query.value(0).toString());
    }
    query.finish();
    db->unlock();
}


bool LinkedNotebookTable::isStacked(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", LINKEDNOTEBOOK_STACK);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = true;
    query.finish();
    db->unlock();
    return retval;
}


void LinkedNotebookTable::removeFromStack(qint32 lid) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", LINKEDNOTEBOOK_STACK);
    query.exec();
    query.finish();
    db->unlock();
}



qint32 LinkedNotebookTable::getLastUpdateSequenceNumber(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select data from datastore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", LINKEDNOTEBOOK_LAST_USN);
    query.exec();
    qint32 retval = 0;
    if (query.next()) {
        retval = query.value(0).toInt();
    }
    query.finish();
    db->unlock();
    return retval;
}



void LinkedNotebookTable::setLastUpdateSequenceNumber(qint32 lid, qint32 lastUSN) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from datastore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", LINKEDNOTEBOOK_LAST_USN);
    query.exec();

    query.prepare("insert into datastore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", LINKEDNOTEBOOK_LAST_USN);
    query.bindValue(":data", lastUSN);
    query.exec();
    query.finish();
    db->unlock();
}


bool LinkedNotebookTable::exists(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select lid from datastore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", LINKEDNOTEBOOK_SHARE_NAME);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = true;
    query.finish();
    db->unlock();
    return retval;
}
