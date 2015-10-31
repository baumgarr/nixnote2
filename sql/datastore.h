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

#ifndef DATASTORETABLE_H
#define DATASTORETABLE_H

#include "global.h"
#include <QSqlTableModel>
#include <QList>
#include "sql/databaseconnection.h"

class DatabaseConnection;

//***********************************************************
// The DataStore is the "main" table which contains multiple
// values.  It consists of a LID which identifies a note,
// a value which is the actual data, and a key which
// identifies what the value actually is (i.e. is it a
// note title, a note body, tag name, notebook GUID...
//***********************************************************
using namespace std;

#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;


class DataStore : public QSqlTableModel
{
    Q_OBJECT
private:
    void createTable();
    DatabaseConnection *db;

public:
    explicit DataStore(DatabaseConnection *db);

signals:

public slots:

};


#endif // DATASTORETABLE_H
