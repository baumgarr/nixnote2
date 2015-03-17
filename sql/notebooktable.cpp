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
#include "sql/configstore.h"
#include "sql/notetable.h"
#include "sql/sharednotebooktable.h"
#include "sql/linkednotebooktable.h"
#include "sql/nsqlquery.h"
#include "sql/usertable.h"
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
    this->db = db;
}



// Given a notebook's name as a std::string, we return the lid
qint32 NotebookTable::findByName(string &name) {
    NSqlQuery query(*db);
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
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();

    query.finish();
    return retval;
}




// Given a notebook's name as a QString, we return the lid
qint32 NotebookTable::findByName(QString &name) {
    string n = name.toStdString();
    return findByName(n);
}




// Given a notebook's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void NotebookTable::updateGuid(qint32 lid, Guid &guid) {
    NSqlQuery query(*db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", guid);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    query.finish();
}




// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 NotebookTable::sync(Notebook &notebook) {
    return sync(0, notebook);
}



// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
qint32 NotebookTable::sync(qint32 lid, Notebook &notebook) {
    if (lid == 0) {
        lid = findByName(notebook.name);
        if (lid == 0) {
            SharedNotebookTable stable(db);
            lid = stable.findByNotebookGuid(notebook.guid);
        }
    }
    if (lid > 0) {
        NSqlQuery query(*db);
        NoteTable noteTable(db);
        noteTable.updateNotebookName(lid, notebook.name);

        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid and key>=3000 and key<3200");
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
    } else {
        ConfigStore cs(db);
        lid = cs.incrementLidCounter();
    }

    return add(lid, notebook, false);
}




// Given a notebook's GUID, we return the LID
qint32 NotebookTable::getLid(QString guid) {

    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":data", guid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    if (query.next())
        return query.value(0).toInt();

    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":data", guid);
    query.bindValue(":key", SHAREDNOTEBOOK_NOTEBOOK_GUID);
    query.exec();
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    return retval;
}


// Given a notebook's GUID, we return the LID
qint32 NotebookTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}


// Add a new notebook to the database
qint32 NotebookTable::addStub(QString guid) {
    NSqlQuery query(*db);
    ConfigStore cs(db);
    qint32 lid = cs.incrementLidCounter();
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":data", guid);
    query.exec();
    query.finish();
    return lid;
}

// Add a new notebook to the database
qint32 NotebookTable::add(qint32 l, Notebook &t, bool isDirty, bool isLocal) {

    NSqlQuery query(*db);
    ConfigStore cs(db);

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    qint32 lid = l;
    if (lid == 0) {
        lid = cs.incrementLidCounter();
    } else {
        LinkedNotebookTable ltable(db);
        LinkedNotebook lbook;
        if (ltable.get(lbook, lid)) {
            QString sharename = "";
            if (lbook.shareName.isSet())
                sharename = lbook.shareName;
            if (sharename != "") {
                t.name = lbook.shareName;
            }
        }
    }

    QString guid = "";
    if (t.guid.isSet())
        guid = t.guid;
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":data", guid);
    query.exec();

    QString name = "";
    if (t.name.isSet())
        name = t.name;
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_NAME);
    query.bindValue(":data", name);
    query.exec();

    qint32 usn = 0;
    if (t.updateSequenceNum.isSet())
        usn = t.updateSequenceNum;
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":data", usn);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_LOCAL);
    query.bindValue(":data", isLocal);
    query.exec();

    if (t.defaultNotebook.isSet()) {
        bool defaultNotebook = t.defaultNotebook;
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_IS_DEFAULT);
        query.bindValue(":data", defaultNotebook);
        query.exec();
    }

    if (t.stack.isSet()) {
        QString stack = t.stack;
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_STACK);
        query.bindValue(":data", stack);
        query.exec();
    }

    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":data", isDirty);
    query.exec();

    if (t.serviceCreated.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_SERVICE_CREATED);
        qlonglong created = t.serviceCreated;
        query.bindValue(":data", created);
        query.exec();
    }

    if (t.serviceUpdated.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_SERVICE_UPDATED);
        qlonglong updated = t.serviceUpdated;
        query.bindValue(":data", updated);
        query.exec();
    }

    if (t.published.isSet()) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_PUBLISHED);
        bool published = t.published;
        query.bindValue(":data", published);
        query.exec();
    }

    if (t.publishing.isSet()) {
        Publishing publishing = t.publishing;
        if (publishing.uri.isSet()) {
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_URI);
            QString uri = publishing.uri;
            query.bindValue(":data", uri);
            query.exec();
        }

        if (publishing.order.isSet()) {
            NoteSortOrder::type order = publishing.order;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_ORDER);
            query.bindValue(":data", order);
            query.exec();
        }

        if (publishing.ascending.isSet()) {
            bool ascending = publishing.ascending;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_ASCENDING);
            query.bindValue(":data", ascending);
            query.exec();
        }

        if (publishing.publicDescription.isSet()) {
            QString desc = publishing.publicDescription;
            query.bindValue(":lid", lid);
            query.bindValue(":key", NOTEBOOK_PUBLISHING_DESCRIPTION);
            query.bindValue(":data",desc);
            query.exec();
        }
    }

    if (t.sharedNotebooks.isSet()) {
        SharedNotebookTable sharedTable(db);
        QList<SharedNotebook> sharedNotebooks;
        if (t.sharedNotebooks.isSet())
            sharedNotebooks = t.sharedNotebooks;
        for (int i=0; i<sharedNotebooks.size(); i++) {
            sharedTable.add(lid, sharedNotebooks[i], isDirty);
        }
    }
    query.finish();

    NoteTable noteTable(db);
    noteTable.updateNotebookName(lid, t.name);
    return lid;
}


// Return a notebook structure given the LID
bool NotebookTable::get(Notebook &notebook, qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    Publishing publishing;
    if (query.size() == 0)
        return false;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (NOTEBOOK_GUID):
            notebook.guid = query.value(1).toString();
            break;
        case (NOTEBOOK_UPDATE_SEQUENCE_NUMBER):
            notebook.updateSequenceNum = query.value(1).toInt();
            break;
        case (NOTEBOOK_STACK):
            notebook.stack = query.value(1).toString();
            break;
        case (NOTEBOOK_NAME):
            notebook.name = query.value(1).toString();
            break;
        case (NOTEBOOK_IS_DEFAULT):
            notebook.defaultNotebook = query.value(1).toBool();
            break;
        case (NOTEBOOK_SERVICE_CREATED):
            notebook.serviceCreated = query.value(1).toLongLong();
            break;
        case (NOTEBOOK_SERVICE_UPDATED):
            notebook.serviceUpdated = query.value(1).toLongLong();
            break;
        case (NOTEBOOK_PUBLISHED):
            notebook.published = query.value(1).toBool();
            break;
        case (NOTEBOOK_PUBLISHING_URI):
            if (notebook.publishing.isSet())
                publishing = notebook.publishing;
            publishing.uri = query.value(1).toString();
            notebook.publishing = publishing;
            break;
        case (NOTEBOOK_PUBLISHING_ORDER): {
            qint32 value = query.value(1).toInt();
            if (notebook.publishing.isSet())
                publishing = notebook.publishing;
            publishing.order = NoteSortOrder::CREATED;
            if (value == NoteSortOrder::UPDATED) publishing.order = NoteSortOrder::UPDATED;
            if (value == NoteSortOrder::RELEVANCE) publishing.order = NoteSortOrder::RELEVANCE;
            if (value == NoteSortOrder::UPDATE_SEQUENCE_NUMBER) publishing.order = NoteSortOrder::UPDATE_SEQUENCE_NUMBER;
            if (value == NoteSortOrder::TITLE) publishing.order = NoteSortOrder::TITLE;
            notebook.publishing = publishing;
            break;
        }
        case (NOTEBOOK_PUBLISHING_ASCENDING):
            if (notebook.publishing.isSet())
                publishing = notebook.publishing;
            publishing.ascending = query.value(1).toBool();
            notebook.publishing = publishing;
            break;
        case (NOTEBOOK_PUBLISHING_DESCRIPTION):
            if (notebook.publishing.isSet())
                publishing = notebook.publishing;
            publishing.publicDescription = query.value(1).toString();
            notebook.publishing = publishing;
            break;
        }
    }
    query.finish();
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
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    return retval;
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
    NSqlQuery query(*db);
    query.prepare("Delete from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":lid", lid);
    query.exec();

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":data", dirty);
    bool retval = query.exec();
    query.finish();
    return retval;
}

// Does this notebook exist?
bool NotebookTable::exists(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = true;
    query.finish();
    return retval;
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
    NSqlQuery query(*db);
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    while (query.next()) {
        books.append(query.value(0).toInt());
    }
    query.finish();
    return books.size();
}



// Get a list of all notebooks
qint32 NotebookTable::getAllOrderByName(QList<qint32> &books) {
    NSqlQuery query(*db);
    query.prepare("select distinct lid from DataStore where key=:key order by data");
    query.bindValue(":key", NOTEBOOK_NAME);
    query.exec();
    while (query.next()) {
        books.append(query.value(0).toInt());
    }
    query.finish();
    return books.size();
}


// Get all notebooks for a particular stack
qint32 NotebookTable::getStack(QList<qint32> &retval, QString &stack){
    NSqlQuery query(*db);
    query.prepare("select distinct lid from DataStore where key=:key and data=:stack");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":stack", stack);
    query.exec();
    while (query.next()) {
        retval.append(query.value(0).toInt());
    }
    query.finish();
    return retval.size();

}



// Get the guid for a particular lid
bool NotebookTable::getGuid(QString &retval, qint32 lid){
    NSqlQuery query(*db);
    query.prepare("select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":lid", lid);
    query.exec();
    while (query.next()) {
        retval = query.value(0).toString();
        query.finish();
        return true;
    }
    query.finish();
    return false;

}



bool NotebookTable::findGuidByName(QString &retval, QString &guid) {
    NSqlQuery query(*db);
    query.prepare("select data from DataStore where key=:key and lid=(select lid from DataStore where data=:guid and key=:gkey)");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":guid", guid);
    query.bindValue(":gkey", NOTEBOOK_NAME);
    query.exec();
    QLOG_DEBUG() << query.lastError();
    while (query.next()) {
        retval = query.value(0).toString();
        query.finish();
        return true;
    }
    query.finish();
    return false;
}



// Delete this notebook
void NotebookTable::deleteNotebook(qint32 lid) {
    if (!exists(lid))
        return;

    // First delete all the notes for this notebook and
    // move them to the default notebook
    QList<qint32> notes;
    NoteTable noteTable(db);
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
    if (notebook.updateSequenceNum.isSet() && notebook.updateSequenceNum > 0) {
        NSqlQuery query(*db);
        query.prepare("delete from datastore where lid=:lid and key=:key");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_IS_DELETED);
        query.exec();

        query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_IS_DELETED);
        query.bindValue(":data", true);
        query.exec();
        query.finish();

        setDirty(lid, true);
    } else {
        expunge(lid);
    }
}


bool NotebookTable::isLocal(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_LOCAL);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    return retval;
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
    QString oldname = "";
    QString newname = "";
    if (notebook.name.isSet())
        newname = notebook.name;
    if (oldBook.name.isSet())
        oldname = oldBook.name;
    if (newname != oldname) {
        NSqlQuery query(*db);
        query.prepare("Update notetable set notebook=:name where notebooklid=:lid");
        query.bindValue(":name", newname);
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
    }
    return true;
}

void NotebookTable::expunge(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid and key>=3000 and key<3200");
    query.bindValue(":lid", lid);
    query.exec();
    query.finish();
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
    NSqlQuery query(*db);
    query.prepare("Update Datastore set data=:newname where key=:key and data=:oldname");
    query.bindValue(":newname", newName);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":oldName", oldName);
    query.exec();
    query.finish();

    for (qint32 i=0; i<lids.size(); i++)
        setDirty(lids[i], true);
}


void NotebookTable::findByStack(QList<qint32> &lids, QString stackName) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":name", stackName);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
}


bool NotebookTable::isDeleted(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("select lid from DataStore where lid=:lid and key=:key and data=:value");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_DELETED);
    query.bindValue(":value", true);
    query.exec();
    bool retval = false;
    if (query.next()) {
        retval = true;
    }
    query.finish();
    return retval;
}


void NotebookTable::getStacks(QStringList &stacks) {
    NSqlQuery query(*db);
    query.prepare("Select distinct data from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
    while (query.next()) {
        stacks.append(query.value(0).toString());
    }
    query.finish();
}


bool NotebookTable::isStacked(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
    bool retval = false;
    if (query.next())
        retval = true;
    query.finish();
    return retval;
}


void NotebookTable::removeFromStack(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
    query.finish();
}



qint32 NotebookTable::getDefaultNotebookLid() {
    NSqlQuery query(*db);
    qint32 retval = 0;
    query.exec("Select lid from datastore where key=3007 and data='true'");
    if (query.next()) {
        retval = query.value(0).toInt();
    } else {
        query.exec("Select lid from datastore where key=3001");
        if (query.next()) {
            retval = query.value(0).toInt();
        }
    }
    query.finish();
    return retval;
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
        SharedNotebookTable stable(db);   
        bool found = stable.get(sharedNotebook, notebookLid);
        if (found) {
            UserTable userTable(db);
            User user;
            userTable.getUser(user);
            QString username = "";
            QString shareusername = "";
            if (user.username.isSet())
                username = user.username;
            if (sharedNotebook.username.isSet())
                shareusername = sharedNotebook.username;
            if (shareusername == username)
                return false;
            if (sharedNotebook.privilege == SharedNotebookPrivilegeLevel::READ_NOTEBOOK)
                return true;
            if (sharedNotebook.privilege == SharedNotebookPrivilegeLevel::READ_NOTEBOOK_PLUS_ACTIVITY)
                return true;
        } else {
            LinkedNotebookTable ltable(db);
            LinkedNotebook linkedNotebook;
            found = ltable.get(linkedNotebook, notebookLid);
            if (found && linkedNotebook.uri.isSet())
              return true;
        }
    }
    return false;
}



qint32 NotebookTable::getConflictNotebook() {
    NSqlQuery query(*db);
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
    query.finish();

    // If there is no conflict notebook, we create one
    Notebook n;
    if (i>0)
        n.name = "Conflict-" +QString::number(i);
    else
        n.name = "Conflict";
    n.updateSequenceNum = 0;
    return add(0,n,true,true);

}



// Get a count of all new unsequenced notebooks
int NotebookTable::getNewUnsequencedCount() {
    NSqlQuery query(*db);
    query.prepare("Select count(lid) from DataStore where key=:key and data=0 and lid not in (select lid from datastore where key=:localkey and data='true')");
    query.bindValue(":key", NOTEBOOK_UPDATE_SEQUENCE_NUMBER);
    query.bindValue(":localKey", NOTEBOOK_IS_LOCAL);
    query.exec();
    qint32 retval = 0;
    while(query.next()) {
        retval = query.value(0).toInt();
    }
    query.finish();
    return retval;
}



// Get all dirty lids
qint32 NotebookTable::getAllDirty(QList<qint32> &lids) {
    NSqlQuery query(*db);
    lids.clear();
    query.prepare("Select lid from DataStore where key=:key and data='true'");
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
    return lids.size();
}



// Update the USN
void NotebookTable::setUpdateSequenceNumber(qint32 lid, qint32 usn) {
    NSqlQuery query(*db);
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", usn);
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_UPDATE_SEQUENCE_NUMBER);
    query.exec();
    query.finish();
}

// Linked notebooks are not uploaded, so we reset the dirty flags in case
// they were updated locally
void NotebookTable::resetLinkedNotebooksDirty() {
    NSqlQuery query(*db);
    query.prepare("Delete from datastore where key=:key and lid in (select lid from datastore where key=:linkedkey)");
    query.bindValue(":key", NOTEBOOK_ISDIRTY);
    query.bindValue(":linkedkey", LINKEDNOTEBOOK_GUID);
    query.exec();
    query.finish();
}



qint32 NotebookTable::findByUri(QString uri) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":key", NOTEBOOK_PUBLISHING_URI);
    query.bindValue(":data", uri);
    query.exec();
    qint32 retval = 0;
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    return retval;
}

qint32 NotebookTable::findByUri(string uri) {
    return findByUri(QString::fromStdString(uri));
}


// Return a list of all closed notebooks.  Return the
// list of LIDs.
void NotebookTable::getClosedNotebooks(QList<qint32> &lids) {
    lids.empty();
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_IS_CLOSED);
    query.exec();
    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
    query.finish();
}

// Get a list of any notebooks that are open and
// return a list of LIDs.
void NotebookTable::getOpenNotebooks(QList<qint32> &lids) {
    lids.empty();
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and lid not in (select lid from datastore where key=:key2)");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.bindValue(":key2", NOTEBOOK_IS_CLOSED);
    query.exec();
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        lids.append(lid);
    }
    query.finish();
}


// Open all notebooks.  Really we just delete the
// NOTEBOOK_IS_CLOSED record since the default is to be
// open.
void NotebookTable::openAllNotebooks() {
    NSqlQuery query(*db);
    query.prepare("delete from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_IS_CLOSED);
    query.exec();
    query.finish();
}



// Close a specific notebook
void NotebookTable::closeNotebook(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_CLOSED);
    query.exec();

    query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, 'true')");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_IS_CLOSED);
    query.exec();
    query.finish();
}


void NotebookTable::merge(qint32 source, qint32 target) {
    this->expunge(source);
    NSqlQuery query(*db);
    query.prepare("Update datastore set data=:newLid where key=:key and data=:oldLid");
    query.bindValue(":newLid", target);
    query.bindValue(":key", NOTE_NOTEBOOK_LID);
    query.bindValue(":oldLid", source);
    query.exec();
    QLOG_DEBUG() << query.lastError();

    query.prepare("Update notetable set notebooklid=:newLid where notebooklid=:oldLid");
    query.bindValue(":newLid", target);
    query.bindValue(":oldLid", source);
    query.exec();
    QLOG_DEBUG() << query.lastError();

    query.finish();
}

