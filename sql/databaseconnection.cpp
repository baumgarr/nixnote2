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
    conn = QSqlDatabase::addDatabase("QSQLITE");
    conn.setDatabaseName(global.fileManager.getDbDirPath("nixnote.db"));
    conn.open();

    // Start preparing the tables
    configStore = new ConfigStore();
    dataStore = new DataStore(0);

    QSqlQuery tempTable;
    tempTable.exec("Create table if not exists filter (lid integer)");
    tempTable.exec("delete from filter");
    tempTable.exec("insert into filter select lid from NoteTable;");
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



