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

#ifndef SEARCHTABLE_H
#define SEARCHTABLE_H

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

using namespace evernote::edam;
using namespace std;

#define SEARCH_GUID                    2000
#define SEARCH_NAME                    2001
#define SEARCH_UPDATE_SEQUENCE_NUMBER  2003
#define SEARCH_ISDIRTY                 2004
#define SEARCH_FORMAT                  2005
#define SEARCH_QUERY                   2006
#define SEARCH_ISDELETED               2007

#define SEARCH_LID_POSITION            0
#define SEARCH_NAME_POSITION           1

class SearchTable
{

private:
    QSqlDatabase *db;

public:
    SearchTable(QSqlDatabase *db=NULL);                          // Constructor
    qint32 getLid(QString guid);            // given a guid, return the lid
    qint32 getLid(string guid);             // Given a guid, return the lid
    qint32 findByName(string &name);           // Find a record given a name
    qint32 findByName(QString &name);          // Find a record given a name
    qint32 getAllDirty(QList<qint32>&lids);          // get all dirty lids
    void updateGuid(qint32 lid, Guid &guid);    // Update a record's guid
    void sync(SavedSearch &search);                    // Sync a record
    void sync(qint32 lid, SavedSearch &search);           // Sync a record
    void add(qint32 lid, SavedSearch &t, bool isDirty); // Add a new record
    bool get(SavedSearch &search, qint32 lid);           // Get a record given a lid
    bool get(SavedSearch &search, QString guid);      // get a record given a guid
    bool get(SavedSearch &search, string guid);       // get a record given a guid
    bool isDirty(qint32 lid);                  // Check if a record is dirty
    bool isDirty(QString guid);             // Check if a record is dirty
    bool isDirty(string guid);              // Check if a record is dirty
    bool exists(qint32 lid);                   // Does this record exist?
    bool exists(QString guid);              // Does this record exist?
    bool exists(string guid);               // Does this record exist?
    bool update(qint32 lid, SavedSearch &s, bool isDirty);   // Update an existing saved search
    void deleteSearch(qint32 lid);
    void expunge(qint32 lid);
    void expunge(QString guid);
    void expunge(string guid);
    void setDirty(qint32 lid, bool dirty);
    bool isDeleted(qint32 lid);
    void setUpdateSequenceNumber(qint32 lid, qint32 usn);
    string getGuid(qint32 lid);
};

#endif // SEARCHTABLE_H
