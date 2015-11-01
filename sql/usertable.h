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

#ifndef USERTABLE_H
#define USERTABLE_H

#include <QtGlobal>
#if QT_VERSION < 0x050000
#include <Q_LONG>
#endif
#include <QSqlDatabase>
#include "sql/databaseconnection.h"

#include <qevercloud/include/QEverCloud.h>


using namespace qevercloud;

class UserTable
{
private:
    void createTable();
    DatabaseConnection *db;

public:
    UserTable(DatabaseConnection *db);      // Default constructor

    // DB Read Functions
    qlonglong getLastSyncDate();      // Get the last date the user did a sync
    qint32 getLastSyncNumber();       // Get the last high sequence number
    void getUser(User &user);         // Get the user record
    qlonglong getUploadAmt();         // Get the amount uploaded

    // DB Write Functions
    void updateUser(User &user);            // Save the user record
    void updateSyncState(SyncState s);      // Update the sync state
    void updateLastSyncDate(long date);     // Update the last sync date
    void updateLastSyncNumber(qint32 usn);  // Update the last sync number
};

#endif // USER_H
