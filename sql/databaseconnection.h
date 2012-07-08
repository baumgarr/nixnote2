#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include "global.h"
#include "datastore.h"
#include "configstore.h"

#include <QtSql>

//***************************************
//* This class is used to control the
//* database as a whole.
//***************************************

// Forward declare table classes
class NoteTable;
class ConfigStore;


// Define the class used to access the table
class DatabaseConnection
{
public:
    QSqlDatabase conn;              // The actual database connection
    ConfigStore *configStore;       // Table used to store program settings
    DataStore *dataStore;      // Table that contains the note data

    DatabaseConnection();           // Generic constructor
    ~DatabaseConnection();          // Destructor
};

#endif // DATABASECONNECTION_H


