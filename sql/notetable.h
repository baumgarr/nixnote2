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

#ifndef NOTETABLE_H
#define NOTETABLE_H



#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

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
#define NOTE_NOTEBOOK_LID            5011
#define NOTE_TAG_LID                 5012

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

#define NOTE_EXPUNGED_FROM_TRASH               5998
#define NOTE_INDEX_NEEDED                      5999


using namespace evernote::edam  ;
using namespace std;

class NoteTable
{

private:

public:
    NoteTable();                             // Constructor
    qint32 getLid(QString guid);            // given a guid, return the lid
    qint32 getLid(string guid);             // Given a guid, return the lid
    QString getGuid(int lid);               // given a lid, get the guid
    void updateGuid(qint32 lid, Guid &guid);    // Update a note's guid
    void sync(Note &note);                    // Sync a note with a new record
    void sync(qint32 lid, Note &note);           // Sync a note with a new record
    qint32 add(qint32 lid, Note &t, bool isDirty); // Add a new note
    bool get(Note &note, qint32 lid, bool loadResources, bool loadResourceRecognition);           // Get a note given a lid
    bool get(Note &note, QString guid, bool loadResources, bool loadResourceRecognition);      // get a note given a guid
    bool get(Note &note, string guid,bool loadResources, bool loadResourceRecognition);       // get a note given a guid
    bool isDirty(qint32 lid);                  // Check if a note is dirty
    bool isDirty(QString guid);             // Check if a note is dirty
    bool isDirty(string guid);              // Check if a note is dirty
    bool exists(qint32 lid);                   // Does this note exist?
    bool exists(QString guid);              // Does this note exist?
    bool exists(string guid);               // Does this note exist?
    qint32 findNotesByTag(QList<qint32> &values, QString tags);
    qint32 findNotesByTag(QList<qint32> &values, string tags);
    qint32 findNotesByTag(QList<qint32> &values, qint32 tagLid);
    qint32 findNotesByTitle(QList<qint32> &lids, QString title);
    void updateNoteListTags(qint32 noteLid, QString tags);
    void updateNoteListNotebooks(QString guid, QString name);
    qint32 getNotesWithTag(QList<qint32> &retval, QString tag);  // Find all notes for a specific tag;
    void setIndexNeeded(qint32 lid, bool indexNeeded);    // flag if a note needs reindexing
    qint32 getIndexNeeded(QList<qint32> &lids);           // Get a list of all notes needing indexing
    bool updateNotebookGuid(QString oldGuid, QString newGuid, QString name);  // Update a notebook's name/guid
    bool updateNoteList(qint32 lid, Note &t, bool isDirty);  // Update the user viewing list
    bool updateNotebookName(qint32 lid, QString name);  // Update a notebook's name in the user listing
    void updateNotebook(qint32 noteLid, qint32 notebookLid);   // Set the current note's notebook
    void setDirty(qint32 lid, bool dirty);
    void updateNotebook(qint32 noteLid, qint32 notebookLid, bool setAsDirty=false);
    void updateUrl(qint32 lid, QString text, bool dirty);
    void updateTitle(qint32 noteLid, QString title, bool setAsDirty);
    void updateDate(qint32 lid, Timestamp ts, qint32 key, bool isDirty);
    void updateAuthor(qint32 lid, QString value, bool isDirty);
    void removeTag(qint32 noteLid, qint32 tag, bool isDirty);
    void addTag(qint32 lid, qint32 tag, bool isDirty);
    void rebuildNoteListTags(qint32 lid);   // Update the note's tags in the display table
    void deleteNote(qint32 lid, bool isDirty);           // mark a note for deletion
    void restoreNote(qint32 lid, bool isDirty);         // unmark a note for deletion
    void expunge(qint32 lid);              // expunge a note permanently
    void expunge(QString guid);            // expunge a note permanently
    void expunge(string guid);             // expunge a note permanently
    qint32 findNotesByNotebook(QList<qint32> &notes, QString guid);
    qint32 findNotesByNotebook(QList<qint32> &notes, string guid);
    void updateNoteContent(qint32 lid, QString content, bool isDirty=true);
    qint32 getCount();                                     // count of all notes
    qint32 getUnindexedCount();                            // count of unindexed notes
    qint32 getAllDeleted(QList<qint32> &lids);
    qint32 duplicateNote(qint32 oldLid);
    qint32 getAllDirty(QList<qint32> &lids);                // get all dirty notes
    void setUpdateSequenceNumber(qint32 lid, qint32 usn);   // set the update sequence number
    qint32 getNotebookLid(qint32 noteLid);                  // Get the notebook for a note
};



#endif // NOTETABLE_H
