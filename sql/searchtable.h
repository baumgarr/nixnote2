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

public:
    SearchTable();                          // Constructor
    qint32 getLid(QString guid);            // given a guid, return the lid
    qint32 getLid(string guid);             // Given a guid, return the lid
    qint32 findByName(string &name);           // Find a record given a name
    qint32 findByName(QString &name);          // Find a record given a name
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
    void setDirty(qint32 lid, bool dirty);
    bool isDeleted(qint32 lid);
};

#endif // SEARCHTABLE_H
