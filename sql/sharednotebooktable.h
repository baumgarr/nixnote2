#ifndef SHAREDNOTEBOOKTABLE_H
#define SHAREDNOTEBOOKTABLE_H

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#define SHAREDNOTEBOOK_ID                    7000
#define SHAREDNOTEBOOK_USERID                7001
#define SHAREDNOTEBOOK_NOTEBOOK_GUID         7002
#define SHAREDNOTEBOOK_EMAIL                 7003
#define SHAREDNOTEBOOK_NOTEBOOK_MODIFIABLE   7004
#define SHAREDNOTEBOOK_REQUIRE_LOGIN         7005
#define SHAREDNOTEBOOK_SERVICE_CREATED       7006
#define SHAREDNOTEBOOK_SERVICE_UPDATED       7007
#define SHAREDNOTEBOOK_SHARE_KEY             7008
#define SHAREDNOTEBOOK_USERNAME              7009
#define SHAREDNOTEBOOK_ISDIRTY               7999


using namespace evernote::edam  ;
using namespace std;

class SharedNotebookTable
{

private:

public:
    SharedNotebookTable();                  // Constructor
    int getLid(qlonglong id);               // given a guid, return the lid
    void sync(SharedNotebook &sharedNotebook); // Sync a notebook with a new record
    void sync(int lid, SharedNotebook sharedNotebook); // Sync a notebook with a new record
    void add(int lid, SharedNotebook &t, bool isDirty); // Add a new record
    bool get(SharedNotebook &notebook, int lid);           // Get a shared notebook given a lid
    bool isDirty(int lid);                  // Check if a shared notebook is dirty
    bool exists(int lid);                   // Does this shared notebook exist?
    bool exists(qlonglong id);              // Does this shared notebook exist?
    int getAll(QList<int> &values);                   // Get all possible shared notebook lids
    qlonglong getId(int lid);   // Get a shared notebook for a tag given the lid
    int findById(qlonglong id);  // Find a lid by the id
};


#endif // SHAREDNOTEBOOKTABLE_H
