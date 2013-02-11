#ifndef LINKEDNOTEBOOKTABLE_H
#define LINKEDNOTEBOOKTABLE_H

#define LINKEDNOTEBOOK_SHARE_NAME              8000
#define LINKEDNOTEBOOK_USERNAME                8001
#define LINKEDNOTEBOOK_SHARD_ID                8002
#define LINKEDNOTEBOOK_SHARE_KEY               8003
#define LINKEDNOTEBOOK_URI                     8004
#define LINKEDNOTEBOOK_GUID                    8005
#define LINKEDNOTEBOOK_UPDATE_SEQUENCE_NUMBER  8006
#define LINKEDNOTEBOOK_NOTE_STORE_URL          8007
#define LINKEDNOTEBOOK_WEB_API_URL_PREFIX      8008
#define LINKEDNOTEBOOK_STACK                   8009
#define LINKEDNOTEBOOK_BUSINESS_ID             8010

#define LINKEDNOTEBOOK_IS_DIRTY                8096
#define LINKEDNOTEBOOK_LAST_SEQUENCE_NUMBER    8097
#define LINKEDNOTEBOOK_ALIAS_NAME              8098
#define LINKEDNOTEBOOK_IS_DELETED              8099

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
    void sync(LinkedNotebook &notebook);                    // Sync a notebook with a new record
    void sync(qint32 lid, LinkedNotebook &notebook);           // Sync a notebook with a new record
    void add(qint32 lid, LinkedNotebook &t, bool isDirty); // Add a new notebook
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

signals:
    
public slots:
    
};

#endif // LINKEDNOTEBOOKTABLE_H
