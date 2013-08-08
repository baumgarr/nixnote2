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
#include <Q_LONG>
#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include <QSqlDatabase>

using namespace evernote::edam;

class UserTable
{
private:
    void createTable();
    QSqlDatabase *db;

public:
    UserTable(QSqlDatabase *db);
    void updateUser(User &user);
    void updateSyncState(SyncState s);
    qlonglong getLastSyncDate();
    qint32 getLastSyncNumber();
    void updateLastSyncDate(long date);
    void updateLastSyncNumber(qint32 usn);
    void getUser(User &user);
    qlonglong getUploadAmt();
};

#endif // USER_H
