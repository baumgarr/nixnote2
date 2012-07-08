#include "tagtable.h"
#include "configstore.h"

#include <QSqlTableModel>




// Default constructor
TagTable::TagTable()
{
}



// Given a tag's name as a std::string, we return the lid
int TagTable::findByName(string &name) {
    QLOG_TRACE() << "Entering TagTable::findTagByName()";
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
int TagTable::findByName(QString &name) {
    string n = name.toStdString();
    return findByName(n);
}



// Get a list of all tags
int TagTable::getAll(QList<int> &tags) {
    QLOG_TRACE() << "Entering TagTable::getAll()";

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
void TagTable::updateGuid(int lid, Guid &guid) {
    QLOG_TRACE() << "Entering TagTable::updateTagGuid()";

    QSqlQuery query;
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":lid", lid);
    query.bindValue(":key", TAG_GUID);
    query.exec();

    QLOG_TRACE() << "Leaving TagTable::updateTagGuid()";
}




// Synchronize a new tag with what is in the database.  We basically
// just delete the old one & give it a new entry
void TagTable::sync(Tag &tag) {
    QLOG_TRACE() << "Leaving TagTable::syncTag()";
    int lid= findByName(tag.name);
    sync(lid, tag);
    QLOG_TRACE() << "Leaving TagTable::syncTag()";
}



// Synchronize a new tag with what is in the database.  We basically
// just delete the old one & give it a new entry
void TagTable::sync(int l, Tag &tag) {
    QLOG_TRACE() << "Entering TagTable::syncTag()";
    int lid = l;
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

    QLOG_TRACE() << "Leaving TagTable::syncTag()";
}




// Given a tag's GUID, we return the LID
int TagTable::getLid(QString guid) {

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
int TagTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new tag to the database
void TagTable::add(int l, Tag &t, bool isDirty) {
    ConfigStore cs;
    int lid = l;
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
bool TagTable::get(Tag &tag, int lid) {

    QSqlQuery query;
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0)
        return false;
    while (query.next()) {
        int key = query.value(0).toInt();
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
    int lid = getLid(guid);
    return get(tag, lid);
}



// Return a tag given the GUID as a std::string
bool TagTable::get(Tag &tag, string guid) {
    int lid = getLid(guid);
    return get(tag, lid);
}



// Return if a tag is dirty given its lid
bool TagTable::isDirty(int lid) {
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
    int lid = getLid(guid);
    return isDirty(lid);
}


// Determine if a tag is dirty given a guid
bool TagTable::isDirty(string guid) {
    QString g(QString::fromStdString(guid));
    return isDirty(g);
}


// Set a flag dirty or clean
bool TagTable::setDirty(string guid, bool dirty) {
    int lid = getLid(guid);
    return setDirty(lid, dirty);
}


// Set a flag dirty or clean
bool TagTable::setDirty(QString guid, bool dirty) {
    int lid = getLid(guid);
    return setDirty(lid, dirty);
}


// Return if a tag is dirty given its lid
bool TagTable::setDirty(int lid, bool dirty) {
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
bool TagTable::exists(int lid) {
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
    int lid = getLid(guid);
    return exists(lid);
}


// Determine if a tag is dirty given a guid
bool TagTable::exists(string guid) {
    int lid = getLid(guid);
    return exists(lid);
}


// Return a tag guid given the LID
bool TagTable::getGuid(QString &guid, int lid) {

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

