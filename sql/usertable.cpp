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




#include <qt4/QtSql/QtSql>
#include <qt4/QtCore/QVariant>
#include "global.h"
extern Global global;

using namespace evernote::edam;



UserTable::UserTable(QSqlDatabase *db)
{
    this->db = db;
    // Check if the table exists.  If not, create it.
    NSqlQuery sql(*this->db);
    sql.exec("Select * from sqlite_master where type='table' and name='UserTable';");
    if (!sql.next())
        this->createTable();
}


//* Create the NoteTable table.
void UserTable::createTable() {
    QLOG_TRACE() << "Entering UserTable::userTable()";

    QLOG_DEBUG() << "Creating table UserTable";
    NSqlQuery sql(*db);
    QString command("Create table UserTable (" +
                  QString("key integer primary key,") +
                  QString("data blob default null collate nocase") +
                  QString(")"));
    if (!sql.exec(command)) {
        QLOG_ERROR() << "Creation of UserTable table failed: " << sql.lastError();
    }
}



// Update the database's user record
void UserTable::updateUser(User &user) {
    NSqlQuery query(*db);
    query.prepare("delete from UserTable where key != :last_date and key != :last_number;");
    query.bindValue(":key1", USER_SYNC_LAST_DATE);
    query.bindValue(":key2", USER_SYNC_LAST_NUMBER);
    query.exec();

    if (user.__isset.id) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_EVERNOTE_ACCOUNT);
        query.bindValue(":data", QString::number(user.id));
        query.exec();
    }

    if (user.__isset.email) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_EMAIL);
        query.bindValue(":data", QString::fromStdString(user.email));
        query.exec();
    }

    if (user.__isset.name) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_NAME);
        query.bindValue(":data", QString::fromStdString(user.name));
        query.exec();
    }

    if (user.__isset.timezone) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_TIMEZONE);
        query.bindValue(":data", QString::fromStdString(user.timezone));
        query.exec();
    }

    if (user.__isset.privilege) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_PRIVILEGE);
        query.bindValue(":data", QString::number(user.privilege));
        query.exec();
    }

    if (user.__isset.created) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_CREATED);
        query.bindValue(":data", QString::number(user.created));
        query.exec();
    }

    if (user.__isset.updated) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_UPDATED);
        query.bindValue(":data", QString::number(user.updated));
        query.exec();
    }

    if (user.__isset.deleted) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_DELETED);
        query.bindValue(":data", QString::number(user.deleted));
        query.exec();
    }

    if (user.__isset.active) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ACTIVE);
        query.bindValue(":data", QString::number(user.active));
        query.exec();
    }

    if (user.__isset.username) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_USERID);
        query.bindValue(":data", QString::fromStdString(user.username));
        query.exec();
    }

    if (user.__isset.shardId) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_SHARD);
        query.bindValue(":data", QString::fromStdString(user.shardId));
        query.exec();
    }

    if (user.__isset.accounting) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ACCOUNTING_UPLOAD_LIMIT);
        //query.bindValue(":data", QVariant::fromValue(user.accounting.uploadLimit));
        query.bindValue(":data", QString::number(user.accounting.uploadLimit));
        query.exec();
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ACCOUNTING_UPLOAD_LIMIT_END);
        query.bindValue(":data",QString::number(user.accounting.uploadLimitEnd));
        query.exec();
    }


    if (user.__isset.attributes) {
        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_GROUPNAME);
        query.bindValue(":data", QString::fromStdString(user.attributes.groupName));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_DAILY_EMAIL_LIMIT);
        query.bindValue(":data", user.attributes.dailyEmailLimit);
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_PREFERED_COUNTRY);
        query.bindValue(":data", QString::fromStdString(user.attributes.preferredCountry));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_DEFAULT_LONGITUDE);
        query.bindValue(":data", user.attributes.defaultLongitude);
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_DEFAULT_LATITUDE);
        query.bindValue(":data", user.attributes.defaultLatitude);
        query.exec();

        query.bindValue(":key", USER_ATTR_BUSINESS_ADDRESS);
        query.bindValue(":data", QString::fromStdString(user.attributes.businessAddress));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_DEFAULT_LOCATION);
        query.bindValue(":data", QString::fromStdString(user.attributes.defaultLocationName));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_BUSINESS_ADDRESS);
        query.bindValue(":data", QString::fromStdString(user.attributes.businessAddress));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_TWITTER_USERNAME);
        query.bindValue(":data", QString::fromStdString(user.attributes.twitterUserName));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_TWITTER_ID);
        query.bindValue(":data", QString::fromStdString(user.attributes.twitterId));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_INCOMING_EMAIL);
        query.bindValue(":data", QString::fromStdString(user.attributes.incomingEmailAddress));
        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_CLIP_FULL_PAGE);
        query.bindValue(":data", user.attributes.clipFullPage);
        query.exec();

//        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
//        query.bindValue(":key", USER_ATTR_CUSTOMER_PROFILE_ID);
//        query.bindValue(":data", QString::number(user.attributes.customerProfileId));
//        query.exec();

        query.prepare("Insert into UserTable (key, data) values (:key, :data);");
        query.bindValue(":key", USER_ATTR_COMMENTS);
        query.bindValue(":data", QString::fromStdString(user.attributes.comments));
        query.exec();
    }
}



// Update the database's user record
void UserTable::updateSyncState(SyncState s) {
    NSqlQuery query(*db);
    query.prepare("Delete from UserTable where key=:key1 or key=:key2 or key=:key3;");
    query.bindValue(":key1", USER_SYNC_UPLOADED);
    query.bindValue(":key2", USER_SYNC_LAST_DATE);
    query.bindValue(":key3", USER_SYNC_LAST_NUMBER);
    query.exec();

    query.prepare("Insert into UserTable (key, data) values (:key, :data);");
    if (s.__isset.uploaded) {
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

}


// Get the last date we synchronized
qlonglong UserTable::getLastSyncDate() {
    NSqlQuery query(*db);
    query.prepare("Select data from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.exec();

    if (query.next()) {
        qlonglong value = query.value(0).toLongLong();
        return value;
    }
    return 0;
}

// Get the last sequence number
qint32 UserTable::getLastSyncNumber() {
    qint32 value = 0;
    NSqlQuery query(*db);
    query.prepare("Select data from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.exec();

    if (query.next()) {
        value = query.value(0).toInt();
        return value;
    }
    return value;
}


// update the last date we synchronized
void UserTable::updateLastSyncDate(long date) {
    NSqlQuery query(*db);
    query.prepare("delete from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.exec();
    query.prepare("Insert into UserTable (key, data) values (:key, :data)");
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.bindValue(":data", qlonglong(date));
    query.exec();
}

// update the last sequence number
void UserTable::updateLastSyncNumber(qint32 value) {
    NSqlQuery query(*db);
    query.prepare("delete from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.exec();
    query.prepare("Insert into UserTable (key, data) values (:key, :data)");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.bindValue(":data", value);
    query.exec();
}


void UserTable::getUser(User &user) {
    NSqlQuery query(*db);
    query.exec("Select key, data from UserTable;");
    while (query.next()) {
        if (query.value(0) == USER_EVERNOTE_ACCOUNT) {
            user.id = QVariant(query.value(1)).toInt();
            user.__isset.id = true;
        }
        if (query.value(0) == USER_EMAIL) {
            user.email = QVariant(query.value(1)).toString().toStdString();
            user.__isset.email = true;
        }
        if (query.value(0) == USER_NAME) {
            user.username = QVariant(query.value(1)).toString().toStdString();
            user.__isset.username = true;
        }
        if (query.value(0) == USER_TIMEZONE) {
            user.timezone = QVariant(query.value(1)).toString().toStdString();
            user.__isset.timezone = true;
        }
        if (query.value(0) == USER_NAME) {
            user.username = QVariant(query.value(1)).toString().toStdString();
            user.__isset.username = true;
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
            user.__isset.privilege = true;
        }
        if (query.value(0) == USER_CREATED) {
            user.created = QVariant(query.value(1)).toInt();
            user.__isset.created = true;
        }
        if (query.value(0) == USER_UPDATED) {
            user.updated = QVariant(query.value(1)).toInt();
            user.__isset.updated = true;
        }
        if (query.value(0) == USER_DELETED) {
            user.deleted = QVariant(query.value(1)).toInt();
            user.__isset.deleted = true;
        }
        if (query.value(0) == USER_ACTIVE) {
            user.active = QVariant(query.value(1)).toInt();
            user.__isset.active = true;
        }
        if (query.value(0) == USER_SHARD) {
            user.shardId = QVariant(query.value(1)).toString().toStdString();
            user.__isset.shardId = true;
        }
        if (query.value(0) == USER_USERID) {
            user.username = QVariant(query.value(1)).toString().toStdString();
            user.__isset.username = true;
        }
        if (query.value(0) == USER_ACCOUNTING_UPLOAD_LIMIT) {
            user.accounting.uploadLimit = QVariant(query.value(1)).toLongLong();
            user.__isset.accounting = true;
            user.accounting.__isset.uploadLimit = true;
        }
        if (query.value(0) == USER_ACCOUNTING_UPLOAD_LIMIT_END) {
            user.accounting.uploadLimitEnd = QVariant(query.value(1)).toLongLong();
            user.__isset.accounting = true;
            user.accounting.__isset.uploadLimitEnd =true;
        }
    }
}


qlonglong UserTable::getUploadAmt() {
    NSqlQuery query(*db);
    query.prepare("Select data from usertable where key=:key");
    query.bindValue(":key", USER_SYNC_UPLOADED);
    query.exec();
    if (query.next())
        return query.value(0).toLongLong();
    else
        return 0;
}

