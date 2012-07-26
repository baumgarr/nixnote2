#include "notebooktable.h"
#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
#include "sql/configstore.h"
#include "sql/notetable.h"
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>


// Default constructor
NotebookTable::NotebookTable()
{
}



// Given a notebook's name as a std::string, we return the lid
int NotebookTable::findByName(string &name) {
    QLOG_TRACE() << "Entering NotebookTable::findByName()";
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", NOTEBOOK_NAME);
    query.bindValue(":name", QString::fromStdString(name));
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}




// Given a notebook's name as a QString, we return the lid
int NotebookTable::findByName(QString &name) {
    string n = name.toStdString();
    return findByName(n);
}




// Given a notebook's lid, we give it a new guid.  This can happen
// the first time a record is synchronized
void NotebookTable::updateGuid(int lid, Guid &guid) {
    QLOG_TRACE() << "Entering NotebookTable::updateGuid()";

    QSqlQuery query;
    query.prepare("Update DataStore set data=:data where key=:key and lid=:lid");
    query.bindValue(":data", QString::fromStdString(guid));
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();

    QLOG_TRACE() << "Leaving NotebookTable::updateGuid()";
}




// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
void NotebookTable::sync(Notebook &notebook) {
    QLOG_TRACE() << "Leaving NotebookTable::sync()";
    sync(0, notebook);
    QLOG_TRACE() << "Leaving NotebookTable::sync()";
}



// Synchronize a new notebook with what is in the database.  We basically
// just delete the old one & give it a new entry
void NotebookTable::sync(int lid, Notebook &notebook) {
    QLOG_TRACE() << "Enteringt NotebookTable::sync()";

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

    add(lid, notebook, false);

    QLOG_TRACE() << "Leaving NotebookTable::sync()";
}




// Given a notebook's GUID, we return the LID
int NotebookTable::getLid(QString guid) {

    QSqlQuery query;
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
int NotebookTable::getLid(string guid) {
    QString s(QString::fromStdString(guid));
    return getLid(s);
}



// Add a new notebook to the database
void NotebookTable::add(int l, Notebook &t, bool isDirty, bool isLocal) {
    QSqlQuery query;
    ConfigStore cs;

    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
    int lid = l;
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
        query.bindValue(":data", t.serviceCreated);
        query.exec();
    }

    if (t.__isset.serviceUpdated) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_SERVICE_UPDATED);
        query.bindValue(":data", t.serviceUpdated);
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
}


// Return a notebook structure given the LID
bool NotebookTable::get(Notebook &notebook, int lid) {
    QSqlQuery query;
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    if (query.size() == 0)
        return false;
    while (query.next()) {
        int key = query.value(0).toInt();
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
            int value = query.value(1).toInt();
            if (value == CREATED) notebook.publishing.order = CREATED;
            if (value == UPDATED) notebook.publishing.order = UPDATED;
            if (value == RELEVANCE) notebook.publishing.order = RELEVANCE;
            if (value == UPDATE_SEQUENCE_NUMBER) notebook.publishing.order = UPDATE_SEQUENCE_NUMBER;
            if (value == TITLE) notebook.publishing.order = TITLE;
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
    int lid = getLid(guid);
    return get(notebook, lid);
}



// Return a notebook given the GUID as a std::string
bool NotebookTable::get(Notebook &notebook, string guid) {
    int lid = getLid(guid);
    return get(notebook, lid);
}



// Return if a notebook is dirty given its lid
bool NotebookTable::isDirty(int lid) {
    QSqlQuery query;
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
    int lid = getLid(guid);
    return isDirty(lid);
}


// Determine if a notebook is dirty given a guid
bool NotebookTable::isDirty(string guid) {
    QString g(QString::fromStdString(guid));
    return isDirty(g);
}

bool NotebookTable::setDirty(string guid, bool dirty=true) {
    int lid = getLid(guid);
    return setDirty(lid, dirty);
}

bool NotebookTable::setDirty(QString guid, bool dirty=true) {
    int lid = getLid(guid);
    return setDirty(lid, dirty);
}

bool NotebookTable::setDirty(int lid, bool dirty) {
    QSqlQuery query;
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
bool NotebookTable::exists(int lid) {
    QSqlQuery query;
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
    int lid = getLid(guid);
    return exists(lid);
}


// Determine if a notebook is dirty given a guid
bool NotebookTable::exists(string guid) {
    int lid = getLid(guid);
    return exists(lid);
}



// Get a list of all notebooks
int NotebookTable::getAll(QList<int> &books) {
    QLOG_TRACE() << "Entering NotebookTable::getAll()";

    QSqlQuery query;
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_GUID);
    query.exec();
    while (query.next()) {
        books.append(query.value(0).toInt());
    }

    return books.size();
}


// Get all notebooks for a particular stack
int NotebookTable::getStack(QList<int> &retval, QString &stack){
    QLOG_TRACE() << "Entering NotebookTable::getStack()";

    QSqlQuery query;
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
bool NotebookTable::getGuid(QString &retval, int lid){
    QLOG_TRACE() << "Entering NotebookTable::getStack()";

    QSqlQuery query;
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
    QLOG_TRACE() << "Entering NotebookTable::findGuidByName()";

    QSqlQuery query;
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



// Delete this tag
void NotebookTable::deleteNotebook(int lid) {
    if (!exists(lid))
        return;

    // First delete all the notes for this notebook
    QList<int> notes;
    NoteTable noteTable;
    QString guid;
    getGuid(guid, lid);
    noteTable.findNotesByNotebook(notes, guid);
    for (int i=0; i<notes.size(); i++) {
        noteTable.deleteNote(notes[i], true);
    }

    // Now delete the actual notebook
    Notebook notebook;
    get(notebook, lid);
    if (notebook.__isset.updateSequenceNum && notebook.updateSequenceNum > 0) {
        QSqlQuery query;
        query.prepare("insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", NOTEBOOK_IS_DELETED);
        query.bindValue(":data", true);
        query.exec();
    } else {
        expunge(lid);
    }
}


bool NotebookTable::isLocal() {
    return false;
}


bool NotebookTable::update(Notebook &notebook, bool isDirty) {
    Notebook oldBook;
    int lid = getLid(notebook.guid);
    get(oldBook, lid);
    if (lid <= 0)
        return false;
    expunge(lid);
    add(lid, notebook, isDirty, isLocal());
    // Rename anything in the note list
    if (notebook.name != oldBook.name) {
        QSqlQuery query;
        query.prepare("Update notetable set notebook=:name where notebooklid=:lid");
        query.bindValue(":name", QString::fromStdString(notebook.name));
        query.bindValue(":lid", lid);
        query.exec();
    }
    return true;
}

void NotebookTable::expunge(int lid) {
    QSqlQuery query;
    query.prepare("delete from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
}


void NotebookTable::renameStack(QString oldName, QString newName) {
    QList<int> lids;
    findByStack(lids, oldName);
    QSqlQuery query;
    query.prepare("Update Datastore set data=:newname where key=:key and data=:oldname");
    query.bindValue(":newname", newName);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":oldName", oldName);
    query.exec();

    for (int i=0; i<lids.size(); i++)
        setDirty(lids[i], true);
}


void NotebookTable::findByStack(QList<int> &lids, QString stackName) {
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:name");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.bindValue(":name", stackName);
    query.exec();
    while(query.next()) {
        lids.append(query.value(0).toInt());
    }
}


bool NotebookTable::isDeleted(int lid) {
    QSqlQuery query;
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
    QSqlQuery query;
    query.prepare("Select distinct data from DataStore where key=:key");
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
    while (query.next()) {
        stacks.append(query.value(0).toString());
    }
}


bool NotebookTable::isStacked(int lid) {
    QSqlQuery query;
    query.prepare("Select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}


void NotebookTable::removeFromStack(int lid) {
    QSqlQuery query;
    query.prepare("delete from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", NOTEBOOK_STACK);
    query.exec();
}
