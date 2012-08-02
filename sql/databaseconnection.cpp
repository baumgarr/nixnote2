#include "databaseconnection.h"
#include "global.h"
#include "notetable.h"
#include <QSqlQuery>


extern Global global;
//*****************************************
//* This class is used to connect to the
//* database.
//*****************************************
DatabaseConnection::DatabaseConnection()
{
    QLOG_DEBUG() << "SQL drivers available: " << QSqlDatabase::drivers();
    QLOG_TRACE() << "Adding database SQLITE";
    conn = QSqlDatabase::addDatabase("QSQLITE");
    QLOG_TRACE() << "Setting DB name";
    conn.setDatabaseName(global.fileManager.getDbDirPath("nixnote.db"));
    QLOG_TRACE() << "Opening database";
    conn.open();

    QLOG_TRACE() << "Preparing tables";
    // Start preparing the tables
    configStore = new ConfigStore();
    dataStore = new DataStore(0);

    QSqlQuery tempTable;
//    tempTable.exec("pragma cache_size=8096");
//    tempTable.exec("pragma page_size=8096");
    QLOG_TRACE() << "Creating filter table";
    tempTable.exec("Create table if not exists filter (lid integer)");
    tempTable.exec("delete from filter");
    QLOG_TRACE() << "Adding to filter table";
    tempTable.exec("insert into filter select distinct lid from NoteTable;");
    QLOG_TRACE() << "Addition complete";
    //tempTable.bindValue(":key", NOTE_ACTIVE);
    //tempTable.exec();
}


// Destructor.  Close the database & delete the
// memory used by the valiables.
DatabaseConnection::~DatabaseConnection() {
    conn.close();
    delete configStore;
    delete dataStore;
}



