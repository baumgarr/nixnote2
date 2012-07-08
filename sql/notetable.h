#ifndef NOTETABLE_H
#define NOTETABLE_H



#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"
#include "global.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#define NOTE_GUID                    5000
#define NOTE_TITLE                   5001
#define NOTE_CONTENT                 5002
#define NOTE_UPDATE_SEQUENCE_NUMBER  5003
#define NOTE_ISDIRTY                 5004
#define NOTE_CONTENT_HASH            5005
#define NOTE_CONTENT_LENGTH          5006
#define NOTE_CREATED_DATE            5007
#define NOTE_UPDATED_DATE            5008
#define NOTE_DELETED_DATE            5009
#define NOTE_ACTIVE                  5010
#define NOTE_NOTEBOOK                5011
#define NOTE_TAG                     5012

#define NOTE_ATTRIBUTE_SUBJECT_DATE            5014
#define NOTE_ATTRIBUTE_LATITUDE                5015
#define NOTE_ATTRIBUTE_LONGITUDE               5016
#define NOTE_ATTRIBUTE_ALTITUDE                5017
#define NOTE_ATTRIBUTE_AUTHOR                  5018
#define NOTE_ATTRIBUTE_SOURCE                  5019
#define NOTE_ATTRIBUTE_SOURCE_URL              5020
#define NOTE_ATTRIBUTE_SOURCE_APPLICATION      5021

#define NOTE_HAS_ENCRYPT                       5022
#define NOTE_HAS_TODO_COMPLETED                5023
#define NOTE_HAS_TODO_UNCOMPLETED              5024
#define NOTE_HAS_IMAGE                         5025
#define NOTE_HAS_AUDIO                         5026
#define NOTE_HAS_INK                           5026
#define NOTE_HAS_PDF                           5027
#define NOTE_HAS_ATTACHMENT                    5028

#define NOTE_ATTRIBUTE_SHARE_DATE              5029
#define NOTE_ATTRIBUTE_PLACE_NAME              5030
#define NOTE_ATTRIBUTE_CONTENT_CLASS           5031

#define NOTE_INDEX_NEEDED                      5999


using namespace evernote::edam  ;
using namespace std;

class NoteTable
{

private:
    void rebuildNoteListTags(int lid);

public:
    NoteTable();                             // Constructor
    int getLid(QString guid);            // given a guid, return the lid
    int getLid(string guid);             // Given a guid, return the lid
    void updateGuid(int lid, Guid &guid);    // Update a note's guid
    void sync(Note &note);                    // Sync a note with a new record
    void sync(int lid, Note &note);           // Sync a note with a new record
    void add(int lid, Note &t, bool isDirty); // Add a new note
    bool get(Note &note, int lid, bool loadResources, bool loadResourceRecognition);           // Get a note given a lid
    bool get(Note &note, QString guid, bool loadResources, bool loadResourceRecognition);      // get a note given a guid
    bool get(Note &note, string guid,bool loadResources, bool loadResourceRecognition);       // get a note given a guid
    bool isDirty(int lid);                  // Check if a note is dirty
    bool isDirty(QString guid);             // Check if a note is dirty
    bool isDirty(string guid);              // Check if a note is dirty
    bool exists(int lid);                   // Does this note exist?
    bool exists(QString guid);              // Does this note exist?
    bool exists(string guid);               // Does this note exist?
    int findNotesByTag(QList<int> &values, QString data);
    int findNotesByTag(QList<int> &values, string data);
    void updateNoteListTags(int noteLid, QString tags);
    void updateNoteListNotebooks(QString guid, QString name);
    int getNotesWithTag(QList<int> &retval, QString tag);  // Find all notes for a specific tag;
    void setIndexNeeded(int lid, bool indexNeeded);    // flag if a note needs reindexing
    int getIndexNeeded(QList<int> &lids);           // Get a list of all notes needing indexing
    bool updateNotebookGuid(QString oldGuid, QString newGuid, QString name);  // Update a notebook's name/guid
    bool updateNoteList(int lid, Note &t, bool isDirty);  // Update the user viewing list
    bool updateNotebookName(int lid, QString name);  // Update a notebook's name in the user listing
    void updateNotebook(int noteLid, int notebookLid, bool isDirty);   // Set the current note's notebook
    void setDirty(int lid, bool dirty);
    void updateNotebook(int noteLid, int notebookLid);
    void updateUrl(int lid, QString text, bool dirty);
    void updateTitle(int noteLid, QString title, bool setAsDirty);
    void updateDate(int lid, Timestamp ts, int key, bool isDirty);
    void updateAuthor(int lid, QString value, bool isDirty);
    void removeTag(int noteLid, int tag, bool isDirty);
    void addTag(int lid, int tag, bool isDirty);
};



#endif // NOTETABLE_H
