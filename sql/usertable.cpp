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

#include "usertable.h"
#include "nsqlquery.h"
#include "sql/databaseconnection.h"

#define USER_EVERNOTE_ACCOUNT 1   // Unique Evernote account number
#define USER_EMAIL 2                         // User email registered with Evernote
#define USER_NAME 3                           // Name of owner of the account
#define USER_TIMEZONE 4                   // Timezone of the user
#define USER_PRIVILEGE 5                 // User privilige level
#define USER_CREATED 6                     // date the Evernote account was created
#define USER_UPDATED 7                     // Date the account was last updated
#define USER_DELETED 8                     // Date the userid was deleted
#define USER_ACTIVE  9                      // Is the user active?
#define USER_SHARD  10                        // user's shard id
#define USER_USERID 11                          // userid

#define USER_ACCOUNTING_UPLOAD_LIMIT 100
#define USER_ACCOUNTING_UPLOAD_LIMIT_END 101

#define USER_ATTR_BUSINESS_ADDRESS 200
#define USER_ATTR_CLIP_FULL_PAGE 201
#define USER_ATTR_COMMENTS 202
#define USER_ATTR_CUSTOMER_PROFILE_ID 203
#define USER_ATTR_DAILY_EMAIL_LIMIT 204
#define USER_ATTR_DEFAULT_LATITUDE 205
#define USER_ATTR_DEFAULT_LONGITUDE 206
#define USER_ATTR_DEFAULT_LOCATION 207
#define USER_ATTR_GROUPNAME 208
#define USER_ATTR_INCOMING_EMAIL 209
#define USER_ATTR_PREFERED_COUNTRY 210
#define USER_ATTR_TWITTER_ID 211
#define USER_ATTR_TWITTER_USERNAME 212

#define USER_SYNC_UPLOADED 300
#define USER_SYNC_LAST_DATE 301
#define USER_SYNC_LAST_NUMBER 302




#include <QtSql>
#include "global.h"
extern Global global;


// Default constructor
UserTable::UserTable(DatabaseConnection *db)
{
    this->db = db;
    // Check if the table exists.  If not, create it.
    NSqlQuery sql(db);
    db->lockForRead();
    sql.exec("Select * from sqlite_master where type='table' and name='UserTable';");
    if (!sql.next()) {
        db->unlock();
        this->createTable();
    } else
        db->unlock();
}


//* Create the NoteTable table.
void UserTable::createTable() {
    QLOG_TRACE() << "Entering UserTable::userTable()";

    QLOG_DEBUG() << "Creating table UserTable";
    NSqlQuery sql(db);
    db->lockForWrite();
    QString command("Create table UserTable (" +
                  QString("key integer primary key,") +
                  QString("data blob default null collate nocase") +
                  QString(")"));
    if (!sql.exec(command)) {
        QLOG_ERROR() << "Creation of UserTable table failed: " << sql.lastError();
    }
    db->unlock();
}



// Update the database's user record
void UserTable::updateUser(User &user) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from UserTable where key != :lastdate and key != :lastnumber;");
    query.bindValue(":lastdate", USER_SYNC_LAST_DATE);
    query.bindValue(":lastnumber", USER_SYNC_LAST_NUMBER);
    query.exec();

    if (user.id.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_EVERNOTE_ACCOUNT);
        qint32 id = user.id;
        query.bindValue(":data", id);
        query.exec();
    }

    if (user.email.isSet()) {
        QString email = user.email;
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_EMAIL);
        query.bindValue(":data", email);
        query.exec();
    }

    if (user.name.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        QString name = user.name;
        query.bindValue(":key", USER_NAME);
        query.bindValue(":data",name);
        query.exec();
    }

    if (user.timezone.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_TIMEZONE);
        QString tz = user.timezone;
        query.bindValue(":data", tz);
        query.exec();
    }

    if (user.privilege.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_PRIVILEGE);
        int priv = user.privilege;
        query.bindValue(":data", priv);
        query.exec();
    }

    if (user.created.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_CREATED);
        qlonglong created = user.created;
        query.bindValue(":data", created);
        query.exec();
    }

    if (user.updated.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_UPDATED);
        qlonglong updated = user.updated;
        query.bindValue(":data", updated);
        query.exec();
    }

    if (user.deleted.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_DELETED);
        qlonglong deleted = user.deleted;
        query.bindValue(":data",deleted);
        query.exec();
    }

    if (user.active.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ACTIVE);
        bool active = user.active;
        query.bindValue(":data", active);
        query.exec();
    }

    if (user.username.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_USERID);
        QString username = user.username;
        query.bindValue(":data", username);
        query.exec();
    }

    if (user.shardId.isSet()) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_SHARD);
        QString shard = user.shardId;
        query.bindValue(":data", shard);
        query.exec();
    }

    if (user.accounting.isSet()) {
        Accounting accounting = user.accounting;
        if (accounting.uploadLimit.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ACCOUNTING_UPLOAD_LIMIT);
            qint32 limit = accounting.uploadLimit;
            query.bindValue(":data", limit);
            query.exec();
        }
        if (accounting.uploadLimitEnd.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ACCOUNTING_UPLOAD_LIMIT_END);
            qlonglong limit = accounting.uploadLimitEnd;
            query.bindValue(":data",limit);
            query.exec();
        }
    }


    if (user.attributes.isSet()) {
        UserAttributes attributes = user.attributes;
        if (attributes.groupName.isSet()) {
            QString groupName = attributes.groupName;
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_GROUPNAME);
            query.bindValue(":data", groupName);
            query.exec();
        }

        if (attributes.dailyEmailLimit.isSet()) {
            int limit = attributes.dailyEmailLimit;
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_DAILY_EMAIL_LIMIT);
            query.bindValue(":data", limit);
            query.exec();
        }

        if (attributes.preferredCountry.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_PREFERED_COUNTRY);
            QString country = attributes.preferredCountry;
            query.bindValue(":data",country);
            query.exec();
        }

        if (attributes.defaultLongitude.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_DEFAULT_LONGITUDE);
            qlonglong longitude = attributes.defaultLongitude;
            query.bindValue(":data", longitude);
            query.exec();
        }

        if (attributes.defaultLatitude.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_DEFAULT_LATITUDE);
            qlonglong latitude = attributes.defaultLatitude;
            query.bindValue(":data", latitude);
            query.exec();
        }

        if (attributes.businessAddress.isSet()) {
            query.bindValue(":key", USER_ATTR_BUSINESS_ADDRESS);
            QString address = attributes.businessAddress;
            query.bindValue(":data", address);
            query.exec();
        }

        if (attributes.defaultLocationName.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_DEFAULT_LOCATION);
            QString loc = attributes.defaultLocationName;
            query.bindValue(":data", loc);
            query.exec();
        }

        if (attributes.businessAddress.isSet()) {
            QString addr = attributes.businessAddress;
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_BUSINESS_ADDRESS);
            query.bindValue(":data", addr);
            query.exec();
        }

        if (attributes.twitterUserName.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_TWITTER_USERNAME);
            QString username = attributes.twitterUserName;
            query.bindValue(":data", username);
            query.exec();
        }

        if (attributes.twitterId.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_TWITTER_ID);
            QString id = attributes.twitterId;
            query.bindValue(":data", id);
            query.exec();
        }

        if (attributes.incomingEmailAddress.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_INCOMING_EMAIL);
            QString email = attributes.incomingEmailAddress;
            query.bindValue(":data", email);
            query.exec();
        }

        if (attributes.clipFullPage.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_CLIP_FULL_PAGE);
            bool clipFullPage = attributes.clipFullPage;
            query.bindValue(":data", clipFullPage);
            query.exec();
        }

//        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
//        query.bindValue(":key", USER_ATTR_CUSTOMER_PROFILE_ID);
//        query.bindValue(":data", QString::number(user.attributes.customerProfileId));
//        query.exec();

        if (attributes.comments.isSet()) {
            query.prepare("Insert into UserTable (key, data) values (:key, :data);");
            query.bindValue(":key", USER_ATTR_COMMENTS);
            QString comments = attributes.comments;
            query.bindValue(":data", comments);
            query.exec();
        }
    }
    query.finish();
    db->unlock();
}



// Update the database's user record
void UserTable::updateSyncState(SyncState s) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("Delete from UserTable where key=:key1 or key=:key2 or key=:key3;");
    query.bindValue(":key1", USER_SYNC_UPLOADED);
    query.bindValue(":key2", USER_SYNC_LAST_DATE);
    query.bindValue(":key3", USER_SYNC_LAST_NUMBER);
    query.exec();

    query.prepare("Insert into UserTable (key, data) values (:key, :data);");
    if (s.uploaded.isSet()) {
        query.bindValue(":key", USER_SYNC_UPLOADED);
        query.bindValue(":data",qlonglong(s.uploaded));
        if (!query.exec()) {
            QLOG_ERROR() << "Error updating USER_SYNC_UPLOADED : " << query.lastError();
        }
     }

     query.prepare("Insert into UserTable (key, data) values (:key, :data);");
     query.bindValue(":key", USER_SYNC_LAST_DATE);
     query.bindValue(":data", qlonglong(s.currentTime));
     if (!query.exec()) {
         QLOG_ERROR() << "Error updating USER_SYNC_LAST_DATE : " << query.lastError();
     }

     query.prepare("Insert into UserTable (key, data) values (:key, :data);");
     query.bindValue(":key", USER_SYNC_LAST_NUMBER);
     query.bindValue(":data", s.updateCount);
     if (!query.exec()) {
         QLOG_ERROR() << "Error updating USER_SYNC_LAST_NUMBER : " << query.lastError();
     }
     query.finish();
     db->unlock();
}


// Get the last date we synchronized
qlonglong UserTable::getLastSyncDate() {
    NSqlQuery query(db);
    query.prepare("Select data from UserTable where key=:key");
    db->lockForRead();
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.exec();

    if (query.next()) {
        qlonglong value = query.value(0).toLongLong();
        query.finish();
        db->unlock();
        return value;
    }
    query.finish();
    db->unlock();
    return 0;
}


// Get the last sequence number
qint32 UserTable::getLastSyncNumber() {
    qint32 value = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.exec();

    if (query.next()) {
        value = query.value(0).toInt();
        query.finish();
        db->unlock();
        return value;
    }
    query.finish();
    db->unlock();
    return value;
}


// update the last date we synchronized
void UserTable::updateLastSyncDate(long date) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.exec();
    query.prepare("Insert into UserTable (key, data) values (:key, :data)");
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.bindValue(":data", qlonglong(date));
    query.exec();
    query.finish();
    db->unlock();
}


// update the last sequence number
void UserTable::updateLastSyncNumber(qint32 value) {
    NSqlQuery query(db);
    db->lockForWrite();
    query.prepare("delete from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.exec();
    query.prepare("Insert into UserTable (key, data) values (:key, :data)");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.bindValue(":data", value);
    query.exec();
    query.finish();
    db->unlock();
}


// Fetch the user record fro the DB
void UserTable::getUser(User &user) {
    NSqlQuery query(db);
    db->lockForRead();
    query.exec("Select key, data from UserTable;");
    Accounting accounting;
    while (query.next()) {
        if (query.value(0) == USER_EVERNOTE_ACCOUNT) {
            user.id = QVariant(query.value(1)).toInt();
        }
        if (query.value(0) == USER_EMAIL) {
            user.email = QVariant(query.value(1)).toString();
        }
        if (query.value(0) == USER_NAME) {
            user.username = QVariant(query.value(1)).toString();
        }
        if (query.value(0) == USER_TIMEZONE) {
            user.timezone = QVariant(query.value(1)).toString();
        }
        if (query.value(0) == USER_NAME) {
            user.username = QVariant(query.value(1)).toString();
        }
        if (query.value(0) == USER_PRIVILEGE) {
            int priv = QVariant(query.value(1)).toInt();
            user.privilege = PrivilegeLevel::NORMAL;
            if (priv == PrivilegeLevel::ADMIN)
                user.privilege = PrivilegeLevel::ADMIN;
            if (priv == PrivilegeLevel::PREMIUM)
                user.privilege = PrivilegeLevel::PREMIUM;
            if (priv == PrivilegeLevel::MANAGER)
                user.privilege = PrivilegeLevel::MANAGER;
            if (priv == PrivilegeLevel::SUPPORT)
                user.privilege = PrivilegeLevel::SUPPORT;
        }
        if (query.value(0) == USER_CREATED) {
            user.created = QVariant(query.value(1)).toInt();
        }
        if (query.value(0) == USER_UPDATED) {
            user.updated = QVariant(query.value(1)).toInt();
        }
        if (query.value(0) == USER_DELETED) {
            user.deleted = QVariant(query.value(1)).toInt();
        }
        if (query.value(0) == USER_ACTIVE) {
            user.active = QVariant(query.value(1)).toInt();
        }
        if (query.value(0) == USER_SHARD) {
            user.shardId = QVariant(query.value(1)).toString();
        }
        if (query.value(0) == USER_USERID) {
            user.username = QVariant(query.value(1)).toString();
        }
        if (query.value(0) == USER_ACCOUNTING_UPLOAD_LIMIT) {
            accounting.uploadLimit =query.value(1).toLongLong();
            user.accounting = accounting;
        }
        if (query.value(0) == USER_ACCOUNTING_UPLOAD_LIMIT_END) {
            accounting.uploadLimitEnd = query.value(1).toLongLong();
            user.accounting = accounting;
        }
    }
    query.finish();
    db->unlock();
}



// Get the amount of data the user has uploaded
qlonglong UserTable::getUploadAmt() {
    qint32 retval = 0;
    NSqlQuery query(db);
    db->lockForRead();
    query.prepare("Select data from usertable where key=:key");
    query.bindValue(":key", USER_SYNC_UPLOADED);
    query.exec();
    if (query.next())
       retval = query.value(0).toLongLong();
    query.finish();
    db->unlock();
    return retval;
}
