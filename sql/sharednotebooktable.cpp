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

#include "sharednotebooktable.h"
#include "configstore.h"
#include "global.h"
#include "sql/notebooktable.h"
#include "sql/nsqlquery.h"

#include <QSqlTableModel>
extern Global global;

SharedNotebookTable::SharedNotebookTable(QSqlDatabase *db)
{
    this->db = db;
}


// Given the notebook ID, return the LID
qint32 SharedNotebookTable::getLid(qlonglong id){
    qint32 retval = 0;
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.bindValue(":data", id);
    query.exec();
    if (query.next())
        retval = query.value(0).toInt();
    query.finish();
    return retval;
}


// Synchronize a shared notebook
qint32 SharedNotebookTable::sync(SharedNotebook &sharedNotebook){
    return sync(0, sharedNotebook);
}


// Synchronize a shared notebook
qint32 SharedNotebookTable::sync(qint32 l, SharedNotebook sharedNotebook){
    qint32 lid = l;
    if (lid == 0)
        lid= findById(sharedNotebook.id);
    if (lid == 0)
       lid = findByNotebookGuid(sharedNotebook.notebookGuid);

   if (lid > 0) {
        NSqlQuery query(*db);
        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid and key>=3300 and key <3400");
        query.bindValue(":lid", lid);
        query.exec();
        query.finish();
    } else {
       ConfigStore cs(db);
       lid = cs.incrementLidCounter();
    }

    return add(lid, sharedNotebook, false);
}


// Add a new shared notebook
qint32 SharedNotebookTable::add(qint32 l, const SharedNotebook &t, bool isDirty){
    Q_UNUSED(isDirty);  //suppress unused
    ConfigStore cs(db);
    qint32 lid = l;
    if (lid == 0)
        lid = cs.incrementLidCounter();

    NSqlQuery query(*db);

    if (t.email.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_EMAIL);
        QString email = t.email;
        query.bindValue(":data",email);
        query.exec();
    }

    if (t.id.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_ID);
        qint64 id = t.id;
        query.bindValue(":data", id);
        query.exec();
    }

    if (t.notebookGuid.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_NOTEBOOK_GUID);
        QString notebookGuid = t.notebookGuid;
        query.bindValue(":data", notebookGuid);
        query.exec();
    }

    if (t.notebookModifiable.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_MODIFIABLE);
        bool mod = t.notebookModifiable;
        query.bindValue(":data", mod);
        query.exec();
    }
    if (t.privilege.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_PRIVILEGE);
        qint32 priv = t.privilege;
        query.bindValue(":data", priv);
        query.exec();
    }
    if (t.requireLogin.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_REQUIRE_LOGIN);
        bool login = t.requireLogin;
        query.bindValue(":data", login);
        query.exec();
    }
    if (t.allowPreview.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_ALLOW_PREVIEW);
        bool preview = t.allowPreview;
        query.bindValue(":data", preview);
        query.exec();
    }
    if (t.serviceCreated.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_SERVICE_CREATED);
        qlonglong date = t.serviceCreated;
        query.bindValue(":data", date);
        query.exec();
    }
    if (t.shareKey.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_SHARE_KEY);
        QString key = t.shareKey;
        query.bindValue(":data", key);
        query.exec();
    }
    if (t.userId.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_USERID);
        int userid = t.userId;
        query.bindValue(":data", userid);
        query.exec();
    }
    if (t.username.isSet()) {
        query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_USERNAME);
        QString username = t.username;
        query.bindValue(":data", username);
        query.exec();
    }
    query.finish();
    return lid;
}




// given a LID, return a shared notebook record
bool SharedNotebookTable::get(SharedNotebook &notebook, qint32 lid){
    NSqlQuery query(*db);
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    bool returnVal = false;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (SHAREDNOTEBOOK_EMAIL):
            notebook.email = query.value(1).toString();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_ISDIRTY):
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_SERVICE_CREATED):
            notebook.serviceCreated = query.value(1).toLongLong();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_SERVICE_UPDATED):
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_ALLOW_PREVIEW):
            notebook.allowPreview = query.value(1).toBool();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_ID):
            notebook.id = query.value(1).toLongLong();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_NOTEBOOK_GUID):
            notebook.notebookGuid = query.value(1).toString();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_MODIFIABLE):
            notebook.notebookModifiable = query.value(1).toBool();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_REQUIRE_LOGIN):
            notebook.requireLogin = query.value(1).toBool();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_PRIVILEGE): {
            int priv = query.value(1).toInt();
            notebook.privilege = SharedNotebookPrivilegeLevel::READ_NOTEBOOK;
            if (priv == SharedNotebookPrivilegeLevel::FULL_ACCESS)
                notebook.privilege = SharedNotebookPrivilegeLevel::FULL_ACCESS;
            if (priv == SharedNotebookPrivilegeLevel::BUSINESS_FULL_ACCESS)
                notebook.privilege = SharedNotebookPrivilegeLevel::BUSINESS_FULL_ACCESS;
            if (priv == SharedNotebookPrivilegeLevel::GROUP)
                notebook.privilege = SharedNotebookPrivilegeLevel::GROUP;
            if (priv == SharedNotebookPrivilegeLevel::MODIFY_NOTEBOOK_PLUS_ACTIVITY)
                notebook.privilege = SharedNotebookPrivilegeLevel::MODIFY_NOTEBOOK_PLUS_ACTIVITY;
            if (priv == SharedNotebookPrivilegeLevel::READ_NOTEBOOK_PLUS_ACTIVITY)
                notebook.privilege = SharedNotebookPrivilegeLevel::READ_NOTEBOOK_PLUS_ACTIVITY;
            returnVal = true;
            break;
        }
        case (SHAREDNOTEBOOK_USERID):
            notebook.userId = query.value(1).toInt();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_SHARE_KEY):
            notebook.shareKey = query.value(1).toString();
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_USERNAME):
            notebook.username = query.value(1).toString();
            returnVal =true;
            break;
        }
    }
    query.finish();
    return returnVal;
}



// Does this shared notebook need a sync?
bool SharedNotebookTable::isDirty(qint32 lid){
    bool retval = false;
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SHAREDNOTEBOOK_ISDIRTY);
    query.exec();
    if (query.next())
        retval = query.value(0).toBool();
    query.finish();
    return retval;
}



// Does this shared notebook exist?
bool SharedNotebookTable::exists(qint32 lid){
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.exec();
    if (query.next()) {
        query.finish();
        return true;
    }
    query.finish();
    return false;
}


// Does this shared notebook exist?
bool SharedNotebookTable::exists(qlonglong id){
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:id");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.bindValue(":id", id);
    query.exec();
    if (query.next()) {
        query.finish();
        return true;
    }
    query.finish();
    return false;
}


// Get a list of all shared notebooks
qint32 SharedNotebookTable::getAll(QList<qint32> &values){
    QLOG_TRACE() << "Entering SharedNotebookTable::getAll()";

    NSqlQuery query(*db);
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.exec();
    while (query.next()) {
        values.append(query.value(0).toInt());
    }
    query.finish();
    return  values.size();
}


// Get the shared notebook ID when given the LID
qlonglong SharedNotebookTable::getId(qint32 lid){
    NSqlQuery query(*db);
    query.prepare("Select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.exec();
    while (query.next()) {
        qint32 retval = 0;
        retval =  query.value(0).toLongLong();
        query.finish();
        return retval;
    }
    query.finish();
    return 0;
}



// Find a shared notebook LID by it's ID
qint32 SharedNotebookTable::findById(qlonglong id) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:id");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.bindValue(":data", id);
    query.exec();
    if (query.next()) {
        qint32 retval = 0;
        retval = query.value(0).toInt();
        query.finish();
        return retval;
    }
    query.finish();
    return 0;
}


// Find a shared notebook by the share key
qint32 SharedNotebookTable::findByShareKey(QString id) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:id");
    query.bindValue(":key", SHAREDNOTEBOOK_SHARE_KEY);
    query.bindValue(":data", id);
    query.exec();
    while (query.next()) {
        qint32 retval = query.value(0).toInt();
        query.finish();
        return retval;
    }
    query.finish();
    return 0;
}


// Find a shared notebook by the share key
qint32 SharedNotebookTable::findByShareKey(string id) {
    return findByShareKey(QString::fromStdString(id));
}



// Find a shared notebook LID by its GUID
qint32 SharedNotebookTable::findByNotebookGuid(QString id) {
    NSqlQuery query(*db);
    query.prepare("Select lid from DataStore where key=:key and data=:id");
    query.bindValue(":key", SHAREDNOTEBOOK_NOTEBOOK_GUID);
    query.bindValue(":data", id);
    query.exec();
    while (query.next()) {
        qint32 retval = query.value(0).toInt();
        query.finish();
        return retval;
    }
    query.finish();
    NotebookTable ntable(db);
    return ntable.getLid(id);
}



// Find a shared notebook by the GUID
qint32 SharedNotebookTable::findByNotebookGuid(string id) {
    return findByNotebookGuid(QString::fromStdString(id));
}
