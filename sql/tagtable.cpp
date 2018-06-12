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

#include "tagtable.h"
#include "configstore.h"
#include "notetable.h"
#include "sql/nsqlquery.h"

#include <QSqlTableModel>
#include <QList>

#include "global.h"

extern Global global;


// Default constructor
TagTable::TagTable(DatabaseConnection *db)
{
    this->db = db;
}



// Given a tag's name as a std::string, we return the lid
qint32 TagTable::findByName(string &name, qint32 account) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", TAG_NAME);
    query.bindValue(":name", QString::fromStdString(name));
    query.exec();

    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        if (account == 0 || owningAccount(lid) == account) {
            query.finish();
            db->unlock();
            return lid;
        }
    }
    query.finish();
    db->unlock();
    return 0;
}




// Given a tag's name as a QString, we return the lid
qint32 TagTable::findByName(QString &name, qint32 account) {
    string n = name.toStdString();
    return findByName(n, account);
}


// Determine if a lid belongs to a linked notebook
bool TagTable::isLinked(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from datastore where key=:key and lid=:lid");
    query.bindValue(":key", TAG_OWNING_ACCOUNT);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next()) {
        query.finish();
        db->unlock();
        return true;
    }
    query.finish();
    db->unlock();
    return false;
}


// Get the owning account for a lid
qint32 TagTable::owningAccount(qint32 lid) {
    qint32 retval = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from datastore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_OWNING_ACCOUNT);
    query.exec();
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    return retval;
}



// Get a list of all tag names hashed by LID
void TagTable::getAllNames(QHash<qint32, QString> *list) {
    NSqlQuery query(db);
    list->clear();
    db->lockForRead();
    query.prepare("select lid, data from DataStore where key=:key");
    query.bindValue(":key", TAG_NAME);
    query.exec();
    while (query.next()) {
        list->insert(query.value(0).toInt(), query.value(1).toString());
    }
    query.finish();
    db->unlock();
    return;
}



// Get a list of all tags
qint32 TagTable::getAllInAccount(QList<qint32> &tags, qint32 account) {
    NSqlQuery query(db);
    db->lockForRead();
    if (account != 0)
        query.prepare("select distinct lid from DataStore where key=:key and lid in (select lid from DataStore where key=:key2 and data=:account)");
    else
        query.prepare("select distinct lid from DataStore where key=:key and lid not in (select lid from DataStore where key=:key2)");
    query.bindValue(":key", TAG_GUID);
    query.bindValue(":key2", TAG_OWNING_ACCOUNT);
    if (account !=0)
        query.bindValue(":account", account);
    query.exec();
    while (query.next()) {
        tags.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return tags.size();
}




// Get a list of all tags
qint32 TagTable::getAll(QList<qint32> &tags) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", TAG_GUID);
    query.exec();
    while (query.next()) {
        tags.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    return tags.size();
}





// Given a tag's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void TagTable::updateGuid(qint32 lid, Guid &guid) {
    QLOG_TRACE_IN();
    QString oldGuid;
    getGuid(oldGuid, lid);

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", guid);
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.exec();
    query.finish();
    db->unlock();
    QLOG_TRACE_OUT();
}




// Synchronize a new tag with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 TagTable::sync(Tag &tag, qint32 account) {
    qint32 lid= getLid(tag.guid);
    return sync(lid, tag, account);
}




// Update a tag record
void TagTable::update(Tag &tag, bool dirty=true) {
    qint32 lid = getLid(tag.guid);
    if (lid > 0) {
        NSqlQuery query(db);
        // Delete the old record
        db->lockForWrite();
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
        db->unlock();
        qint32 account = owningAccount(lid);
        add(lid, tag, dirty,account);

        // Now update the Note display list
        NoteTable noteTable(db);
        QList<qint32> noteList;
        noteTable.findNotesByTag(noteList, lid);
        for (qint32 i=0; i<noteList.size(); i++) {
            noteTable.rebuildNoteListTags(noteList[i]);
        }
    }
}


// Synchronize a newtag with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 TagTable::sync(qint32 l, Tag &tag, qint32 account) {
    qint32 lid = l;
    if (lid == 0)
        lid= getLid(tag.name);

    if (lid > 0) {
        NSqlQuery query(db);
        // Delete the old record
        db->lockForWrite();
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
        db->unlock();
    } else {
        ConfigStore cs(db);
        lid = cs.incrementLidCounter();
    }

    add(lid, tag, false, account);
    return lid;
}




// Given a tag's GUID, we return the LID
qint32 TagTable::getLid(QString guid) {
    QLOG_TRACE_IN();
    qint32 retval = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":data", guid);
    query.bindValue(":key", TAG_GUID);
    query.exec();
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    db->unlock();
    QLOG_TRACE_OUT();
    return retval;
}


// Given a tag's GUID, we return the LID
qint32 TagTable::getLid(string guid) {
    QLOG_TRACE_IN();
    QString s(QString::fromStdString(guid));
    QLOG_TRACE_OUT();
    return getLid(s);
}



// Add a new tag to the database
qint32 TagTable::add(qint32 l, Tag &t, bool isDirty, qint32 account) {
    ConfigStore cs(db);
    qint32 lid = l;
    if (lid == 0)
        lid = cs.incrementLidCounter();

    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");

    if (t.guid.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", TAG_GUID);
        QString guid = t.guid;
        query.bindValue(":data", guid);
        query.exec();
    }

    if (t.name.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", TAG_NAME);
        QString name  = t.name;
        query.bindValue(":data", name);
        query.exec();
    }

    qint32 usn = 0;
    if (t.updateSequenceNum.isSet())
        usn = t.updateSequenceNum;
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":data", usn);
    query.exec();

    if (t.parentGuid.isSet()) {
        QString parentGuid = t.parentGuid;
        if (parentGuid != "") {
            db->unlock();
            qint32 parentLid = getLid(t.parentGuid);
            if (parentLid == 0) {
                Tag tempTag;
                parentLid = cs.incrementLidCounter();
                tempTag.guid = t.parentGuid;
                tempTag.name="<no name>";
                tempTag.updateSequenceNum = 0;
                add(parentLid, tempTag, false, account);
            }
            db->lockForWrite();
            query.bindValue(":lid", lid);
            query.bindValue(":key", TAG_PARENT_LID);
            query.bindValue(":data", parentLid);
            query.exec();
        }
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();

    if (account >0) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", TAG_OWNING_ACCOUNT);
        query.bindValue(":data", account);
        query.exec();
    }
    query.finish();
    db->unlock();
    return lid;
}



// Return a tag structure given the LID
bool TagTable::get(Tag &tag, qint32 lid) {
    QLOG_TRACE_IN();
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0) {
        query.finish();
        db->unlock();
        QLOG_TRACE_OUT();
        return false;
    }
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
            case (TAG_GUID):
                tag.guid = query.value(1).toString();
                break;
            case (TAG_UPDATE_SEQUENCE_NUMBER):
                tag.updateSequenceNum = query.value(1).toInt();
                break;
            case (TAG_PARENT_LID): {
                if (query.value(1).toInt() > 0) {
                        QString parentGuid;
                        if (getGuid(parentGuid, query.value(1).toInt()))
                            tag.parentGuid = parentGuid;
                        break;
                    }
                }
            case (TAG_NAME):
                tag.name = query.value(1).toString();
                break;
            case (TAG_ISDIRTY):
                // currently just ignore the dirty flag
                // fixes https://github.com/baumgarr/nixnote2/issues/363
                break;

            default: {
                QLOG_ERROR() << "Unknown Tag record key: " << key << " lid: " << lid;
            }
        }
    }
    query.finish();
    db->unlock();
    if (!tag.name.isSet())
        tag.name = "** Unknown **";
    QLOG_TRACE_OUT();
    return true;
}



// Return a tag given the GUID
bool TagTable::get(Tag &tag, QString guid) {
    qint32 lid = getLid(guid);
    return get(tag, lid);
}



// Return a tag given the GUID as a std::string
bool TagTable::get(Tag &tag, string guid) {
    qint32 lid = getLid(guid);
    return get(tag, lid);
}



// Return if a tag is dirty given its lid
bool TagTable::isDirty(qint32 lid) {
    QLOG_TRACE_IN();
    NSqlQuery query(db);
    bool retval = false;
    db->lockForRead();
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_ISDIRTY);
    query.exec();
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    db->unlock();
    QLOG_TRACE_OUT();
    return retval;
}


// Determine if a tag is dirty given a guid
bool TagTable::isDirty(QString guid) {
    qint32 lid = getLid(guid);
    return isDirty(lid);
}


// Determine if a tag is dirty given a guid
bool TagTable::isDirty(string guid) {
    QString g(QString::fromStdString(guid));
    return isDirty(g);
}


// Set a flag dirty or clean
void TagTable::setDirty(string guid, bool dirty) {
    qint32 lid = getLid(guid);
    return setDirty(lid, dirty);
}


// Set a flag dirty or clean
void TagTable::setDirty(QString guid, bool dirty) {
    qint32 lid = getLid(guid);
    return setDirty(lid, dirty);
}


// Return if a tag is dirty given its lid
void TagTable::setDirty(qint32 lid, bool dirty) {
    //bool retval = false;
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update DataStore set data=:dirty where key=:key and lid=:lid");
    query.bindValue(":dirty", dirty);
    query.bindValue(":key", TAG_ISDIRTY);
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
    db->unlock();
}


// Does this tag exist?
bool TagTable::exists(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.exec();
    if (query.next()) {
        query.finish();
        db->unlock();
        return true;
    }
    query.finish();
    db->unlock();
    return false;
}


// Determine if a tag is dirty given a guid
bool TagTable::exists(QString guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}


// Determine if a tag is dirty given a guid
bool TagTable::exists(string guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}


// Return a tag guid given the LID
bool TagTable::getGuid(QString &guid, qint32 lid) {
    QLOG_TRACE_IN();

    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data, data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.exec();
    while (query.next()) {
        guid = query.value(0).toString();
        query.finish();
        db->unlock();
        QLOG_TRACE_OUT();
        return true;
    }
    query.finish();
    db->unlock();
    QLOG_TRACE_OUT();
    return false;
}



// Delete this tag
void TagTable::deleteTag(qint32 lid) {
    if (!exists(lid))
        return;

    // First find the current tag we want to delet.
    // If it has a parent, then we save it because
    // when we delete the current tag all of the children
    // will be promoted to the current tag's parent
    Tag currentTag;
    QString newParent = "";
    get(currentTag, lid);
    if (currentTag.parentGuid.isSet())
        newParent = currentTag.parentGuid;

    // Now, get a list of all the children.
    // The children will now be given a new parent
    QList<qint32> list;
    QString currentGuid;
    getGuid(currentGuid, lid);
    findChildren(list, currentGuid);
    for (int i=0; i<list.size(); i++) {
        Tag childTag;
        get(childTag, list[i]);
        childTag.parentGuid = newParent;
        update(childTag, true);
    }


    // Now, we need to delete the actual tag.
    NoteTable noteTable(db);
    NSqlQuery query(db);
    if (currentTag.updateSequenceNum.isSet() && currentTag.updateSequenceNum > 0) {
        db->lockForWrite();
        query.prepare("delete from DataStore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", TAG_ISDELETED);
        query.exec();
        query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", TAG_ISDELETED);
        query.bindValue(":data", true);
        query.exec();
        db->unlock();
        setDirty(lid, true);
    } else {
        expunge(lid);
    }
    query.finish();

    // Now, find all the notes with this tag and remove them.
    QList<qint32> noteLids;
    noteTable.findNotesByTag(noteLids, lid);
    for (int j=0; j<noteLids.size(); j++) {
        noteTable.removeTag(noteLids[j], lid, false);
    }
}



// Erase a tag
void TagTable::expunge(qint32 lid) {
    QString tagGuid;
    this->getGuid(tagGuid, lid);
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
    db->unlock();

    NoteTable noteTable(db);
    QList<int> notes;
    noteTable.findNotesByTag(notes, tagGuid);
    for (int i=0; i<notes.size(); i++)
        noteTable.removeTag(notes[i], lid, false);
}


// Erase a tag
void TagTable::expunge(string guid) {
    int lid = this->getLid(guid);
    expunge(lid);
}


// Erase a tag
void TagTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    expunge(lid);
}


// Is this search deleted?
bool TagTable::isDeleted(qint32 lid) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where lid=:lid and key=:key and data=:data");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_ISDELETED);
    query.bindValue(":data", true);
    query.exec();
    if (query.next()) {
        query.finish();
        db->unlock();
        return true;
    }
    query.finish();
    db->unlock();
    return false;
}


qint32 TagTable::findChildren(QList<qint32> &list, QString parentGuid) {
    qint32 parentLid = getLid(parentGuid);
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select lid from DataStore where key=:key and data=:parent");
    query.bindValue(":key", TAG_PARENT_LID);
    query.bindValue(":parent", parentLid);
    query.exec();
    db->unlock();

    // Now find all the children for each found tag
    while (query.next()) {
        qint32 nextChild = query.value(0).toInt();
        QString nextGuid;
        getGuid(nextGuid, nextChild);
        findChildren(list, nextGuid);
        list.append(nextChild);
    }
    query.finish();
    return list.size();
}





// Get all dirty lids
qint32 TagTable::getAllDirty(QList<qint32> &lids) {
    QLOG_TRACE_IN();
    NSqlQuery query(db);
    db->lockForRead();
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data=1");
    query.bindValue(":key", TAG_ISDIRTY);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    db->unlock();
    QLOG_TRACE_OUT();
    return lids.size();
}



// Update the USN
void TagTable::setUpdateSequenceNumber(qint32 lid, qint32 usn) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", usn);
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_UPDATE_SEQUENCE_NUMBER);
    query.exec();
    query.finish();
    db->unlock();
}



// Linked tags are not uploaded, so we reset the dirty flags in case
// they were updated locally
void TagTable::resetLinkedTagsDirty() {
    QLOG_TRACE_IN();
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Delete from datastore where key=:key and lid in (select lid from datastore where key=:linkedkey and data > 0)");
    query.bindValue(":key", TAG_ISDIRTY);
    query.bindValue(":linkedkey", TAG_OWNING_ACCOUNT);
    query.exec();
    query.finish();
    db->unlock();
    QLOG_TRACE_OUT();
}




// Get a count of all new unsequenced notebooks
// Get all dirty lids
int TagTable::getNewUnsequencedCount() {
    NSqlQuery query(db);
    db->lockForRead();
    qint32 retval = 0;
    query.prepare("Select count(lid) from DataStore where key=:key and data=0");
    query.bindValue(":key", TAG_UPDATE_SEQUENCE_NUMBER);
    query.exec();
    while(query.next()) {
       retval = query.value(0).toInt();
    }
    query.finish();
    db->unlock();
    return retval;
}


// This function will look at all the tags in linked notebooks.  If any tag has a name of
// <no name> then it is missing.  This can happen because we get a parent guid from a tag
// sync chunk, but not the actual parent tag.  When this happens we delete the <no name> tag
// and remove the parent record.
void TagTable::cleanupLinkedTags() {
    // Delete the parent records from the children
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from datastore where data in (select lid from datastore where lid in (select lid from datastore where data='<no name>' and key=:nameKey and lid in (select lid from datastore where key=:owningKey)) and key=:parentKey)");
    query.bindValue(":parentKey", TAG_PARENT_LID);
    query.bindValue(":nameKey", TAG_NAME);
    query.bindValue(":owningKey", TAG_OWNING_ACCOUNT);
    query.exec();

    // Delete the actual parent
    query.prepare("delete from datastore where lid in (select lid from datastore where data='<no name>' and key=:nameKey and lid in (select lid from datastore where key=:owningKey))");
    query.bindValue(":owningKey", TAG_OWNING_ACCOUNT);
    query.bindValue(":nameKey", TAG_NAME);
    query.exec();
    query.finish();
    db->unlock();
}



// Get a hashtable of tags by GUID
void TagTable::getGuidMap(QHash<QString,QString> &guidMap) {
    QList<qint32> tags;
    Tag t;
    getAll(tags);
    guidMap.empty();
    for (int i=0; i<tags.size(); i++) {
        get(t,tags[i]);
        QString guid = "";
        QString name = "";
        if (t.guid.isSet())
            guid = t.guid;
        if (t.name.isSet())
            name = t.name;
        guidMap.insert(t.guid, t.name);
    }
}



// Get a hash table of tags by name
void TagTable::getNameMap(QHash<QString,QString> &nameMap) {
    QList<qint32> tags;
    Tag t;
    getAll(tags);
    nameMap.empty();
    for (int i=0; i<tags.size(); i++) {
        get(t,tags[i]);
        nameMap.insert(t.name, t.guid);
    }
}




// Find any invalid parent records
void TagTable::findMissingParents(QList<qint32> &lids) {
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("select data from datastore where key=:parentKey and data not in (select lid from datastore where key=:guidKey)");
    query.bindValue(":parentKey", TAG_PARENT_LID);
    query.bindValue(":guidKey", TAG_GUID);
    query.exec();
    lids.clear();

    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        lids.append(lid);
    }
    query.finish();
    db->unlock();
}


// Remove any invalid parent records
void TagTable::cleanupMissingParents() {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from datastore where key=:parentKey and data not in (select lid from datastore where key=:guidKey)");
    query.bindValue(":parentKey", TAG_PARENT_LID);
    query.bindValue(":guidKey", TAG_GUID);
    query.exec();
    query.finish();
    db->unlock();
}
