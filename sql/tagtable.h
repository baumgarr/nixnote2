#ifndef TAGTABLE_H
#define TAGTABLE_H

#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>
#include <QList>

#define TAG_GUID                    1000
#define TAG_NAME                    1001
#define TAG_PARENT                  1002
#define TAG_UPDATE_SEQUENCE_NUMBER  1003
#define TAG_ISDIRTY                 1004
#define TAG_ISDELETED               1005

using namespace evernote::edam  ;
using namespace std;

class TagTable
{

private:

public:
    TagTable();                             // Constructor
    qint32 getLid(QString guid);               // given a guid, return the lid
    qint32 getLid(string guid);                // Given a guid, return the lid
    qint32 findByName(string &name);           // Find a tag given a name
    qint32 findByName(QString &name);          // Find a tag given a name
    qint32 findChildren(QList<qint32> &list, QString parentGuid);
    void updateGuid(qint32 lid, Guid &guid);   // Update a tag's guid
    void deleteTag(qint32 lid);                // delete a tag
    void expunge(qint32 lid);               // really delete a tag
    void sync(Tag &tag);                    // Sync a tag with a new record
    void sync(qint32 lid, Tag &tag);           // Sync a tag with a new record
    void add(qint32 lid, Tag &t, bool isDirty); // Add a new tag
    bool get(Tag &tag, qint32 lid);           // Get a tag given a lid
    bool get(Tag &tag, QString guid);      // get a tag given a guid
    bool get(Tag &tag, string guid);       // get a tag given a guid
    bool isDirty(qint32 lid);                  // Check if a tag is dirty
    bool isDirty(QString guid);             // Check if a tag is dirty
    bool isDirty(string guid);              // Check if a tag is dirty
    bool setDirty(string guid, bool dirty);  // set the note dirty flag
    bool setDirty(QString guid, bool dirty); // set the note dirty flag
    bool setDirty(qint32 lid, bool dirty);      // set the note dirty flag
    bool exists(qint32 lid);                   // Does this tag exist?
    bool exists(QString guid);              // Does this tag exist?
    bool exists(string guid);               // Does this tag exist?
    qint32 getAll(QList<qint32> &values);                   // Get all possible tag lids
    bool getGuid(QString &guid, qint32 lid);   // Get a guid for a tag given the lid
    void update(Tag &tag, bool dirty);           // Update an existing tag
    bool isDeleted(qint32 lid);                // is this tag deleted?
};

#endif // TAGTABLE_H
