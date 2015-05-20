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


//*****************************************
// This is a version of QSqlQuery.  The
// main reason to have this is to handle
// the database being locked and to issue
// a retry if it fails.
//*****************************************

#ifndef NSQLQUERY_H
#define NSQLQUERY_H

#include <QtSql>
#include "sql/databaseconnection.h"

using namespace std;

#define DATABASE_LOCKED 5

class NSqlQuery : public QSqlQuery
{
private:
    DatabaseConnection *db;
    int DEBUG_TRIGGER;
public:
    explicit NSqlQuery(DatabaseConnection *db);   // Constructor
    ~NSqlQuery();                          // Destructor
    bool exec();                           // Execute SQL statement
    bool exec(const QString &query);       // Execute SQL statement
    bool exec(const string query);         // Execute SQL statement
    bool exec(const char *query);          // Execute SQL statement
};

#endif // NSQLQUERY_H
