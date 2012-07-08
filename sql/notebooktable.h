#ifndef NOTEBOOKTABLE_H
#define NOTEBOOKTABLE_H

#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
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
#define NOTEBOOK_PUBLISHED                  3100
#define NOTEBOOK_PUBLISHING_URI             3101
#define NOTEBOOK_PUBLISHING_ORDER           3102
#define NOTEBOOK_PUBLISHING_ASCENDING       3103
#define NOTEBOOK_PUBLISHING_DESCRIPTION     3104


using namespace evernote::edam  ;
using namespace std;

class NotebookTable
{

private:

public:
    NotebookTable();                             // Constructor
    int getLid(QString guid);            // given a guid, return the lid
    int getLid(string guid);             // Given a guid, return the lid
    int findByName(string &name);           // Find a notebook given a name
    int findByName(QString &name);          // Find a notebook given a name
    void updateGuid(int lid, Guid &guid);    // Update a notebook's guid
    void sync(Notebook &notebook);                    // Sync a notebook with a new record
    void sync(int lid, Notebook &notebook);           // Sync a notebook with a new record
    void add(int lid, Notebook &t, bool isDirty, bool isLocal = false); // Add a new notebook
    bool get(Notebook &notebook, int lid);           // Get a notebook given a lid
    bool get(Notebook &notebook, QString guid);      // get a notebook given a guid
    bool get(Notebook &notebook, string guid);       // get a notebook given a guid
    bool isDirty(int lid);                  // Check if a notebook is dirty
    bool isDirty(QString guid);             // Check if a notebook is dirty
    bool isDirty(string guid);              // Check if a notebook is dirty
    bool setDirty(string guid, bool dirty);  // Set the notebook dirty/clean
    bool setDirty(QString guid, bool dirty); // set the notebook dirty/clean
    bool setDirty(int lid, bool dirty);      // set the notebook dirty/clean
    bool exists(int lid);                   // Does this notebook exist?
    bool exists(QString guid);              // Does this notebook exist?
    bool exists(string guid);               // Does this notebook exist?
    int getAll(QList<int> &retVal);                    // Get a list of all notebooks
    int getStack(QList<int> &retval, QString &stack);  // Get all notebooks for a particular stack
    bool getGuid(QString& retval, int lid); // Get a guid for a particular lid
    bool findGuidByName(QString &retval, QString &guid);  // Search for a notebook's guid based upon its name
};

#endif // NOTEBOOKTABLE_H
