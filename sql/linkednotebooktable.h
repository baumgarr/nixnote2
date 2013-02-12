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
#define LINKEDNOTEBOOK_LAST_SEQUENCE_NUMBER    3211

#include <QObject>
#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"

using namespace evernote::edam  ;
using namespace std;

class LinkedNotebookTable : public QObject
{
    Q_OBJECT
public:
    explicit LinkedNotebookTable(QObject *parent = 0);
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
    qint32 getLastUpdateSequenceNumber(qint32 lid);
    void setLastUpdateSequenceNumber(qint32 lid, qint32 lastUSN);

signals:
    
public slots:
    
};

#endif // LINKEDNOTEBOOKTABLE_H
