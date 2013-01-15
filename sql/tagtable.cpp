#include "tagtable.h"
#include "configstore.h"
#include "notetable.h"

#include <QSqlTableModel>
#include <QList>




// Default constructor
TagTable::TagTable()
{
}



// Given a tag's name as a std::string, we return the lid
qint32 TagTable::findByName(string &name) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", TAG_NAME);
    query.bindValue(":name", QString::fromStdString(name));
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}




// Given a tag's name as a QString, we return the lid
qint32 TagTable::findByName(QString &name) {
    string n = name.toStdString();
    return findByName(n);
}



// Get a list of all tags
qint32 TagTable::getAll(QList<qint32> &tags) {
    QSqlQuery query;
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", TAG_GUID);
    query.exec();
    while (query.next()) {
        tags.append(query.value(0).toInt());
    }

    return tags.size();
}





// Given a tag's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void TagTable::updateGuid(qint32 lid, Guid &guid) {
    QString oldGuid;
    getGuid(oldGuid, lid);

    QSqlQuery query;
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.exec();

    query.prepare("Update DataStore set data=:data where key=:key and data=:oldGuid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":key", TAG_PARENT);
    query.bindValue(":oldGuid", oldGuid);
    query.exec();
}




// Synchronize a new tag with what is in the database.  We basically
// just delete the old one & give it a new entry
void TagTable::sync(Tag &tag) {
    qint32 lid= findByName(tag.name);
    sync(lid, tag);
}




// Update a tag record
void TagTable::update(Tag &tag, bool dirty=true) {
    qint32 lid = getLid(tag.guid);
    if (lid > 0) {
        QSqlQuery query;
        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();

        add(lid, tag, dirty);

        // Now update the Note display list
        NoteTable noteTable;
        QList<qint32> noteList;
        noteTable.findNotesByTag(noteList, lid);
        for (qint32 i=0; i<noteList.size(); i++) {
            noteTable.rebuildNoteListTags(noteList[i]);
        }

    }
}

// Synchronize a newtag with what is in the database.  We basically
// just delete the old one & give it a new entry
void TagTable::sync(qint32 l, Tag &tag) {
    qint32 lid = l;
    if (lid == 0)
        lid= findByName(tag.name);

    if (lid > 0) {
        QSqlQuery query;
        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
    } else {
        ConfigStore cs;
        lid = cs.incrementLidCounter();
    }

    add(lid, tag, false);
}




// Given a tag's GUID, we return the LID
qint32 TagTable::getLid(QString guid) {

    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":data", guid);
    query.bindValue(":key", TAG_GUID);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}


// Given a tag's GUID, we return the LID
qint32 TagTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new tag to the database
void TagTable::add(qint32 l, Tag &t, bool isDirty) {
    ConfigStore cs;
    qint32 lid = l;
    if (lid == 0)
        lid = cs.incrementLidCounter();

    QSqlQuery query;
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");

    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.bindValue(":data", QString::fromStdString(t.guid));
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_NAME);
    query.bindValue(":data", QString::fromStdString(t.name));
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":data", t.updateSequenceNum);
    query.exec();

    if (t.__isset.parentGuid) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", TAG_PARENT);
        query.bindValue(":data", QString::fromStdString(t.parentGuid));
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();
}



// Return a tag structure given the LID
bool TagTable::get(Tag &tag, qint32 lid) {

    QSqlQuery query;
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0)
        return false;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
            case (TAG_GUID):
                tag.guid = query.value(1).toString().toStdString();
                tag.__isset.guid = true;
                break;
            case (TAG_UPDATE_SEQUENCE_NUMBER):
                tag.updateSequenceNum = query.value(1).toInt();
                tag.__isset.updateSequenceNum = true;
                break;
            case (TAG_PARENT):
                tag.parentGuid = query.value(1).toString().toStdString();
                tag.__isset.parentGuid =true;
                break;
            case (TAG_NAME):
                tag.name = query.value(1).toString().toStdString();
                tag.__isset.name = true;
                break;
        }
    }

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
    QSqlQuery query;
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_ISDIRTY);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
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
bool TagTable::setDirty(string guid, bool dirty) {
    qint32 lid = getLid(guid);
    return setDirty(lid, dirty);
}


// Set a flag dirty or clean
bool TagTable::setDirty(QString guid, bool dirty) {
    qint32 lid = getLid(guid);
    return setDirty(lid, dirty);
}


// Return if a tag is dirty given its lid
bool TagTable::setDirty(qint32 lid, bool dirty) {
    QSqlQuery query;
    query.prepare("Update DataStore set data=:dirty where key=:key and lid=:lid");
    query.bindValue(":dirty", dirty);
    query.bindValue(":key", TAG_ISDIRTY);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}


// Does this tag exist?
bool TagTable::exists(qint32 lid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.exec();
    if (query.next())
        return true;
    else
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

    QSqlQuery query;
    query.prepare("Select data, data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.exec();
    while (query.next()) {
        guid = query.value(0).toString();
        return true;
    }

    return true;
}



// Delete this tag
void TagTable::deleteTag(qint32 lid) {
    if (!exists(lid))
        return;

    // First find all the children & delete them
    QList<qint32> list;
    QString parentGuid;
    getGuid(parentGuid, lid);
    findChildren(list, parentGuid);
    getGuid(parentGuid, lid);

    // Now add this lid to the list to be deleted
    list.append(lid);

    for (qint32 i=0; i<list.size(); i++) {
        Tag tag;
        qint32 currentLid = list[i];
        get(tag, currentLid);
        if (tag.__isset.updateSequenceNum && tag.updateSequenceNum > 0) {
            QSqlQuery query;
            query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, :data)");
            query.bindValue(":lid", currentLid);
            query.bindValue(":key", TAG_ISDELETED);
            query.bindValue(":data", true);
            query.exec();
        } else {
            expunge(currentLid);
        }
    }
}


void TagTable::expunge(qint32 lid) {
    QString tagGuid;
    this->getGuid(tagGuid, lid);
    QSqlQuery query;
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();

    NoteTable noteTable;
    QList<int> notes;
    noteTable.findNotesByTag(notes, tagGuid);
    for (int i=0; i<notes.size(); i++)
        noteTable.removeTag(notes[i], lid, false);
}


void TagTable::expunge(string guid) {
    int lid = this->getLid(guid);
    expunge(lid);
}

void TagTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    expunge(lid);
}


// Is this search deleted?
bool TagTable::isDeleted(qint32 lid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where lid=:lid and key=:key and data=:data");
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_ISDELETED);
    query.bindValue(":data", true);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}


qint32 TagTable::findChildren(QList<qint32> &list, QString parentGuid) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:parent");
    query.bindValue(":key", TAG_PARENT);
    query.bindValue(":parent", parentGuid);
    query.exec();

    // Now find all the children for each found tag
    while (query.next()) {
        qint32 nextChild = query.value(0).toInt();
        QString nextGuid;
        getGuid(nextGuid, nextChild);
        findChildren(list, nextGuid);
        list.append(nextChild);
    }
    return list.size();
}

