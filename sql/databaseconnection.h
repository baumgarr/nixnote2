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
class DataStore;

// Define the class used to access the table
class DatabaseConnection
{
public:
    QSqlDatabase conn;              // The actual database connection
    ConfigStore *configStore;       // Table used to store program settings
    DataStore *dataStore;           // Table that contains the note data
    bool dbLocked;

    DatabaseConnection(QString connection);           // Generic constructor
    ~DatabaseConnection();          // Destructor
    void lockForRead();
    void lockForWrite();
    void unlock();
};

#endif // DATABASECONNECTION_H


