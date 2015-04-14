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


#include "searchtable.h"
#include "configstore.h"

#include <QSqlTableModel>
#include "sql/nsqlquery.h"

#include "global.h"
extern Global global;


// Default constructor
SearchTable::SearchTable(DatabaseConnection *db)
{
    this->db = db;
}


// Get the LIDs for all searches
void SearchTable::getAll(QList<qint32> &lids) {
    lids.empty();
    NSqlQuery query(db);
    query.prepare("Select lid from datastore where key=:key");
    query.bindValue(":key", SEARCH_GUID);
    query.exec();
    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
}

// Given a record's name as a std::string, we return the lid
qint32 SearchTable::findByName(string &name) {
    QLOG_TRACE() << "Entering SearchTable::findByName()";
    qint32 retval = 0;
    NSqlQuery query(db);
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", SEARCH_NAME);
    query.bindValue(":name", QString::fromStdString(name));
    query.exec();
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    return retval;
}




// Given a search's name as a QString, we return the lid
qint32 SearchTable::findByName(QString &name) {
    string n = name.toStdString();
    return findByName(n);
}




// Given a search's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void SearchTable::updateGuid(qint32 lid, Guid &guid) {
    NSqlQuery query(db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", guid);
    query.bindValue(":lid", lid);
    query.bindValue(":key", SEARCH_GUID);
    query.exec();
    query.finish();
}




// Synchronize a new search with what is in the database.  We basically
// just delete the old one & give it a new entry
void SearchTable::sync(SavedSearch &search) {
    sync(0, search);
}



// Synchronize a new search with what is in the database.  We basically
// just delete the old one & give it a new entry
void SearchTable::sync(qint32 lid, SavedSearch &search) {
    NSqlQuery query(db);

    if (lid > 0) {
        query.prepare("Delete from DataStore where lid=:lid");
        query.bindValue(":lid", lid);
        query.exec();
    } else {
        ConfigStore cs(db);
        lid = cs.incrementLidCounter();
    }

    add(lid, search, false);
}




// Given a record's GUID, we return the LID
qint32 SearchTable::getLid(QString guid) {
    qint32 retval = 0;
    NSqlQuery query(db);
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":data", guid);
    query.bindValue(":key", SEARCH_GUID);
    query.exec();
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    return retval;
}


// Given a search's GUID, we return the LID
qint32 SearchTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new search to the database
void SearchTable::add(qint32 l, SavedSearch &t, bool isDirty) {
    ConfigStore cs(db);
    qint32 lid = l;
    if (lid == 0)
        lid = cs.incrementLidCounter();

    NSqlQuery query(db);
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");

    if (t.guid.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_GUID);
        QString guid = t.guid;
        query.bindValue(":data", guid);
        query.exec();
    }

    if (t.name.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_NAME);
        QString name = t.name;
        query.bindValue(":data", name);
        query.exec();
    }

    if (t.updateSequenceNum.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_UPDATE_SEQUENCE_NUMBER);
        qint32 usn = t.updateSequenceNum;
        query.bindValue(":data", usn);
        query.exec();
    } else {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_UPDATE_SEQUENCE_NUMBER);
        query.bindValue(":data", 0);
        query.exec();
    }

    if (t.format.isSet()) {
        int format = t.format;
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_FORMAT);
        query.bindValue(":data", format);
        query.exec();
    }

    if (t.query.isSet()) {
        QString q= t.query;
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_QUERY);
        query.bindValue(":data", q);
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", SEARCH_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();
    query.finish();
}



// Return a search's structure given the LID
bool SearchTable::get(SavedSearch &search ,qint32 lid) {

    NSqlQuery query(db);
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0) {
        query.finish();
        return false;
    }
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (SEARCH_GUID):
            search.guid = query.value(1).toString();
            break;
        case (SEARCH_UPDATE_SEQUENCE_NUMBER):
            search.updateSequenceNum = query.value(1).toInt();
            break;
        case (SEARCH_NAME):
            search.name = query.value(1).toString();
            break;
        case (SEARCH_QUERY):
            search.query = query.value(1).toString();
            break;
        case (SEARCH_FORMAT):
            qint32 value = query.value(1).toInt();
            search.format = QueryFormat::USER;
            if (value == QueryFormat::SEXP) search.format = QueryFormat::SEXP;
            break;
        }
    }
    query.finish();
    return true;
}



// Return a search given the GUID
bool SearchTable::get(SavedSearch &search, QString guid) {
    qint32 lid = getLid(guid);
    return get(search, lid);
}



// Return a search given the GUID as a std::string
bool SearchTable::get(SavedSearch &search, string guid) {
    qint32 lid = getLid(guid);
    return get(search, lid);
}



// Return if a search is dirty given its lid
bool SearchTable::isDirty(qint32 lid) {
    bool retval = false;
    NSqlQuery query(db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SEARCH_ISDIRTY);
    query.exec();
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    return retval;
}


// Determine if a search is dirty given a guid
bool SearchTable::isDirty(QString guid) {
    qint32 lid = getLid(guid);
    return isDirty(lid);
}


// Determine if a search is dirty given a guid
bool SearchTable::isDirty(string guid) {
    QString g(QString::fromStdString(guid));
    return isDirty(g);
}


// Does this search exist?
bool SearchTable::exists(qint32 lid) {
    NSqlQuery query(db);
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SEARCH_GUID);
    query.exec();
    if (query.next()) {
        query.finish();
        return true;
    }
    query.finish();
    return false;
}


// Set the search as "dirty" so it is synchronized next time
void SearchTable::setDirty(qint32 lid, bool dirty) {
    NSqlQuery query(db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", dirty);
    query.bindValue(":lid", lid);
    query.bindValue(":key", SEARCH_ISDIRTY);
    query.exec();
    query.finish();
}


// Delete this search
void SearchTable::deleteSearch(qint32 lid) {
    if (!exists(lid))
        return;
    SavedSearch s;
    get(s, lid);
    if (s.updateSequenceNum.isSet() && s.updateSequenceNum > 0) {
        NSqlQuery query(db);
        query.prepare("Delet from DataStore where key=:key and lid=:lid");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_ISDELETED);
        query.exec();
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":data", true);
        query.bindValue(":lid", lid);
        query.bindValue(":key", SEARCH_ISDELETED);
        query.exec();
        query.finish();
        setDirty(lid,true);
    } else {
        expunge(lid);
    }
}


void SearchTable::expunge(qint32 lid) {
    NSqlQuery query(db);
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
}


// Erase a search
void SearchTable::expunge(string guid) {
    expunge(getLid(guid));
}



// Erase a search
void SearchTable::expunge(QString guid) {
    expunge(getLid(guid));
}




// Determine if a search is dirty given a guid
bool SearchTable::exists(QString guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}


// Determine if a search is dirty given a guid
bool SearchTable::exists(string guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}



// Update an existing saved search
bool SearchTable::update(qint32 lid, SavedSearch &s, bool isDirty=true) {
    this->sync(lid, s);
    this->setDirty(lid, isDirty);
    return true;
}


// Is this search deleted?
bool SearchTable::isDeleted(qint32 lid) {
    NSqlQuery query(db);
    query.prepare("Select lid from DataStore where key=:key and lid=:lid and data='true'");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SEARCH_ISDELETED);
    query.exec();
    if (query.next()) {
        query.finish();
        return true;
    }
    query.finish();
    return false;
}



// Get all dirty lids
qint32 SearchTable::getAllDirty(QList<qint32> &lids) {
    NSqlQuery query(db);
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data='true'");
    query.bindValue(":key", SEARCH_ISDIRTY);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    return lids.size();
}



// Update the USN
void SearchTable::setUpdateSequenceNumber(qint32 lid, qint32 usn) {
    NSqlQuery query(db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", usn);
    query.bindValue(":lid", lid);
    query.bindValue(":key", SEARCH_UPDATE_SEQUENCE_NUMBER);
    query.exec();
    query.finish();
}


// Get all dirty lids
QString SearchTable::getGuid(qint32 lid) {
    QString retval = "";
    NSqlQuery query(db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", SEARCH_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    if(query.next()) {
        retval = query.value(0).toString();
    }
    query.finish();
    return retval;
}
