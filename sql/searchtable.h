#ifndef SEARCHTABLE_H
#define SEARCHTABLE_H

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
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
    int getLid(QString guid);            // given a guid, return the lid
    int getLid(string guid);             // Given a guid, return the lid
    int findByName(string &name);           // Find a record given a name
    int findByName(QString &name);          // Find a record given a name
    void updateGuid(int lid, Guid &guid);    // Update a record's guid
    void sync(SavedSearch &search);                    // Sync a record
    void sync(int lid, SavedSearch &search);           // Sync a record
    void add(int lid, SavedSearch &t, bool isDirty); // Add a new record
    bool get(SavedSearch &search, int lid);           // Get a record given a lid
    bool get(SavedSearch &search, QString guid);      // get a record given a guid
    bool get(SavedSearch &search, string guid);       // get a record given a guid
    bool isDirty(int lid);                  // Check if a record is dirty
    bool isDirty(QString guid);             // Check if a record is dirty
    bool isDirty(string guid);              // Check if a record is dirty
    bool exists(int lid);                   // Does this record exist?
    bool exists(QString guid);              // Does this record exist?
    bool exists(string guid);               // Does this record exist?
    bool update(int lid, SavedSearch &s, bool isDirty);   // Update an existing saved search
    void deleteSearch(int lid);
    void expunge(int lid);
    void setDirty(int lid, bool dirty);
    bool isDeleted(int lid);
};

#endif // SEARCHTABLE_H
