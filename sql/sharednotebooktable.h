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

#ifndef SHAREDNOTEBOOKTABLE_H
#define SHAREDNOTEBOOKTABLE_H

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#define SHAREDNOTEBOOK_ID                    3300
#define SHAREDNOTEBOOK_USERID                3301
#define SHAREDNOTEBOOK_NOTEBOOK_GUID         3302
#define SHAREDNOTEBOOK_EMAIL                 3303
#define SHAREDNOTEBOOK_PRIVILEGE             3304
#define SHAREDNOTEBOOK_ALLOW_PREVIEW         3305
#define SHAREDNOTEBOOK_SERVICE_CREATED       3306
#define SHAREDNOTEBOOK_SERVICE_UPDATED       3307
#define SHAREDNOTEBOOK_SHARE_KEY             3308
#define SHAREDNOTEBOOK_USERNAME              3309
#define SHAREDNOTEBOOK_ISDIRTY               3399


using namespace evernote::edam  ;
using namespace std;

class SharedNotebookTable
{

private:
    QSqlDatabase *db;
public:
    SharedNotebookTable(QSqlDatabase *db=NULL);                  // Constructor
    qint32 getLid(qlonglong id);               // given a guid, return the lid
    void sync(SharedNotebook &sharedNotebook); // Sync a notebook with a new record
    void sync(qint32 lid, SharedNotebook sharedNotebook); // Sync a notebook with a new record
    void add(qint32 lid, SharedNotebook &t, bool isDirty); // Add a new record
    bool get(SharedNotebook &notebook, qint32 lid);           // Get a shared notebook given a lid
    bool isDirty(qint32 lid);                  // Check if a shared notebook is dirty
    bool exists(qint32 lid);                   // Does this shared notebook exist?
    bool exists(qlonglong id);              // Does this shared notebook exist?
    qint32 getAll(QList<qint32> &values);                   // Get all possible shared notebook lids
    qlonglong getId(qint32 lid);   // Get a shared notebook for a tag given the lid
    qint32 findById(qlonglong id);  // Find a lid by the id
};


#endif // SHAREDNOTEBOOKTABLE_H
