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

#include <iostream>
#include <string>
#include <stdio.h>

#include <QSqlTableModel>
#include <QtSql>
#include <QString>

#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;


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
#define NOTE_ATTRIBUTE_REMINDER_ORDER          5032
#define NOTE_ATTRIBUTE_REMINDER_TIME           5033
#define NOTE_ATTRIBUTE_REMINDER_DONE_TIME      5034

#define NOTE_TITLE_COLOR                       5995
#define NOTE_ISPINNED                          5996
#define NOTE_THUMBNAIL_NEEDED                  5997
#define NOTE_EXPUNGED_FROM_TRASH               5998
#define NOTE_INDEX_NEEDED                      5999

using namespace std;

class NoteTable
{

private:
    QSqlDatabase *db;

public:
    NoteTable(QSqlDatabase *db);                             // Constructor
    qint32 getLid(QString guid);            // given a guid, return the lid
    qint32 getLid(string guid);             // Given a guid, return the lid
    QString getGuid(int lid);               // given a lid, get the guid
    void updateGuid(qint32 lid, Guid &guid);    // Update a note's guid
    void sync(Note &note, qint32 account=0);                    // Sync a note with a new record
    void sync(qint32 lid, const Note &note, qint32 account=0);           // Sync a note with a new record
    qint32 add(qint32 lid, const Note &t, bool isDirty, qint32 account=0); // Add a new note
    bool get(Note &note, qint32 lid, bool loadResources, bool loadBinary);           // Get a note given a lid
    bool get(Note &note, QString guid, bool loadResources, bool loadBinary);      // get a note given a guid
    bool get(Note &note, string guid,bool loadResources, bool loadBinary);       // get a note given a guid
    bool isDirty(qint32 lid);                  // Check if a note is dirty
    bool isDirty(QString guid);             // Check if a note is dirty
    bool isDirty(string guid);              // Check if a note is dirty
    bool exists(qint32 lid);                   // Does this note exist?
    bool exists(QString guid);              // Does this note exist?
    bool exists(string guid);               // Does this note exist?
    bool isPinned(string guid);             // is the note pinned?
    bool isPinned(QString guid);            // is the note pinned?
    bool isPinned(qint32 lid);              // is the note pinned?
    void pinNote(string guid, bool value);             // pin the current note
    void pinNote(QString guid, bool value);            // pin the current note
    void pinNote(qint32 lid, bool value);              // pin the current note
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
    bool updateNoteList(qint32 lid, const Note &t, bool isDirty, qint32 account);  // Update the user viewing list
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
    void updateEnmediaHash(qint32 lid, QByteArray oldHash, QByteArray newHash, bool isDirty=true);
    qint32 getCount();                                     // count of all notes
    qint32 getUnindexedCount();                            // count of unindexed notes
    qint32 getAllDeleted(QList<qint32> &lids);
    qint32 duplicateNote(qint32 oldLid, bool keepCreatedDate=false);
    qint32 getAllDirty(QList<qint32> &lids);                // get all dirty notes
    qint32 getAllDirty(QList<qint32> &lids, qint32 notebookLid);  // Get all dirty for a particular (linked) notebook
    void setUpdateSequenceNumber(qint32 lid, qint32 usn);   // set the update sequence number
    qint32 getNotebookLid(qint32 noteLid);                  // Get the notebook for a note
    void setThumbnail(qint32 lid, QString filename);        // File containing the thumbnail
    void reindexAllNotes();                                 // Reindex all notes
    bool isDeleted(qint32 lid);                             // Is this note deleted?
    bool hasTag(qint32 noteLid, qint32 tagLid);             // Does this note have the specified tag?
    QString getNoteListTags(qint32 lid);                       // Get the string of tags from the notetable list
    void resetGeography(qint32 lid, bool isDirty);          // clear geography of note
    void setGeography(qint32 lid, double longitude, double latitude, double altitude, bool isDirty);
    void setThumbnailNeeded(qint32 lid, bool value);       // Do we need t ocreate a thumbnail?
    void setThumbnailNeeded(QString guid, bool value);      // Set if a thumbail is needed
    void setThumbnailNeeded(string guid, bool value);      // set if a thumbnail is needed
    bool isThumbnailNeeded(qint32 lid);                    // set if a thumbnail is needed
    bool isThumbnailNeeded(QString guid);                  // set if a thumbnail is needed
    bool isThumbnailNeeded(string guid);                   // set if a thumbnail is needed
    qint32 getNextThumbnailNeeded();                       // get any note that needs a thumbnail
    void setReminderCompleted(qint32 lid, bool completed);              // Set a note reminder as completed or clear it
    void removeReminder(qint32 lid);                        // Clear any existing reminder
    void getAllReminders(QList< QPair<qint32, qlonglong>* > *reminders);  // Get all notes with un-completed reminders
    qint32 getThumbnailsNeededCount();
    qint32 addStub(QString noteGuid);                     // Add a stub.  Used if a resource appears before the owning note
    void getAll(QList<qint32> &lids);
    void setTitleColor(qint32 lid, QString color);
    void getAllPinned(QList<QPair<qint32, QString> > &lids);
    void getRecentlyUpdated(QList< QPair< qint32, QString > > &lids);
};



#endif // NOTETABLE_H
