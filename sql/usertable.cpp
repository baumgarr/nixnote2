#include "usertable.h"

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

using namespace evernote::edam;



UserTable::UserTable()
{
    // Check if the table exists.  If not, create it.
    QSqlQuery sql;
    sql.exec("Select * from sqlite_master where type='table' and name='UserTable';");
    if (!sql.next())
        this->createTable();
}


//* Create the NoteTable table.
void UserTable::createTable() {
    QLOG_TRACE() << "Entering UserTable::userTable()";

    QLOG_DEBUG() << "Creating table UserTable";
    QSqlQuery sql;
    QString command("Create table UserTable (" +
                  QString("key integer primary key,") +
                  QString("data blob default null collate nocase") +
                  QString(")"));
    if (!sql.exec(command)) {
        QLOG_ERROR() << "Creation of UserTable table failed: " << sql.lastError();
    }
}



// Update the database's user record
void UserTable::updateUser(AuthenticationResult r) {
    QSqlQuery query;
    query.prepare("delete from UserTable where key != :last_date and key != :last_number;");
    query.bindValue(":key1", USER_SYNC_LAST_DATE);
    query.bindValue(":key2", USER_SYNC_LAST_NUMBER);

    query.prepare("Insert into UserTable (key, data) values (:key, :data);");

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_EVERNOTE_ACCOUNT);
        query.bindValue(":data", QString(r.user.id));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_EMAIL);
        query.bindValue(":data", QString::fromStdString(r.user.email));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_NAME);
        query.bindValue(":data", QString::fromStdString(r.user.name));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_TIMEZONE);
        query.bindValue(":data", QString::fromStdString(r.user.timezone));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_PRIVILEGE);
        query.bindValue(":data", QString(r.user.privilege));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_CREATED);
        query.bindValue(":data", QVariant::fromValue(r.user.created));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_UPDATED);
        query.bindValue(":data", QVariant::fromValue(r.user.updated));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_DELETED);
        query.bindValue(":data", QVariant::fromValue(r.user.deleted));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_ACTIVE);
        query.bindValue(":data", QVariant::fromValue(r.user.active));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_PRIVILEGE);
        query.bindValue(":data", QString(r.user.privilege));
        query.exec();
    }

    if (r.user.__isset.id) {
        query.bindValue(":key", USER_SHARD);
        query.bindValue(":data", QString::fromStdString(r.user.shardId));
        query.exec();
    }

    if (r.user.__isset.accounting) {
        query.bindValue(":key", USER_ACCOUNTING_UPLOAD_LIMIT);
        query.bindValue(":data", QVariant::fromValue(r.user.accounting.uploadLimit));
        query.exec();
        query.bindValue(":key", USER_ACCOUNTING_UPLOAD_LIMIT_END);
        query.bindValue(":data", QVariant::fromValue(r.user.accounting.uploadLimitEnd));
        query.exec();
    }


    if (r.user.__isset.attributes) {
        query.bindValue(":key", USER_ATTR_GROUPNAME);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.groupName));
        query.exec();

        query.bindValue(":key", USER_ATTR_DAILY_EMAIL_LIMIT);
        query.bindValue(":data", r.user.attributes.dailyEmailLimit);
        query.exec();

        query.bindValue(":key", USER_ATTR_PREFERED_COUNTRY);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.preferredCountry));
        query.exec();

        query.bindValue(":key", USER_ATTR_DEFAULT_LONGITUDE);
        query.bindValue(":data", r.user.attributes.defaultLongitude);
        query.exec();

        query.bindValue(":key", USER_ATTR_DEFAULT_LATITUDE);
        query.bindValue(":data", r.user.attributes.defaultLatitude);
        query.exec();

        query.bindValue(":key", USER_ATTR_BUSINESS_ADDRESS);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.businessAddress));
        query.exec();

        query.bindValue(":key", USER_ATTR_DEFAULT_LOCATION);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.defaultLocationName));
        query.exec();

        query.bindValue(":key", USER_ATTR_BUSINESS_ADDRESS);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.businessAddress));
        query.exec();

        query.bindValue(":key", USER_ATTR_TWITTER_USERNAME);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.twitterUserName));
        query.exec();

        query.bindValue(":key", USER_ATTR_TWITTER_ID);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.twitterId));
        query.exec();

        query.bindValue(":key", USER_ATTR_INCOMING_EMAIL);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.incomingEmailAddress));
        query.exec();

        query.bindValue(":key", USER_ATTR_CLIP_FULL_PAGE);
        query.bindValue(":data", r.user.attributes.clipFullPage);
        query.exec();

        query.bindValue(":key", USER_ATTR_CUSTOMER_PROFILE_ID);
        query.bindValue(":data", QVariant::fromValue(r.user.attributes.customerProfileId));
        query.exec();

        query.bindValue(":key", USER_ATTR_COMMENTS);
        query.bindValue(":data", QString::fromStdString(r.user.attributes.comments));
        query.exec();

    }

}



// Update the database's user record
void UserTable::updateSyncState(SyncState s) {
    QSqlQuery query;
    query.prepare("Delete from UserTable where key=:key1 or key=:key2 or key=:key3;");
    query.bindValue(":key1", USER_SYNC_UPLOADED);
    query.bindValue(":key2", USER_SYNC_LAST_DATE);
    query.bindValue(":key3", USER_SYNC_LAST_NUMBER);
    query.exec();

    query.prepare("Insert into UserTable (key, data) values (:key, :data);");
    if (s.__isset.uploaded) {
        query.bindValue(":key", USER_SYNC_UPLOADED);
        query.bindValue(":data", QVariant::fromValue(s.uploaded));
        if (!query.exec()) {
            QLOG_ERROR() << "Error updating USER_SYNC_UPLOADED : " << query.lastError();
        }
     }

     query.bindValue(":key", USER_SYNC_LAST_DATE);
     query.bindValue(":data", qlonglong(s.currentTime));
//     query.bindValue(":data", QDateTime::currentDateTime());
     if (!query.exec()) {
         QLOG_ERROR() << "Error updating USER_SYNC_LAST_DATE : " << query.lastError();
     }


     query.bindValue(":key", USER_SYNC_LAST_NUMBER);
     query.bindValue(":data", s.updateCount);
     if (!query.exec()) {
         QLOG_ERROR() << "Error updating USER_SYNC_LAST_NUMBER : " << query.lastError();
     }

}


// Get the last date we synchronized
qlonglong UserTable::getLastSyncDate() {
    QSqlQuery query;
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
int UserTable::getLastSyncNumber() {
    QSqlQuery query;
    query.prepare("Select data from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.exec();

    if (query.next()) {
        int value = query.value(0).toInt();
        return value;
    }
    return 0;
}


// update the last date we synchronized
void UserTable::updateLastSyncDate(long date) {
    QSqlQuery query;
    query.prepare("delete from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.prepare("Insert into UserTable (key, data) values (:data :key)");
    query.bindValue(":key", USER_SYNC_LAST_DATE);
    query.bindValue(":data", qlonglong(date));
    query.exec();
}

// update the last sequence number
void UserTable::updateLastSyncNumber(int value) {
    QSqlQuery query;
    query.prepare("delete from UserTable where key=:key");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.prepare("Insert into UserTable (key, data) values (:data :key)");
    query.bindValue(":key", USER_SYNC_LAST_NUMBER);
    query.bindValue(":data", value);
    query.exec();
}
