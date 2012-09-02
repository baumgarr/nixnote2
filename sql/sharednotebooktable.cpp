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
        query.prepare("Delete from DataStore where lid=:lid");
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
        query.bindValue(":data", t.id);
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
        query.bindValue(":key", SHAREDNOTEBOOK_NOTEBOOK_MODIFIABLE);
        query.bindValue(":data", t.notebookModifiable);
        query.exec();
    }
    if (t.__isset.requireLogin) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_REQUIRE_LOGIN);
        query.bindValue(":data", t.requireLogin);
        query.exec();
    }
    if (t.__isset.serviceCreated) {
        query.bindValue(":lid", lid);
        query.bindValue(":key", SHAREDNOTEBOOK_SERVICE_CREATED);
        query.bindValue(":data", t.serviceCreated);
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
    if (query.size() == 0)
        return false;
    while (query.next()) {
        qint32 key = query.value(0).toInt();
        switch (key) {
        case (SHAREDNOTEBOOK_EMAIL):
            notebook.email = query.value(1).toString().toStdString();
            notebook.__isset.email = true;
            break;
        case (SHAREDNOTEBOOK_ISDIRTY):
            break;
        case (SHAREDNOTEBOOK_SERVICE_CREATED):
            notebook.serviceCreated = query.value(1).toLongLong();
            notebook.__isset.serviceCreated = true;
            break;
        case (SHAREDNOTEBOOK_SERVICE_UPDATED):
            break;
        case (SHAREDNOTEBOOK_NOTEBOOK_MODIFIABLE):
            notebook.notebookModifiable = query.value(1).toBool();
            notebook.__isset.notebookModifiable = true;
            break;
        case (SHAREDNOTEBOOK_ID):
            notebook.id = query.value(1).toLongLong();
            notebook.__isset.id = true;
            break;
        case (SHAREDNOTEBOOK_NOTEBOOK_GUID):
            notebook.notebookGuid = query.value(1).toString().toStdString();
            notebook.__isset.notebookGuid = true;
            break;
        case (SHAREDNOTEBOOK_REQUIRE_LOGIN):
            notebook.requireLogin = query.value(1).toBool();
            notebook.__isset.requireLogin = true;
            break;
        case (SHAREDNOTEBOOK_USERID):
            notebook.userId = query.value(1).toInt();
            notebook.__isset.userId = true;
            break;
        case (SHAREDNOTEBOOK_SHARE_KEY):
            notebook.shareKey = query.value(1).toString().toStdString();
            notebook.__isset.shareKey = true;
            break;
        case (SHAREDNOTEBOOK_USERNAME):
            notebook.username = query.value(1).toString().toStdString();
            notebook.__isset.username = true;
            break;
        }
    }
    return true;
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


