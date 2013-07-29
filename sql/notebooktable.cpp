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

#include "notebooktable.h"
#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "sql/configstore.h"
#include "sql/notetable.h"
#include "sql/sharednotebooktable.h"
#include "sql/linkednotebooktable.h"
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>


// Default constructor
NotebookTable::NotebookTable(QSqlDatabase *db)
{
    if (db != NULL)
        this->db = db;
    else
        this->db = global.db;
}



// Given a notebook's name as a std::string, we return the lid
qint32 NotebookTable::findByName(string &name) {
    QSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", NOTEBOOK_NAME);
    query.bindValue(":name", QString::fromStdString(name));
    query.exec();
    if (query.next())
        return query.value(0).toInt();

    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", NOTEBOOK_ALIAS);
    query.bindValue(":name", QString::fromStdString(name));
    query.exec();
    if (query.next())
        return query.value(0).toInt();

    return 0;
}




// Given a notebook's name as a QString, we return the lid
qint32 NotebookTable::findByName(QString &name) {
    string n = name.toStdString();
    return findByName(n);
}




// Given a notebook's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void NotebookTable::updateGuid(qint32 lid, Guid &guid) {
    QSqlQuery query(*db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
}




// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 NotebookTable::sync(Notebook &notebook) {
    return sync(0, notebook);
}



// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 NotebookTable::sync(qint32 lid, Notebook &notebook) {
    if (lid > 0) {
        QSqlQuery query(*db);
        NoteTable noteTable;
        noteTable.updateNotebookName(lid, QString::fromStdString(notebook.name));

        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid and key>=3000 and key<3200");
        query.bindValue(":lid", lid);
        query.exec();
    } else {
        ConfigStore cs;
        lid = cs.incrementLidCounter();
    }

    return add(lid, notebook, false);
}




// Given a notebook's GUID, we return the LID
qint32 NotebookTable::getLid(QString guid) {

    QSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":data", guid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}


// Given a notebook's GUID, we return the LID
qint32 NotebookTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}


// Add a new notebook to the database
qint32 NotebookTable::addStub(QString guid) {
    QSqlQuery query(*db);
    ConfigStore cs;
    qint32 lid = cs.incrementLidCounter();
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":data", guid);
    query.exec();

}

// Add a new notebook to the database
qint32 NotebookTable::add(qint32 l, Notebook &t, bool isDirty, bool isLocal) {
    QSqlQuery query(*db);
    ConfigStore cs;

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    qint32 lid = l;
    if (lid == 0) {
        lid = cs.incrementLidCounter();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":data", QString::fromStdString(t.guid));
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_NAME);
    query.bindValue(":data", QString::fromStdString(t.name));
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":data", t.updateSequenceNum);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_LOCAL);
    query.bindValue(":data", isLocal);
    query.exec();

    if (t.__isset.defaultNotebook) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_IS_DEFAULT);
        query.bindValue(":data", t.defaultNotebook);
        query.exec();
    }

    if (t.__isset.stack) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_STACK);
        query.bindValue(":data", QString::fromStdString(t.stack));
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();

    if (t.__isset.serviceCreated) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_SERVICE_CREATED);
        query.bindValue(":data", QVariant::fromValue(t.serviceCreated));
        query.exec();
    }

    if (t.__isset.serviceUpdated) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_SERVICE_UPDATED);
        query.bindValue(":data", QVariant::fromValue(t.serviceUpdated));
        query.exec();
    }

    if (t.__isset.published) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_PUBLISHED);
        query.bindValue(":data", t.published);
        query.exec();
    }

    if (t.__isset.publishing) {

        if (t.publishing.__isset.uri) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_URI);
            query.bindValue(":data", QString::fromStdString(t.publishing.uri));
            query.exec();
        }

        if (t.publishing.__isset.order) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_ORDER);
            query.bindValue(":data", t.publishing.order);
            query.exec();
        }

        if (t.publishing.__isset.ascending) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_ASCENDING);
            query.bindValue(":data", t.publishing.ascending);
            query.exec();
        }

        if (t.publishing.__isset.publicDescription) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_DESCRIPTION);
            query.bindValue(":data", QString::fromStdString(t.publishing.publicDescription));
            query.exec();
        }
    }

    if (t.__isset.sharedNotebooks) {
        SharedNotebookTable sharedTable;
        for (int i=0; i<t.sharedNotebooks.size(); i++) {
            sharedTable.add(lid, t.sharedNotebooks[i], isDirty);
        }
    }

    NoteTable noteTable;
    noteTable.updateNotebookName(lid, QString::fromStdString(t.name));
    return lid;
}


// Return a notebook structure given the LID
bool NotebookTable::get(Notebook &notebook, qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0)
        return false;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (NOTEBOOK_GUID):
            notebook.guid = query.value(1).toString().toStdString();
            notebook.__isset.guid = true;
            break;
        case (NOTEBOOK_UPDATE_SEQUENCE_NUMBER):
            notebook.updateSequenceNum = query.value(1).toInt();
            notebook.__isset.updateSequenceNum = true;
            break;
        case (NOTEBOOK_STACK):
            notebook.stack = query.value(1).toString().toStdString();
            notebook.__isset.stack =true;
            break;
        case (NOTEBOOK_NAME):
            notebook.name = query.value(1).toString().toStdString();
            notebook.__isset.name = true;
            break;
        case (NOTEBOOK_IS_DEFAULT):
            notebook.defaultNotebook = query.value(1).toBool();
            notebook.__isset.defaultNotebook = true;
            break;
        case (NOTEBOOK_SERVICE_CREATED):
            notebook.serviceCreated = query.value(1).toLongLong();
            notebook.__isset.serviceCreated = true;
            break;
        case (NOTEBOOK_SERVICE_UPDATED):
            notebook.serviceUpdated = query.value(1).toLongLong();
            notebook.__isset.serviceUpdated = true;
            break;
        case (NOTEBOOK_PUBLISHED):
            notebook.published = query.value(1).toBool();
            notebook.__isset.published = true;
            break;
        case (NOTEBOOK_PUBLISHING_URI):
            notebook.publishing.uri = query.value(1).toString().toStdString();
            notebook.__isset.publishing = true;
            notebook.publishing.__isset.uri = true;
            break;
        case (NOTEBOOK_PUBLISHING_ORDER): {
            qint32 value = query.value(1).toInt();
            notebook.publishing.order = NoteSortOrder::CREATED;
            if (value == NoteSortOrder::UPDATED) notebook.publishing.order = NoteSortOrder::UPDATED;
            if (value == NoteSortOrder::RELEVANCE) notebook.publishing.order = NoteSortOrder::RELEVANCE;
            if (value == NoteSortOrder::UPDATE_SEQUENCE_NUMBER) notebook.publishing.order = NoteSortOrder::UPDATE_SEQUENCE_NUMBER;
            if (value == NoteSortOrder::TITLE) notebook.publishing.order = NoteSortOrder::TITLE;
            notebook.__isset.publishing = true;
            notebook.publishing.__isset.order = true;
            break; }
        case (NOTEBOOK_PUBLISHING_ASCENDING):
            notebook.publishing.ascending = query.value(1).toBool();
            notebook.__isset.publishing = true;
            notebook.publishing.__isset.ascending = true;
            break;
        case (NOTEBOOK_PUBLISHING_DESCRIPTION):
            notebook.publishing.publicDescription = query.value(1).toString().toStdString();
            notebook.__isset.publishing = true;
            notebook.publishing.__isset.publicDescription = true;
            break;
        }
    }

    return true;
}



// Return a notebook given the GUID
bool NotebookTable::get(Notebook &notebook, QString guid) {
    qint32 lid = getLid(guid);
    return get(notebook, lid);
}



// Return a notebook given the GUID as a std::string
bool NotebookTable::get(Notebook &notebook, string guid) {
    qint32 lid = getLid(guid);
    return get(notebook, lid);
}



// Return if a notebook is dirty given its lid
bool NotebookTable::isDirty(qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}


// Determine if a notebook is dirty given a guid
bool NotebookTable::isDirty(QString guid) {
    qint32 lid = getLid(guid);
    return isDirty(lid);
}


// Determine if a notebook is dirty given a guid
bool NotebookTable::isDirty(string guid) {
    QString g(QString::fromStdString(guid));
    return isDirty(g);
}

bool NotebookTable::setDirty(string guid, bool dirty=true) {
    qint32 lid = getLid(guid);
    return setDirty(lid, dirty);
}

bool NotebookTable::setDirty(QString guid, bool dirty=true) {
    qint32 lid = getLid(guid);
    return setDirty(lid, dirty);
}

bool NotebookTable::setDirty(qint32 lid, bool dirty) {
    QSqlQuery query(*db);
    query.prepare("Delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":data", dirty);
    return query.exec();
}

// Does this notebook exist?
bool NotebookTable::exists(qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}


// Determine if a notebook is dirty given a guid
bool NotebookTable::exists(QString guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}


// Determine if a notebook is dirty given a guid
bool NotebookTable::exists(string guid) {
    qint32 lid = getLid(guid);
    return exists(lid);
}



// Get a list of all notebooks
qint32 NotebookTable::getAll(QList<qint32> &books) {
    QSqlQuery query(*db);
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    while (query.next()) {
        books.append(query.value(0).toInt());
    }

    return books.size();
}


// Get all notebooks for a particular stack
qint32 NotebookTable::getStack(QList<qint32> &retval, QString &stack){
    QSqlQuery query(*db);
    query.prepare("select distinct lid from DataStore where key=:key and data=:stack");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":stack", stack);
    query.exec();
    while (query.next()) {
        retval.append(query.value(0).toInt());
    }

    return retval.size();

}



// Get the guid for a particular lid
bool NotebookTable::getGuid(QString &retval, qint32 lid){
    QSqlQuery query(*db);
    query.prepare("select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    while (query.next()) {
        retval = query.value(0).toString();
        return true;
    }

    return false;

}



bool NotebookTable::findGuidByName(QString &retval, QString &guid) {
    QSqlQuery query(*db);
    query.prepare("select data from DataStore where key=:key and lid=(select lid from DataStore where data=:guid and key=:gkey)");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":guid", guid);
    query.bindValue(":gkey", NOTEBOOK_NAME);
    query.exec();
    QLOG_DEBUG() << query.lastError();
    while (query.next()) {
        retval = query.value(0).toString();
        return true;
    }

    return false;
}



// Delete this notebook
void NotebookTable::deleteNotebook(qint32 lid) {
    if (!exists(lid))
        return;

    // First delete all the notes for this notebook and
    // move them to the default notebook
    QList<qint32> notes;
    NoteTable noteTable;
    QString guid;
    getGuid(guid, lid);
    noteTable.findNotesByNotebook(notes, guid);
    for (qint32 i=0; i<notes.size(); i++) {
        noteTable.updateNotebook(notes[i], lid, true);
        noteTable.deleteNote(notes[i], true);
    }

    // Now delete the actual notebook
    Notebook notebook;
    get(notebook, lid);
    if (notebook.__isset.updateSequenceNum && notebook.updateSequenceNum > 0) {
        QSqlQuery query(*db);
        query.prepare("delete from datastore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_IS_DELETED);
        query.exec();

        query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_IS_DELETED);
        query.bindValue(":data", true);
        query.exec();

        setDirty(lid, true);
    } else {
        expunge(lid);
    }
}


bool NotebookTable::isLocal(qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_LOCAL);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}


bool NotebookTable::update(Notebook &notebook, bool isDirty) {
    Notebook oldBook;
    qint32 lid = getLid(notebook.guid);
    get(oldBook, lid);
    if (lid <= 0)
        return false;
    bool local = isLocal(lid);
    expunge(lid);
    add(lid, notebook, isDirty, local);
    // Rename anything in the note list
    if (notebook.name != oldBook.name) {
        QSqlQuery query(*db);
        query.prepare("Update notetable set notebook=:name where notebooklid=:lid");
        query.bindValue(":name", QString::fromStdString(notebook.name));
        query.bindValue(":lid", lid);
        query.exec();
    }
    return true;
}

void NotebookTable::expunge(qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid and key>=3000 and key<3200");
    query.bindValue(":lid", lid);
    query.exec();
}


void NotebookTable::expunge(string guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}


void NotebookTable::expunge(QString guid) {
    int lid = this->getLid(guid);
    this->expunge(lid);
}

void NotebookTable::renameStack(QString oldName, QString newName) {
    QList<qint32> lids;
    findByStack(lids, oldName);
    QSqlQuery query(*db);
    query.prepare("Update Datastore set data=:newname where key=:key and data=:oldname");
    query.bindValue(":newname", newName);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":oldName", oldName);
    query.exec();

    for (qint32 i=0; i<lids.size(); i++)
        setDirty(lids[i], true);
}


void NotebookTable::findByStack(QList<qint32> &lids, QString stackName) {
    QSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":name", stackName);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
}


bool NotebookTable::isDeleted(qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("select lid from DataStore where lid=:lid and key=:key and data=:value");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_DELETED);
    query.bindValue(":value", true);
    query.exec();
    if (query.next()) {
        return true;
    }
    return false;
}


void NotebookTable::getStacks(QStringList &stacks) {
    QSqlQuery query(*db);
    query.prepare("Select distinct data from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
    while (query.next()) {
        stacks.append(query.value(0).toString());
    }
}


bool NotebookTable::isStacked(qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("Select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}


void NotebookTable::removeFromStack(qint32 lid) {
    QSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
}



qint32 NotebookTable::getDefaultNotebookLid() {
    QSqlQuery query(*db);
    query.exec("Select lid from datastore where key=3007 and data='true'");
    if (query.next()) {
        return query.value(0).toInt();
    } else {
        query.exec("Select lid from datastore where key=3001");
        if (query.next()) {
            return query.value(0).toInt();
        }
    }
    return 0;
}

QString NotebookTable::getDefaultNotebookGuid() {
    qint32 lid = getDefaultNotebookLid();
    QString guid;
    this->getGuid(guid, lid);
    return guid;
}


bool NotebookTable::isReadOnly(qint32 notebookLid) {
    if (notebookLid <= 0)
        return true;
    else {
        SharedNotebook sharedNotebook;
        SharedNotebookTable stable;
        bool found = stable.get(sharedNotebook, notebookLid);
        if (found) {
            if (sharedNotebook.privilege == SharedNotebookPrivilegeLevel::READ_NOTEBOOK)
                return true;
            if (sharedNotebook.privilege == SharedNotebookPrivilegeLevel::READ_NOTEBOOK_PLUS_ACTIVITY)
                return true;
        }
    }
    return false;
}



qint32 NotebookTable::getConflictNotebook() {
    QSqlQuery query(*db);
    query.prepare("Select lid from datastore where data like 'Conflict%' and key=:key");
    query.bindValue(":key", NOTEBOOK_NAME);
    query.exec();
    int i=0;
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        if (isLocal(lid))
            return lid;
        i++;
    }

    // If there is no conflict notebook, we create one
    qint32 lid;
    Notebook n;
    if (i>0)
        n.name = "Conflict-" +QString::number(i).toStdString();
    else
        n.name = "Conflict";
    n.__isset.name = true;
    n.updateSequenceNum = 0;
    n.__isset.updateSequenceNum = true;
    return add(0,n,true,true);

}



// Get a count of all new unsequenced notebooks
int NotebookTable::getNewUnsequencedCount() {
    QSqlQuery query(*db);
    query.prepare("Select count(lid) from DataStore where key=:key and data=0 and lid not in (select lid from datastore where key=:localkey and data='true')");
    query.bindValue(":key", NOTEBOOK_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":localKey", NOTEBOOK_IS_LOCAL);
    query.exec();
    while(query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}



// Get all dirty lids
qint32 NotebookTable::getAllDirty(QList<qint32> &lids) {
    QSqlQuery query(*db);
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data='true'");
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    return lids.size();
}



// Update the USN
void NotebookTable::setUpdateSequenceNumber(qint32 lid, qint32 usn) {
    QSqlQuery query(*db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", usn);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_UPDATE_SEQUENCE_NUMBER);
    query.exec();
}

// Linked notebooks are not uploaded, so we reset the dirty flags in case
// they were updated locally
void NotebookTable::resetLinkedNotebooksDirty() {
    QSqlQuery query(*db);
    query.prepare("Delete from datastore where key=:key and lid in (select lid from datastore where key=:linkedkey)");
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":linkedkey", LINKEDNOTEBOOK_GUID);
    query.exec();
}
