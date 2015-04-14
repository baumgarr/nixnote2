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

#ifndef NOTEBOOKTABLE_H
#define NOTEBOOKTABLE_H

#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#define NOTEBOOK_GUID                       3000
#define NOTEBOOK_NAME                       3001
#define NOTEBOOK_STACK                      3002
#define NOTEBOOK_UPDATE_SEQUENCE_NUMBER     3003
#define NOTEBOOK_ISDIRTY                    3004
#define NOTEBOOK_LID                        3005
#define NOTEBOOK_IS_LOCAL                   3006
#define NOTEBOOK_IS_DEFAULT                 3007
#define NOTEBOOK_SERVICE_CREATED            3008
#define NOTEBOOK_SERVICE_UPDATED            3009
#define NOTEBOOK_ALIAS                      3010
#define NOTEBOOK_IS_CLOSED                  3999

#define NOTEBOOK_PUBLISHED                  3100
#define NOTEBOOK_PUBLISHING_URI             3101
#define NOTEBOOK_PUBLISHING_ORDER           3102
#define NOTEBOOK_PUBLISHING_ASCENDING       3103
#define NOTEBOOK_PUBLISHING_DESCRIPTION     3104
#define NOTEBOOK_IS_DELETED                 3105

using namespace std;

class NotebookTable
{

private:
    DatabaseConnection *db;

public:
    NotebookTable(DatabaseConnection *db);           // Constructor

    // DB Read Functions
    qint32 getLid(QString guid);               // given a guid, return the lid
    qint32 getLid(string guid);                // Given a guid, return the lid
    qint32 findByName(string &name);           // Find a notebook given a name
    qint32 findByName(QString &name);          // Find a notebook given a name
    bool get(Notebook &notebook, qint32 lid);        // Get a notebook given a lid
    bool get(Notebook &notebook, QString guid);      // get a notebook given a guid
    bool get(Notebook &notebook, string guid);       // get a notebook given a guid
    bool isDirty(qint32 lid);                  // Check if a notebook is dirty
    bool isDirty(QString guid);                // Check if a notebook is dirty
    bool isDirty(string guid);                 // Check if a notebook is dirty
    bool exists(qint32 lid);                   // Does this notebook exist?
    bool exists(QString guid);                 // Does this notebook exist?
    bool exists(string guid);                  // Does this notebook exist?
    qint32 getAll(QList<qint32> &retVal);      // Get a list of all notebooks
    qint32 getAllOrderByName(QList<qint32> &retVal);         // Get a list of all notebooks
    qint32 getStack(QList<qint32> &retval, QString &stack);  // Get all notebooks for a particular stack
    bool getGuid(QString& retval, qint32 lid);               // Get a guid for a particular lid
    bool findGuidByName(QString &retval, QString &guid);     // Search for a notebook's guid based upon its name
    bool isDeleted(qint32 lid);                 // is this notebook deleted?
    bool isLocal(qint32 lid);                   // Is this notebook local?
    void findByStack(QList<qint32> &lids, QString stackName);   // Find a notebook by the stack name
    void getStacks(QStringList &stacks);        // Get a list of all stacks
    bool isStacked(qint32 lid);                 // Is this notebook in a stack?
    qint32 getDefaultNotebookLid();             // Get the default notebook LID
    QString getDefaultNotebookGuid();           // Get teh default notebok GUID
    bool isReadOnly(qint32 notebookLid);        // Is this DB read only?
    qint32 getConflictNotebook();               // get the LID of the conflict notebook
    qint32 getAllDirty(QList<qint32> &lids);    // get all dirty notebooks
    int getNewUnsequencedCount();               // Get a count of all notebooks with USN=0
    qint32 findByUri(QString uri);              // Find a notebook by the URI
    qint32 findByUri(string uri);               // Find a notebook by the URI
    void getOpenNotebooks(QList<qint32> &lids); // Get a list of all open notebooks
    void getClosedNotebooks(QList<qint32> &lids);   // Get a list of all closed notebooks

    // DB Write Functions
    void updateGuid(qint32 lid, Guid &guid);    // Update a notebook's guid
    qint32 sync(Notebook &notebook);            // Sync a notebook with a new record
    qint32 sync(qint32 lid, Notebook &notebook);      // Sync a notebook with a new record
    qint32 add(qint32 lid, Notebook &t, bool isDirty, bool isLocal = false);    // Add a new notebook
    void openAllNotebooks();                    // Open all notebooks
    void closeNotebook(qint32 lid);             // Close a notebook
    void merge(qint32 source, qint32 target);   // Merge two notebooks
    qint32 addStub(QString guid);               // Add a stub for later insertion
    void setUpdateSequenceNumber(qint32 lid, qint32 usn);   // set the update sequence number
    bool update(Notebook &notebook, bool isDirty);
    bool setDirty(string guid, bool dirty);     // Set the notebook dirty/clean
    bool setDirty(QString guid, bool dirty);    // set the notebook dirty/clean
    bool setDirty(qint32 lid, bool dirty);      // set the notebook dirty/clean
    void deleteNotebook(qint32 lid);            // mark a notebook for deletion
    void expunge(qint32 lid);                   // purge a notebook
    void expunge(string guid);                  // purge a notebook
    void expunge(QString guid);                 // purge a notebook
    void renameStack(QString oldName, QString newName);    // Rename a stack
    void removeFromStack(qint32 lid);           // Remove from a stack
    void resetLinkedNotebooksDirty();           // mark all linked notebooks as not-dirty

};

#endif // NOTEBOOKTABLE_H
