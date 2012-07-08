#ifndef TAGTABLE_H
#define TAGTABLE_H

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#define TAG_GUID                    1000
#define TAG_NAME                    1001
#define TAG_PARENT                  1002
#define TAG_UPDATE_SEQUENCE_NUMBER  1003
#define TAG_ISDIRTY                 1004

using namespace evernote::edam  ;
using namespace std;

class TagTable
{

private:

public:
    TagTable();                             // Constructor
    int getLid(QString guid);               // given a guid, return the lid
    int getLid(string guid);                // Given a guid, return the lid
    int findByName(string &name);           // Find a tag given a name
    int findByName(QString &name);          // Find a tag given a name
    void updateGuid(int lid, Guid &guid);    // Update a tag's guid
    void sync(Tag &tag);                    // Sync a tag with a new record
    void sync(int lid, Tag &tag);           // Sync a tag with a new record
    void add(int lid, Tag &t, bool isDirty); // Add a new tag
    bool get(Tag &tag, int lid);           // Get a tag given a lid
    bool get(Tag &tag, QString guid);      // get a tag given a guid
    bool get(Tag &tag, string guid);       // get a tag given a guid
    bool isDirty(int lid);                  // Check if a tag is dirty
    bool isDirty(QString guid);             // Check if a tag is dirty
    bool isDirty(string guid);              // Check if a tag is dirty
    bool setDirty(string guid, bool dirty);  // set the note dirty flag
    bool setDirty(QString guid, bool dirty); // set the note dirty flag
    bool setDirty(int lid, bool dirty);      // set the note dirty flag
    bool exists(int lid);                   // Does this tag exist?
    bool exists(QString guid);              // Does this tag exist?
    bool exists(string guid);               // Does this tag exist?
    int getAll(QList<int> &values);                   // Get all possible tag lids
    bool getGuid(QString &guid, int lid);   // Get a guid for a tag given the lid
};

#endif // TAGTABLE_H
