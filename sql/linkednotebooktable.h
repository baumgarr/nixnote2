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


#ifndef LINKEDNOTEBOOKTABLE_H
#define LINKEDNOTEBOOKTABLE_H

#define LINKEDNOTEBOOK_SHARE_NAME              3200
#define LINKEDNOTEBOOK_USERNAME                3201
#define LINKEDNOTEBOOK_SHARD_ID                3202
#define LINKEDNOTEBOOK_SHARE_KEY               3203
#define LINKEDNOTEBOOK_URI                     3204
#define LINKEDNOTEBOOK_GUID                    3205
#define LINKEDNOTEBOOK_UPDATE_SEQUENCE_NUMBER  3206
#define LINKEDNOTEBOOK_NOTE_STORE_URL          3207
#define LINKEDNOTEBOOK_WEB_API_URL_PREFIX      3208
#define LINKEDNOTEBOOK_STACK                   3209
#define LINKEDNOTEBOOK_BUSINESS_ID             3210
#define LINKEDNOTEBOOK_LAST_USN                3211
#define LINKEDNOTEBOOK_IS_DIRTY                3299

#include <QObject>
#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>
#include "global.h"

extern Global global;

using namespace std;

class LinkedNotebookTable : public QObject
{
    Q_OBJECT
public:
    explicit LinkedNotebookTable(QSqlDatabase *db);
    qint32 getLid(QString guid);            // given a guid, return the lid
    qint32 getLid(string guid);             // Given a guid, return the lid
    qint32 findByName(string &name);           // Find a notebook given a name
    qint32 findByName(QString &name);          // Find a notebook given a name
    qint32 sync(LinkedNotebook &notebook);                    // Sync a notebook with a new record
    qint32 sync(qint32 lid, LinkedNotebook &notebook);           // Sync a notebook with a new record
    qint32 add(qint32 lid, LinkedNotebook &t, bool isDirty); // Add a new notebook
    bool get(LinkedNotebook &notebook, qint32 lid);           // Get a notebook given a lid
    bool get(LinkedNotebook &notebook, QString guid);      // get a notebook given a guid
    bool get(LinkedNotebook &notebook, string guid);       // get a notebook given a guid
    qint32 getAll(QList<qint32> &retVal);                    // Get a list of all notebooks
    qint32 getStack(QList<qint32> &retval, QString &stack);  // Get all notebooks for a particular stack
    bool getGuid(QString& retval, qint32 lid); // Get a guid for a particular lid
    bool findGuidByName(QString &retval, QString &guid);  // Search for a notebook's guid based upon its name
    void deleteLinkedNotebook(qint32 lid);           // mark a notebook for deletion
    void expunge(qint32 lid);          // purge a notebook
    void expunge(string guid);         // purge a notebook
    void expunge(QString guid);        // purge a notebook
    bool isDeleted(qint32 lid);            // is this notebook deleted?
    bool update(LinkedNotebook &notebook, bool isDirty);
    void renameStack(QString oldName, QString newName);
    void findByStack(QList<qint32> &lids, QString stackName);
    bool isStacked(qint32 lid);
    void removeFromStack(qint32 lid);
    void getStacks(QStringList &stacks);
    bool exists(qint32 lid);
    qint32 getLastUpdateSequenceNumber(qint32 lid);
    void setLastUpdateSequenceNumber(qint32 lid, qint32 lastUSN);
    QSqlDatabase *db;

signals:
    
public slots:
    
};

#endif // LINKEDNOTEBOOKTABLE_H
