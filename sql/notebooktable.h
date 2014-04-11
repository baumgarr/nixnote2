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
    QSqlDatabase *db;

public:
    NotebookTable(QSqlDatabase *db);                             // Constructor
    qint32 getLid(QString guid);            // given a guid, return the lid
    qint32 getLid(string guid);             // Given a guid, return the lid
    qint32 findByName(string &name);           // Find a notebook given a name
    qint32 findByName(QString &name);          // Find a notebook given a name
    void updateGuid(qint32 lid, Guid &guid);    // Update a notebook's guid
    qint32 sync(Notebook &notebook);                    // Sync a notebook with a new record
    qint32 sync(qint32 lid, Notebook &notebook);           // Sync a notebook with a new record
    qint32 add(qint32 lid, Notebook &t, bool isDirty, bool isLocal = false); // Add a new notebook
    bool get(Notebook &notebook, qint32 lid);           // Get a notebook given a lid
    bool get(Notebook &notebook, QString guid);      // get a notebook given a guid
    bool get(Notebook &notebook, string guid);       // get a notebook given a guid
    bool isDirty(qint32 lid);                  // Check if a notebook is dirty
    bool isDirty(QString guid);             // Check if a notebook is dirty
    bool isDirty(string guid);              // Check if a notebook is dirty
    bool setDirty(string guid, bool dirty);  // Set the notebook dirty/clean
    bool setDirty(QString guid, bool dirty); // set the notebook dirty/clean
    bool setDirty(qint32 lid, bool dirty);      // set the notebook dirty/clean
    bool exists(qint32 lid);                   // Does this notebook exist?
    bool exists(QString guid);              // Does this notebook exist?
    bool exists(string guid);               // Does this notebook exist?
    qint32 getAll(QList<qint32> &retVal);                    // Get a list of all notebooks
    qint32 getAllOrderByName(QList<qint32> &retVal);                    // Get a list of all notebooks
    qint32 getStack(QList<qint32> &retval, QString &stack);  // Get all notebooks for a particular stack
    bool getGuid(QString& retval, qint32 lid); // Get a guid for a particular lid
    bool findGuidByName(QString &retval, QString &guid);  // Search for a notebook's guid based upon its name
    void deleteNotebook(qint32 lid);           // mark a notebook for deletion
    void expunge(qint32 lid);          // purge a notebook
    void expunge(string guid);         // purge a notebook
    void expunge(QString guid);        // purge a notebook
    bool isDeleted(qint32 lid);            // is this notebook deleted?
    bool update(Notebook &notebook, bool isDirty);
    bool isLocal(qint32 lid);
    void renameStack(QString oldName, QString newName);
    void findByStack(QList<qint32> &lids, QString stackName);
    void getStacks(QStringList &stacks);
    bool isStacked(qint32 lid);
    void removeFromStack(qint32 lid);
    qint32 getDefaultNotebookLid();
    QString getDefaultNotebookGuid();
    bool isReadOnly(qint32 notebookLid);
    qint32 getConflictNotebook();
    qint32 getAllDirty(QList<qint32> &lids);                // get all dirty notebooks
    void setUpdateSequenceNumber(qint32 lid, qint32 usn);   // set the update sequence number
    void resetLinkedNotebooksDirty();                       // mark all linked notebooks as not-dirty
    int getNewUnsequencedCount();                           // Get a count of all notebooks with USN=0
    qint32 addStub(QString guid);                           // Add a stub for later insertion
    qint32 findByUri(QString uri);
    qint32 findByUri(string uri);
    void getOpenNotebooks(QList<qint32> &lids);
    void getClosedNotebooks(QList<qint32> &lids);
    void openAllNotebooks();
    void closeNotebook(qint32 lid);
    void merge(qint32 source, qint32 target);
};

#endif // NOTEBOOKTABLE_H
