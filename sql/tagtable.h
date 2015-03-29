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

#ifndef TAGTABLE_H
#define TAGTABLE_H

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
#define TAG_PARENT_LID              1002
#define TAG_UPDATE_SEQUENCE_NUMBER  1003
#define TAG_ISDIRTY                 1004
#define TAG_ISDELETED               1005
#define TAG_OWNING_ACCOUNT          1006

using namespace std;

class TagTable
{

private:
    QSqlDatabase *db;
public:
    TagTable(QSqlDatabase *db);                             // Constructor
    qint32 getLid(QString guid);               // given a guid, return the lid
    qint32 getLid(string guid);                // Given a guid, return the lid
    qint32 findByName(string &name, qint32 account);           // Find a tag given a name
    qint32 findByName(QString &name, qint32 account);          // Find a tag given a name
    qint32 findChildren(QList<qint32> &list, QString parentGuid);
    void updateGuid(qint32 lid, Guid &guid);   // Update a tag's guid
    void deleteTag(qint32 lid);                // delete a tag
    void expunge(qint32 lid);               // really delete a tag
    void expunge(QString guid);             // really delete a tag
    void expunge(string guid);              // really delete a tag
    qint32 sync(Tag &tag, qint32 account);                    // Sync a tag with a new record
    qint32 sync(qint32 lid, Tag &tag, qint32 account);           // Sync a tag with a new record
    qint32 add(qint32 lid, Tag &t, bool isDirty, qint32 account); // Add a new tag
    bool get(Tag &tag, qint32 lid);           // Get a tag given a lid
    bool get(Tag &tag, QString guid);      // get a tag given a guid
    bool get(Tag &tag, string guid);       // get a tag given a guid
    bool isDirty(qint32 lid);                  // Check if a tag is dirty
    bool isDirty(QString guid);             // Check if a tag is dirty
    bool isDirty(string guid);              // Check if a tag is dirty
    void setDirty(string guid, bool dirty);  // set the note dirty flag
    void setDirty(QString guid, bool dirty); // set the note dirty flag
    void setDirty(qint32 lid, bool dirty);      // set the note dirty flag
    bool exists(qint32 lid);                   // Does this tag exist?
    bool exists(QString guid);              // Does this tag exist?
    bool exists(string guid);               // Does this tag exist?
    qint32 getAll(QList<qint32> &values);                   // Get all possible tag lids
    bool getGuid(QString &guid, qint32 lid);   // Get a guid for a tag given the lid
    void update(Tag &tag, bool dirty);           // Update an existing tag
    bool isDeleted(qint32 lid);                // is this tag deleted?
    bool isLinked(qint32 lid);                 // is this a linked tag?
    qint32 owningAccount(qint32 lid);          // Get the owning account for a tag
    qint32 getAllDirty(QList<qint32> &lids);   // get all dirty tags
    void setUpdateSequenceNumber(qint32 lid, qint32 usn);  // set the update sequence number
    void resetLinkedTagsDirty();                // mark all linked tags as not-dirty
    int getNewUnsequencedCount();
    void cleanupLinkedTags();
    void getGuidMap(QHash<QString, QString> &nameMap);
    void getNameMap(QHash<QString,QString> &nameMap);
    void findMissingParents(QList<qint32> &lids);
    void cleanupMissingParents();
};

#endif // TAGTABLE_H
