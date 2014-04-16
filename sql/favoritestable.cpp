/*********************************************************************************
NixNote - An open-source client for the Evernote service.
Copyright (C) 2014 Randy Baumgarte

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



#include "favoritestable.h"
#include "sql/nsqlquery.h"
#include "sql/configstore.h"
#include "sql/tagtable.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include "sql/searchtable.h"

FavoritesTable::FavoritesTable(QSqlDatabase *db, QObject *parent) :
    QObject(parent)
{
    this->db = db;
}



void FavoritesTable::getAll(QList<qint32> &lids) {
    lids.empty();
    NSqlQuery query(*db);
    query.prepare("Select lid from datastore where key=:key");
    query.bindValue(":key", FAVORITES_TARGET);
    query.exec();
    while (query.next()) {
        lids.append(query.value(0).toInt());
    }
}



bool FavoritesTable::get(FavoritesRecord &record, qint32 lid) {
    NSqlQuery query(*db);
    record.parent = 0;
    query.prepare("select key,data from datastore where lid=:lid");
    query.bindValue(":lid", lid);
    query.exec();
    int type = 0;
    bool retval = false;
    while (query.next()) {
        retval = true;
        record.lid = lid;
        int key = query.value(0).toInt();
        switch (key) {
        case FAVORITES_ORDER :
            record.order = query.value(1).toInt();
            break;
        case FAVORITES_TYPE :
            type = query.value(1).toInt();
            break;
        case FAVORITES_TARGET :
            record.target = query.value(1);
            break;
        case FAVORITES_PARENT :
            record.parent = query.value(1).toInt();
        }
    }
    switch (type) {
    case FavoritesRecord::Note :
        record.type = FavoritesRecord::Note;
        break;
    case FavoritesRecord::LocalNotebook :
        record.type = FavoritesRecord::LocalNotebook;
        break;
    case FavoritesRecord::SynchronizedNotebook :
        record.type = FavoritesRecord::SynchronizedNotebook;
        break;
    case FavoritesRecord::ConflictNotebook :
        record.type = FavoritesRecord::ConflictNotebook;
        break;
    case FavoritesRecord::NotebookStack :
        record.type = FavoritesRecord::NotebookStack;
        break;
    case FavoritesRecord::SharedNotebook :
        record.type = FavoritesRecord::SharedNotebook;
        break;
    case FavoritesRecord::LinkedNotebook :
        record.type = FavoritesRecord::LinkedNotebook;
        break;
    case FavoritesRecord::LinkedStack :
        record.type = FavoritesRecord::LinkedStack;
        break;
    case FavoritesRecord::Tag :
        record.type = FavoritesRecord::Tag;
        break;
    case FavoritesRecord::Search :
        record.type = FavoritesRecord::Search;
        break;
    }
    record.displayName = "<missing value>";
    if (record.type == FavoritesRecord::Tag) {
        TagTable tagTable(db);
        Tag t;
        tagTable.get(t, record.target.toInt());
        if (t.name.isSet())
            record.displayName = t.name;
    }
    if (record.type == FavoritesRecord::Note) {
        NoteTable table(db);
        Note r;
        table.get(r, record.target.toInt(), false,false);
        if (r.title.isSet())
            record.displayName = r.title;
    } 
    if (record.type == FavoritesRecord::LocalNotebook||
        record.type == FavoritesRecord::SynchronizedNotebook||
        record.type == FavoritesRecord::ConflictNotebook ||
        record.type == FavoritesRecord::SharedNotebook ||
        record.type == FavoritesRecord::LinkedNotebook) {
        NotebookTable table(db);
        Notebook r;
        table.get(r, record.target.toInt());
        if (r.name.isSet())
            record.displayName = r.name;
    }
    if (record.type == FavoritesRecord::Search) {
        SearchTable table(db);
        SavedSearch r;
        table.get(r, record.target.toInt());
        if (r.name.isSet())
            record.displayName = r.name;
    }
    if (record.type == FavoritesRecord::NotebookStack ||
        record.type == FavoritesRecord::LinkedStack) {
        record.displayName = record.target.toString();
    }
    return retval;
}


void FavoritesTable::setOrder(QList< QPair< qint32, qint32 > > order) {
    NSqlQuery query(*db);
    query.prepare("delete from datastore where key=:key");
    query.bindValue(":key", FAVORITES_ORDER);
    query.exec();

    query.prepare("Insert into datastore (lid, key, data) values (:lid, :key, :value)");
    for (int i=0; i<order.size(); i++) {
        query.bindValue(":lid", order[i].first);
        query.bindValue(":key", FAVORITES_ORDER);
        query.bindValue(":data", order[i].second);
        query.exec();
    }
}


void FavoritesTable::expunge(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("delete from datastore where lid=:key");
    query.bindValue(":lid", lid);
    query.exec();
}


qint32 FavoritesTable::getLidByTarget(const QVariant &target) {
    NSqlQuery query(*db);
    query.prepare("select lid from datastore where key=:key and data=:target");
    query.bindValue(":key", FAVORITES_TARGET);
    query.bindValue(":target", target);
    qint32 result = 0;
    query.exec();
    if (query.next())
        result = query.value(0).toInt();
    return result;
}



qint32 FavoritesTable::add(const FavoritesRecord &record) {
    NSqlQuery query(*db);
    query.prepare("Insert into datastore (lid, key, data) values (:lid, :key, :value)");

    qint32 lid = record.lid;
    ConfigStore cs(db);
    if (lid <= 0)
        lid = cs.incrementLidCounter();
    else
        expunge(lid);
    qint32 tempLid = getLidByTarget(record.target);
    if (tempLid>0)
        expunge(tempLid);

    query.bindValue(":lid", lid);
    query.bindValue(":key", FAVORITES_TYPE);
    query.bindValue(":data", record.type);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", FAVORITES_TARGET);
    query.bindValue(":data", record.target);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", FAVORITES_ORDER);
    query.bindValue(":data", record.order);
    query.exec();

    query.bindValue(":lid", lid);
    query.bindValue(":key", FAVORITES_PARENT);
    query.bindValue(":data", record.parent);
    query.exec();
    return lid;
}



qint32 FavoritesTable::insert(const FavoritesRecord &record) {
    NSqlQuery query(*db);
    NSqlQuery subQuery(*db);
    query.prepare("Select lid, data from datastore where key=:key and lid in (select lid from datastore where key=:parentkey and data=0) order by data");
    query.bindValue(":key", FAVORITES_ORDER);
    query.bindValue(":parentkey", FAVORITES_PARENT);
    query.exec();
    int newOrder = 0;
    while (query.next()) {
        qint32 lid = query.value(0).toInt();
        int order = query.value(1).toInt();
        if(order == record.order)
            newOrder++;
        subQuery.prepare("Update datastore set data=:neworder where lid=:lid and key=:key");
        subQuery.bindValue(":neworder", newOrder);
        subQuery.bindValue(":lid", lid);
        subQuery.bindValue(":key", FAVORITES_ORDER);
        subQuery.exec();
        newOrder++;
    }
    return add(record);
}


bool FavoritesTable::childrenFound(qint32 lid) {
    NSqlQuery query(*db);
    query.prepare("select lid from datastore where key=:key and data=:lid limit 1");
    query.bindValue(":key", FAVORITES_PARENT);
    query.bindValue(":lid", lid);
    query.exec();
    if (query.next())
        return true;
    return false;
}
