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

#ifndef CONFIGSTORE_H
#define CONFIGSTORE_H

#include <QtSql>
#include "sql/databaseconnection.h"

//*************************************
//* This table is used to store
//* config values & things about how
//* to run the program.
//*************************************

// Define key types
#define CONFIG_STORE_LID 0   // This is the highest number object in the database
#define CONFIG_STORE_WINDOW_GEOMETRY 1 // The window geometry between runs
#define CONFIG_STORE_WINDOW_STATE 2 // The window state between runs

class DatabaseConnection;

// Class used to access & update the table
class ConfigStore
{
private:
    void initTable();           // Initialize a new table
    DatabaseConnection *db;           // DB connection

public:
    ConfigStore(DatabaseConnection *conn);  // Generic constructor

    // DB Read Functions
    bool getSetting(QByteArray &value, int key);  // retrieve a setting

    // DB Write Functions
    void createTable();               // SQL to create the table
    qint32 incrementLidCounter();     // Get the next LID number
    void saveSetting(int key, QByteArray);        // Save a setting
};

#endif // CONFIGSTORE_H
