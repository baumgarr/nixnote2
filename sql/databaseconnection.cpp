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

#include "databaseconnection.h"
#include "global.h"
#include "notetable.h"
#include "sql/nsqlquery.h"
#include "resourcetable.h"

extern Global global;
//*****************************************
//* This class is used to connect to the
//* database.
//*****************************************
DatabaseConnection::DatabaseConnection(QString connection)
{
    dbLocked = Unlocked;
    this->connection = connection;
    QLOG_DEBUG() << "SQL drivers available: " << QSqlDatabase::drivers();
    QLOG_TRACE() << "Adding database SQLITE";
    conn = QSqlDatabase::addDatabase("QSQLITE", connection);
    QLOG_TRACE() << "Setting DB name";
    conn.setDatabaseName(global.fileManager.getDbDirPath("nixnote.db"));
    QLOG_TRACE() << "Opening database";
    if (!conn.open()) {
        QLOG_ERROR() << "Error opening database: " << conn.lastError();
        exit(16);
    }

    if (connection == "nixnote")
        global.db = this;
    QLOG_TRACE() << "Preparing tables";
    // Start preparing the tables
    configStore = new ConfigStore(this);
    dataStore = new DataStore(this);

    NSqlQuery tempTable(this);
//    tempTable.exec("pragma cache_size=8096");
//    tempTable.exec("pragma page_size=8096");
    tempTable.exec("pragma busy_timeout=50000");
    tempTable.exec("pragma journal_mode=wal");

//    tempTable.exec("pragma SQLITE_THREADSAFE=2");
    if (connection == "nixnote") {
        tempTable.exec("pragma COMPILE_OPTIONS");
        QLOG_DEBUG() << "*** SQLITE COMPILE OPTIONS ***";
        while (tempTable.next()) {
            QLOG_DEBUG() << tempTable.value(0).toString();
        }
        QLOG_DEBUG() << "*****************";
    }

    QLOG_TRACE() << "Creating filter table";
    tempTable.exec("Create table if not exists filter (lid integer)");
    tempTable.exec("delete from filter");
    QLOG_TRACE() << "Adding to filter table";
    tempTable.exec("insert into filter select distinct lid from NoteTable;");
    QLOG_TRACE() << "Addition complete";
    tempTable.finish();


}


// Destructor.  Close the database & delete the
// memory used by the valiables.
DatabaseConnection::~DatabaseConnection() {
    conn.close();
    delete configStore;
    delete dataStore;
}


// Lock the database for a read request
void DatabaseConnection::lockForRead() {
    return;
    if (dbLocked)
        return;
    global.dbLock->lockForRead();
    dbLocked = Read;
}


// Lock the database for a read request
void DatabaseConnection::lockForWrite() {
    return;
    if (dbLocked)
        return;
    global.dbLock->lockForWrite();
    dbLocked = Write;
}


// Unlock the database
void DatabaseConnection::unlock() {
    return;
    if (dbLocked == Unlocked)
        return;
    dbLocked = Unlocked;
    global.dbLock->unlock();
}


// Get the database connection name
QString DatabaseConnection::getConnectionName() {
    return connection;
}


