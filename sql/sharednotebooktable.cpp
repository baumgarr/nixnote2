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

#include <QSqlTableModel>

SharedNotebookTable::SharedNotebookTable()
{
}


qint32 SharedNotebookTable::getLid(qlonglong id){
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:data");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.bindValue(":data", id);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}


void SharedNotebookTable::sync(SharedNotebook &sharedNotebook){
    qint32 lid = findById(sharedNotebook.id);
    sync(lid, sharedNotebook);
}



void SharedNotebookTable::sync(qint32 l, SharedNotebook sharedNotebook){
    qint32 lid = l;
    if (lid == 0)
        lid= findById(sharedNotebook.id);

    if (lid > 0) {
        QSqlQuery query;
        // Delete the old record
        query.prepare("Delete from DataStore where lid=:lid and key>=3300 and key <3400");
        query.bindValue(":lid", lid);
        query.exec();
    } else {
        ConfigStore cs;
        lid = cs.incrementLidCounter();
    }

    add(lid, sharedNotebook, false);
}



void SharedNotebookTable::add(qint32 l, SharedNotebook &t, bool isDirty){
    isDirty=isDirty;  //suppress unused
    ConfigStore cs;
    qint32 lid = l;
    if (lid == 0)
        lid = cs.incrementLidCounter();

    QSqlQuery query;
    query.prepare("Insert into DataStore (lid, key, data) values (:lid, :key, :data)");

    if (t.__isset.email) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_EMAIL);
        query.bindValue(":data", QString::fromStdString(t.email));
        query.exec();
    }

    if (t.__isset.id) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_ID);
        query.bindValue(":data", QVariant::fromValue(t.id));
        query.exec();
    }

    if (t.__isset.notebookGuid) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_NOTEBOOK_GUID);
        query.bindValue(":data", QString::fromStdString(t.notebookGuid));
        query.exec();
    }

    if (t.__isset.notebookModifiable) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_PRIVILEGE);
        query.bindValue(":data", t.privilege);
        query.exec();
    }
    if (t.__isset.requireLogin) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_ALLOW_PREVIEW);
        query.bindValue(":data", t.allowPreview);
        query.exec();
    }
    if (t.__isset.serviceCreated) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_SERVICE_CREATED);
        query.bindValue(":data", QVariant::fromValue(t.serviceCreated));
        query.exec();
    }
    if (t.__isset.shareKey) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_SHARE_KEY);
        query.bindValue(":data", QString::fromStdString(t.shareKey));
        query.exec();
    }
    if (t.__isset.userId) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_USERID);
        query.bindValue(":data", t.userId);
        query.exec();
    }
    if (t.__isset.username) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_USERNAME);
        query.bindValue(":data", QString::fromStdString(t.username));
        query.exec();
    }
}



bool SharedNotebookTable::get(SharedNotebook &notebook, qint32 lid){
    QSqlQuery query;
    query.prepare("Select key, data from DataStore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    bool returnVal = false;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (SHAREDNOTEBOOK_EMAIL):
            notebook.email = query.value(1).toString().toStdString();
            notebook.__isset.email = true;
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_ISDIRTY):
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_SERVICE_CREATED):
            notebook.serviceCreated = query.value(1).toLongLong();
            notebook.__isset.serviceCreated = true;
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_SERVICE_UPDATED):
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_ALLOW_PREVIEW):
            notebook.allowPreview = query.value(1).toBool();
            notebook.__isset.allowPreview = true;
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_ID):
            notebook.id = query.value(1).toLongLong();
            notebook.__isset.id = true;
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_NOTEBOOK_GUID):
            notebook.notebookGuid = query.value(1).toString().toStdString();
            notebook.__isset.notebookGuid = true;
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
            notebook.__isset.privilege = true;
            returnVal = true;
            break;
        }
        case (SHAREDNOTEBOOK_USERID):
            notebook.userId = query.value(1).toInt();
            notebook.__isset.userId = true;
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_SHARE_KEY):
            notebook.shareKey = query.value(1).toString().toStdString();
            notebook.__isset.shareKey = true;
            returnVal = true;
            break;
        case (SHAREDNOTEBOOK_USERNAME):
            notebook.username = query.value(1).toString().toStdString();
            notebook.__isset.username = true;
            returnVal =true;
            break;
        }
    }
    return returnVal;
}




bool SharedNotebookTable::isDirty(qint32 lid){
    QSqlQuery query;
    query.prepare("Select data from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SHAREDNOTEBOOK_ISDIRTY);
    query.exec();
    if (query.next())
        return query.value(0).toBool();
    else
        return false;
}




bool SharedNotebookTable::exists(qint32 lid){
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and lid=:lid");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}



bool SharedNotebookTable::exists(qlonglong id){
    QSqlQuery query;
    query.prepare("Select lid from DataStore where key=:key and data=:id");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.bindValue(":id", id);
    query.exec();
    if (query.next())
        return true;
    else
        return false;
}



qint32 SharedNotebookTable::getAll(QList<qint32> &values){
    QLOG_TRACE() << "Entering SharedNotebookTable::getAll()";

    QSqlQuery query;
    query.prepare("select distinct lid from DataStore where key=:key");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.exec();
    while (query.next()) {
        values.append(query.value(0).toInt());
    }

    return  values.size();
}



qlonglong SharedNotebookTable::getId(qint32 lid){
    QSqlQuery query;
    query.prepare("Select data from DataStore where lid=:lid and key=:key");
    query.bindValue(":lid", lid);
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.exec();
    while (query.next()) {
        return query.value(0).toLongLong();
    }

    return 0;
}


qint32 SharedNotebookTable::findById(qlonglong id) {
    QSqlQuery query;
    query.prepare("Select data from DataStore where key=:key and data=:id");
    query.bindValue(":key", SHAREDNOTEBOOK_ID);
    query.bindValue(":data", id);
    query.exec();
    while (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
