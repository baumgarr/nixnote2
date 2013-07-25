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
#include <QTimer>

#include "syncrunner.h"
#include "evernote/UserStore_constants.h"
#include "global.h"
#include "sql/databaseconnection.h"
#include "sql/usertable.h"
#include "sql/tagtable.h"
#include "sql/searchtable.h"
#include "sql/notebooktable.h"
#include "sql/notetable.h"
#include "sql/linkednotebooktable.h"
#include "sql/resourcetable.h"
#include "sql/sharednotebooktable.h"
#include "html/thumbnailer.h"
#include "nixnote.h"
#include "communication/communicationmanager.h"
#include "communication/communicationerror.h"

extern Global global;

SyncRunner::SyncRunner()
{
    init = false;
}

SyncRunner::~SyncRunner() {
}


void SyncRunner::synchronize() {
    QLOG_DEBUG() << "Starting SyncRunner.synchronize()";
    if (!init) {
        init = true;
        consumerKey = "";
        secret = "";

        // Setup the user agent
        userAgent = "NixNote2/Linux";

        userStoreUrl = QString("http://" +global.server +"/edam/user").toStdString();
        updateSequenceNumber = 0;

         defaultMsgTimeout = 30000;
         comm = new CommunicationManager(this);
         db = new DatabaseConnection("syncrunner");
         connect(global.application, SIGNAL(stdException(QString)), this, SLOT(applicationException(QString)));
    }
    error = false;

    comm->error.reset();
    if (!comm->connect()) {
        this->communicationErrorHandler();
        error = true;
        emit syncComplete();
        return;
    }

    global.connected = true;
    keepRunning = true;
    evernoteSync();
    emit syncComplete();
    comm->disconnect();
    global.connected=false;
}




void SyncRunner::evernoteSync() {
    QLOG_DEBUG() << "Starting SyncRunner.evernoteSync()";
    if (!global.connected)
        return;

    User user;
    UserTable userTable(&db->conn);
    if (!comm->getUserInfo(user)) {
        this->communicationErrorHandler();
        error =true;
        return;
    }
    userTable.updateUser(user);

    SyncState syncState;
    if (!comm->getSyncState("", syncState)) {
        this->communicationErrorHandler();
        error = true;
        return;
    }

    fullSync = false;

    qlonglong lastSyncDate = userTable.getLastSyncDate();
    updateSequenceNumber = userTable.getLastSyncNumber();

    if ((syncState.fullSyncBefore/1000) > lastSyncDate) {
        QLOG_DEBUG() <<  "Full sequence date has expired";
        lastSyncDate = 0;
        fullSync = true;
    }

    if (updateSequenceNumber == 0)
        fullSync = true;

    emit setMessage(tr("Beginning Sync"), defaultMsgTimeout);
    // If there are remote changes
    QLOG_DEBUG() <<  "--->>>  Current Chunk High Sequence Number: " << syncState.updateCount;
    QLOG_DEBUG() <<  "--->>>  Last User High Sequence Number: " <<  updateSequenceNumber;

    if (syncState.updateCount > updateSequenceNumber) {
        QLOG_DEBUG() <<  "Remote changes found";
        QLOG_DEBUG() << "Downloading changes";
        emit setMessage(tr("Downloading changes"), defaultMsgTimeout);
        syncRemoteToLocal(syncState.updateCount);
    }
    syncRemoteLinkedNotebooksActual();


    if (!global.disableUploads) {
        qint32 searchUsn = uploadSavedSearches();
        if (searchUsn > updateSequenceNumber)
            updateSequenceNumber = searchUsn;
        qint32 tagUsn = uploadTags();
        if (tagUsn > updateSequenceNumber)
            updateSequenceNumber = tagUsn;
        qint32 notebookUsn = uploadNotebooks();
        if (notebookUsn > updateSequenceNumber)
            updateSequenceNumber = notebookUsn;
        qint32 personalNotesUsn = uploadPersonalNotes();
        if (personalNotesUsn > updateSequenceNumber)
            updateSequenceNumber = personalNotesUsn;
    }
    updateNoteTableTags();

    /*  This is commented out because for some reason it causes a "could not refill buffer error after
     * a successfull sync.
     */
    if (!comm->getUserInfo(user)) {
        this->communicationErrorHandler();
        error = true;
        return;
    }
    userTable.updateUser(user);
    if (!comm->getSyncState("", syncState)) {
        error =true;
        this->communicationErrorHandler();
        return;
    }
    userTable.updateSyncState(syncState);   
    /**/

    if (!error)
        emit setMessage(tr("Sync Complete"), defaultMsgTimeout);
    QLOG_TRACE() << "Leaving SyncRunner::evernoteSync()";
}



void SyncRunner::syncRemoteToLocal(qint32 updateCount) {
    QLOG_TRACE() << "Entering SyncRunner::SyncRemoteToLocal()";

    // The first thing we do is to see if any new tags or notebooks (ones with update sequence number = 0) have
    // been created.  If so, we get a list of notebooks & tags to try and find matching names on Evernote.
    // This avoids problems with duplicate or missing names later
    NotebookTable ntable(&db->conn);
    TagTable ttable(&db->conn);
    if (ntable.getNewUnsequencedCount() > 0) {
        vector<Notebook> books;
        if (comm->getNotebookList(books))
            syncRemoteNotebooks(books);
        else {
            QLOG_ERROR() << "Error retrieving notebook list";
            error = true;
            this->communicationErrorHandler();
            return;
        }

    }
    if (ttable.getNewUnsequencedCount() > 0) {
        vector<Tag> tags;
        if (comm->getTagList(tags))
            syncRemoteTags(tags);
        else {
            QLOG_ERROR() << "Error retrieving tag list";
            error = true;
            this->communicationErrorHandler();
            return;
        }

    }

    int chunkSize = 20;
    bool more = true;
    SyncChunk chunk;

    bool rc;
    int startingSequenceNumber = updateSequenceNumber;
    while(more && keepRunning)  {

        QSqlQuery query(db->conn);
        //query.exec("begin");
        rc = comm->getSyncChunk("", chunk, updateSequenceNumber, chunkSize, fullSync);
        if (!rc) {
            QLOG_ERROR() << "Error retrieving chunk";
            error = true;
            this->communicationErrorHandler();
            return;
        }
        QLOG_DEBUG() << "------>>>>  Old USN:" << updateSequenceNumber << " New USN:" << chunk.chunkHighUSN;
        int pct = (updateSequenceNumber-startingSequenceNumber)*100/(updateCount-startingSequenceNumber);
        emit setMessage(tr("Download ") +QString::number(pct) + tr("% complete."), defaultMsgTimeout);
        processSyncChunk(chunk);

        if (chunk.chunkHighUSN >= updateCount)
            more = false;
        updateSequenceNumber = chunk.chunkHighUSN;
        UserTable userTable(&db->conn);
        userTable.updateLastSyncNumber(updateSequenceNumber);
        userTable.updateLastSyncDate(chunk.currentTime);

        //query.exec("commit");
    }
    emit setMessage(tr("Download 100% complete."), defaultMsgTimeout);
}



// Deal with the sync chunk returned
void SyncRunner::processSyncChunk(SyncChunk &chunk, qint32 linkedNotebook) {

    // For public linked notebooks, we change any note's GUID to the
    // linked notebook guid.  This saves headaches later
    if (linkedNotebook > 0) {
        LinkedNotebookTable ltable(0, &db->conn);
        LinkedNotebook book;
        ltable.get(book, linkedNotebook);
        if (book.shareKey == "") {
            for (unsigned int i=0; i<chunk.notes.size(); i++) {
                chunk.notes[i].notebookGuid =  book.guid;
            }
        }
    }

    // Now start processing the chunk
    if (chunk.expungedNotes.size() > 0)
        syncRemoteExpungedNotes(chunk.expungedNotes);
    if (chunk.expungedTags.size() > 0)
        syncRemoteExpungedTags(chunk.expungedTags);
    if (chunk.expungedNotebooks.size() > 0)
        syncRemoteExpungedNotebooks(chunk.expungedNotebooks);
    if (chunk.expungedSearches.size() > 0)
        syncRemoteExpungedSavedSearches(chunk.expungedSearches);
    if (chunk.expungedLinkedNotebooks.size() > 0)
        syncRemoteExpungedLinkedNotebooks(chunk.expungedLinkedNotebooks);
    if (chunk.tags.size() > 0)
        syncRemoteTags(chunk.tags, linkedNotebook);
    if (chunk.notebooks.size() > 0)
        syncRemoteNotebooks(chunk.notebooks);
    if (chunk.searches.size() > 0)
        syncRemoteSearches(chunk.searches);
    if (chunk.notes.size() > 0)
        syncRemoteNotes(chunk.notes);
    if (chunk.resources.size() > 0)
        syncRemoteResources(chunk.resources);
    if (chunk.linkedNotebooks.size() > 0)
        syncRemoteLinkedNotebooks(chunk.linkedNotebooks);

    chunk.expungedLinkedNotebooks.clear();;
    chunk.expungedNotebooks.clear();
    chunk.expungedNotes.clear();
    chunk.expungedSearches.clear();
    chunk.expungedTags.clear();
    chunk.notebooks.clear();
    chunk.notes.clear();
    chunk.tags.clear();
    chunk.linkedNotebooks.clear();
    chunk.searches.clear();

    // Save any thumbnails notes
    while (comm->thumbnailList->size() > 0) {
        QPair<QString, QImage *> *pair = comm->thumbnailList->takeFirst();
        NoteTable nTable(&db->conn);
        qint32 lid = nTable.getLid(pair->first);
        if (lid > 0) {
            QString filename = global.fileManager.getThumbnailDirPath() + QString::number(lid) + QString(".png");
            pair->second->save(filename, "png");
            nTable.setThumbnail(lid, filename);
        }
        delete pair->second;
        delete pair;
    }

    // Save any ink notes
    while (comm->inkNoteList->size() > 0) {
        QPair<QString, QImage *> *pair = comm->inkNoteList->takeFirst();
        ResourceTable resTable(&db->conn);
        qint32 resLid = resTable.getLid(pair->first);
        if (resLid > 0) {
            QString filename = global.fileManager.getDbaDirPath() + QString::number(resLid) + QString(".png");
            pair->second->save(filename);
        }
        delete pair->second;
        delete pair;
    }
}



// Expunge deleted notes from the local database
void SyncRunner::syncRemoteExpungedNotes(vector<string> guids) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteExpungedNotes";
    NoteTable noteTable(&db->conn);
    for (unsigned int i=0; i<guids.size(); i++) {
        noteTable.expunge(guids[i]);
    }
    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteExpungedNotes";
}


// Expunge deleted notebooks from the local database
void SyncRunner::syncRemoteExpungedNotebooks(vector<string> guids) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteExpungedNotebooks";
    NotebookTable notebookTable(&db->conn);
    for (unsigned int i=0; i<guids.size(); i++) {
        int lid = notebookTable.getLid(guids[i]);
        notebookTable.expunge(guids[i]);
        emit notebookExpunged(lid);
    }
    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteExpungedNotebooks";
}


// Expunge deleted tags from the local database
void SyncRunner::syncRemoteExpungedTags(vector<string> guids) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteExpungedTags";
    TagTable tagTable(&db->conn);
    for (unsigned int i=0; i<guids.size(); i++) {
        int lid = tagTable.getLid(guids[i]);
        tagTable.expunge(guids[i]);
        emit tagExpunged(lid);
    }
    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteExpungedTags";
}


// Expunge deleted tags from the local database
void SyncRunner::syncRemoteExpungedSavedSearches(vector<string> guids) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteExpungedSavedSearches";
    SearchTable searchTable(&db->conn);
    for (unsigned int i=0; i<guids.size(); i++) {
        int lid = searchTable.getLid(guids[i]);
        searchTable.expunge(guids[i]);
        emit searchExpunged(lid);
    }
    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteExpungedSavedSearches";
}

// Synchronize remote tags with the current database
// If there is a conflict, the remote wins
void SyncRunner::syncRemoteTags(vector<Tag> tags, qint32 account) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteTags";
    TagTable tagTable(&db->conn);

    for (unsigned int i=0; i<tags.size() && keepRunning; i++) {
        Tag t = tags.at(i);

        // There are two ways to get the tag.  We can get
        // it by name or by guid.  We check both.  We'll find it by
        // name if a new tag was created locally with the same name
        // as an unsynced remote.  We then merge them.  We'll find it by guid
        // if a note was synchrozied with this tag before a chunk
        // with this tag was downloaded.
        qint32 lid = tagTable.findByName(t.name, account);
        if (lid == 0)
            lid = tagTable.getLid(t.guid);

        if (lid > 0) {
            Tag currentTag;
            tagTable.get(currentTag, lid);
            if (currentTag.name != t.name)
                changedTags.insert(QString::fromStdString(t.guid), QString::fromStdString(t.name));
            lid = tagTable.sync(lid, t, account);
        } else {
            tagTable.sync(t, account);
            lid = tagTable.getLid(t.guid);
            changedTags.insert(QString::fromStdString(t.guid), QString::fromStdString(t.name));
        }
        emit tagUpdated(lid, QString::fromStdString(t.name));
    }

    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteTags";
}


// Synchronize remote searches with the current database
// If there is a conflict, the remote wins
void SyncRunner::syncRemoteSearches(vector<SavedSearch> searches) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteSearches";
    SearchTable searchTable(&db->conn);

    for (unsigned int i=0; i<searches.size() && keepRunning; i++) {
        SavedSearch t = searches.at(i);
        qint32 lid = searchTable.getLid(t.guid);
        if (lid > 0) {
            searchTable.sync(lid, t);
        } else {
            searchTable.sync(t);
            lid = searchTable.getLid(t.guid);
        }
        emit searchUpdated(lid, QString::fromStdString(t.name));
    }

    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteSearches";
}

// Synchronize remote notebooks with the current database
// If there is a conflict, the remote wins
void SyncRunner::syncRemoteNotebooks(vector<Notebook> books) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteNotebooks";
    NotebookTable notebookTable(&db->conn);

    for (unsigned int i=0; i<books.size() && keepRunning; i++) {
        Notebook t = books.at(i);

        // There are two ways to get the notebook.  We can get
        // it by name or by guid.  We check both.  We'll find it by
        // name if a new notebook was created locally with the same name
        // as the remote.  We then merge them.  We'll find it by guid
        // if a note was synchrozied with this notebook before a chunk
        // with this notebook was downloaded.
        qint32 lid = notebookTable.findByName(t.name);
        if (lid == 0)
            lid = notebookTable.getLid(t.guid);

        if (lid > 0) {
            notebookTable.sync(lid, t);
        } else {
            notebookTable.sync(t);
            lid = notebookTable.getLid(t.guid);
        }
        changedNotebooks.insert(QString::fromStdString(t.guid), QString::fromStdString(t.name));
        emit notebookUpdated(lid, QString::fromStdString(t.name));
    }

    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteNotebooks";
}


// Synchronize remote notes with the current database
void SyncRunner::syncRemoteNotes(vector<Note> notes) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteNotes";
    NoteTable noteTable(&db->conn);
    NotebookTable bookTable(&db->conn);

    for (unsigned int i=0; i<notes.size() && keepRunning; i++) {
        Note t = notes[i];
        qint32 lid = noteTable.getLid(t.guid);
        if (lid > 0) {
            // Find out if it is a conflicting change
            if (noteTable.isDirty(lid)) {
                qint32 newLid = noteTable.duplicateNote(lid);
                qint32 conflictNotebook = bookTable.getConflictNotebook();
                noteTable.updateNotebook(newLid, conflictNotebook, true);
                emit noteUpdated(newLid);
             }
            noteTable.sync(lid, notes.at(i));
        } else {
            noteTable.sync(t);
            lid = noteTable.getLid(t.guid);
        }
        // Remove it from the cache (if it exists)
        if (global.cache.contains(lid))
            delete global.cache[lid];
            global.cache.remove(lid);
        emit noteUpdated(lid);
    }

    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteNotes";
}



// Synchronize remote resources with the current database
void SyncRunner::syncRemoteResources(vector<Resource> resources) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteResources";
    ResourceTable resTable(&db->conn);

    for (unsigned int i=0; i<resources.size(); i++) {
        Resource r = resources[i];
        qint32 lid = resTable.getLid(r.noteGuid, r.guid);
        if (lid > 0)
            resTable.sync(lid, r);
        else
            resTable.sync(r);
    }
    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteResources";
}



// Update the note table with any notebooks or tags that have changed
void SyncRunner::updateNoteTableTags() {
    QHashIterator<QString, QString> keys(changedTags);
    NoteTable noteTable(&db->conn);
    TagTable tagTable(&db->conn);
    QList<qint32> notesChanged;
    QHash<QString,QString> tagHash;

    // Create a list of tags.  We'll use this multiple times to avoid needing
    // to do multiple lookups.
    QList<qint32> tagLids;
    tagTable.getAll(tagLids);
    for (qint32 i=0; i<tagLids.size(); i++) {
        Tag tag;
        tagTable.get(tag, tagLids.at(i));
        tagHash.insert(QString::fromStdString(tag.guid), QString::fromStdString(tag.name));
    }

    while (keys.hasNext()) {
        keys.next();

        // Find out which notes need to have the tag names updated
        QList<qint32> noteLids;
        noteTable.findNotesByTag(noteLids, keys.key());

        // Go through the list of notes and rebuild the tag name string
        for (qint32 i=0; i<noteLids.size(); i++) {

            // If we haven't already rebuild this note, then we rebuild it now
            // If multiple tags for a note changed, we can run into the same
            // note twice, but rebuilding it once is all we need.
            if (!notesChanged.contains(noteLids[i])) {
                notesChanged.append(noteLids[i]);
                QMap<QString,QString> tagMap;
                Note n;
                noteTable.get(n, noteLids[i], false, false);
                for (unsigned int j=0; j<n.tagGuids.size(); j++) {
                    string noteTag = n.tagGuids.at(j);
                    QString tname = tagHash.value(QString::fromStdString(noteTag));
                    tagMap.insert(tname.toUpper(), tname);
                }

                //Now we have a map of the possible names, we need to build
                // the string for the table
                QString tagNames;
                QMapIterator<QString,QString> mapI(tagMap);
                while (mapI.hasNext()) {
                    mapI.next();
                    if (tagNames!="") {
                        tagNames=tagNames+", ";
                    }
                    tagNames = tagNames +mapI.value();
                }
                noteTable.updateNoteListTags(noteLids[i],tagNames);

            }
        }
    }

}



// Update the note table with any notebooks or tags that have changed
void SyncRunner::updateNoteTableNotebooks() {
    QHashIterator<QString, QString> keys(changedNotebooks);
    NoteTable noteTable(&db->conn);

    // Go through the list of changed notebooks
    while (keys.hasNext()) {
        keys.next();
        noteTable.updateNoteListNotebooks(keys.key(), keys.value());
    }

}



// Synchronize remote linked notebooks
void SyncRunner::syncRemoteLinkedNotebooks(vector<LinkedNotebook> books) {
    LinkedNotebookTable ltable(0, &db->conn);
    for (unsigned int i=0; i<books.size(); i++) {
        qint32 lid = ltable.sync(books[i]);
        if (!comm->authenticateToLinkedNotebookShard(books[i])) {
            this->communicationErrorHandler();
            error = true;
            return;
        }
        if (books[i].shareKey != "") {
            SharedNotebook sharedNotebook;
            if (!comm->getSharedNotebookByAuth(sharedNotebook)) {
                this->communicationErrorHandler();
                error = true;
                return;
            }
            SharedNotebookTable stable(&db->conn);
            stable.sync(lid, sharedNotebook);
        }
        NotebookTable ntable(&db->conn);

        // Build the dummy notebook entry
        Notebook book;
        book.guid = books[i].guid;
        book.__isset.guid = true;

        book.name = books[i].shareName;
        book.__isset.name = true;

        book.updateSequenceNum = books[i].updateSequenceNum;
        book.__isset.updateSequenceNum = true;

        book.published = true;
        book.published = true;

        book.publishing.uri = books[i].uri;
        book.publishing.__isset.uri = true;
        book.__isset.published = true;
        book.__isset.publishing = true;

        if (books[i].__isset.stack) {
            book.__isset.stack = true;
            book.stack = books[i].stack;
        }

        ntable.sync(lid, book);
        comm->disconnectFromLinkedNotebook();
        emit notebookUpdated(lid, QString::fromStdString(books[i].shareName));

    }
}




// Synchronize remote linked notebooks
void SyncRunner::syncRemoteLinkedNotebooksActual() {
    LinkedNotebookTable ltable(0,&db->conn);
    QList<qint32> lids;
    ltable.getAll(lids);
    for (int i=0; i<lids.size(); i++) {
        LinkedNotebook book;
        bool fs;
        qint32 usn = ltable.getLastUpdateSequenceNumber(lids[i]);
        ltable.get(book, lids[i]);
        SyncChunk chunk;
        int chunkSize = 50;
        if (!comm->authenticateToLinkedNotebookShard(book)) {
            this->communicationErrorHandler();
            error = true;
            return;
        }
        bool more = true;
        SyncState syncState;
        if (!comm->getLinkedNotebookSyncState(syncState, book)) {
            this->communicationErrorHandler();
            error = true;
            return;
        }
        if (syncState.updateCount <= usn)
            more=false;
        qint32 startingSequenceNumber = usn;
        if (usn == 0)
            fs = true;
        else
            fs = false;
        while (more && keepRunning) {
            if (!comm->getLinkedNotebookSyncChunk(chunk,book, usn, chunkSize, fs)) {
                this->communicationErrorHandler();
                error = true;
                return;
            }
            processSyncChunk(chunk, lids[i]);
            if (chunk.chunkHighUSN >= syncState.updateCount)
                more = false;
            ltable.setLastUpdateSequenceNumber(lids[i], chunk.chunkHighUSN);
            usn = chunk.chunkHighUSN;
            if (chunk.updateCount > startingSequenceNumber) {
                int pct = (usn-startingSequenceNumber)*100/(chunk.updateCount-startingSequenceNumber);
                emit setMessage(tr("Downloading ") +QString::number(pct) + tr("% complete for shared notebook ") +QString::fromStdString(book.shareName) + tr("."), defaultMsgTimeout);
            }
        }
        comm->disconnectFromLinkedNotebook();
    }
}




// Synchronize remote expunged linked notebooks
void SyncRunner::syncRemoteExpungedLinkedNotebooks(vector<string> guids) {
    LinkedNotebookTable btable(0,&db->conn);
    for (unsigned int i=0; i<guids.size(); i++)
        btable.expunge(guids[0]);
}



void SyncRunner::applicationException(QString s) {
    QLOG_DEBUG() << "Application Exception!!! : " << s;
}





// Upload any saved searchs
qint32 SyncRunner::uploadSavedSearches() {
    qint32 usn;
    qint32 maxUsn = 0;
    SearchTable stable(&db->conn);
    QList<qint32> lids;
    stable.getAllDirty(lids);
    if (lids.size() == 0)
        return 0;

    for (int i=0; i<lids.size(); i++) {
        SavedSearch search;
        stable.get(search, lids[i]);
        if (!stable.isDeleted(lids[i])) {
            qint32 oldUsn = search.updateSequenceNum;
            usn = comm->uploadSavedSearch(search);
            if (usn == 0) {
                this->communicationErrorHandler();
                error = true;
                return maxUsn;
            }
            if (usn > maxUsn) {
                maxUsn = usn;
                if (oldUsn == 0)
                    stable.updateGuid(lids[i], search.guid);
                stable.setUpdateSequenceNumber(lids[i], usn);
            } else {
                error = true;
            }
        } else {
            string guid = stable.getGuid(lids[i]);
            stable.expunge(lids[i]);
            if (search.updateSequenceNum > 0) {
                usn = comm->expungeSavedSearch(guid);
                if (usn>maxUsn)
                    maxUsn = usn;
            }
        }
    }
    return maxUsn;
}


// Upload any tags
qint32 SyncRunner::uploadTags() {
    qint32 usn;
    qint32 maxUsn = 0;
    TagTable table(&db->conn);
    QList<qint32> lids, deletedLids, updatedLids;
    table.resetLinkedTagsDirty();
    table.getAllDirty(lids);
    if (lids.size() == 0)
        return 0;
    // Split the lids into lids to be updated, and lids to be deleted
    for (int i=0; i<lids.size(); i++) {
        if (table.isDeleted(lids[i]))
            deletedLids.append(lids[i]);
        else
            updatedLids.append(lids[i]);
    }

    // Update any lids
    int i=0;
    while(updatedLids.size() > 0) {
        Tag tag;
        table.get(tag, updatedLids[i]);
        qint32 parentLid = 0;
        if (tag.__isset.parentGuid && tag.parentGuid != "")
            parentLid = table.getLid(tag.parentGuid);

        // If the parent is either not dirty, or there is no parent we can update this lid.
        if (parentLid <= 0 || !table.isDirty(parentLid)) {
            qint32 oldUsn = tag.updateSequenceNum;
            usn = comm->uploadTag(tag);
            if (usn == 0) {
                this->communicationErrorHandler();
                error = true;
                return maxUsn;
            }
            if (usn > 0) {
                maxUsn = usn;
                if (oldUsn == 0)
                    table.updateGuid(lids[i], tag.guid);
                table.setUpdateSequenceNumber(lids[i], usn);
                table.setDirty(tag.guid, false);
                updatedLids.removeAt(i);
                i=-1;  // Reset for the next time through the loop
            } else {
                error = true;
                updatedLids.clear();
            }
        }
        i++;
    }

    // delete any lids
    for (int i=0; i<deletedLids.size(); i++) {
        Tag tag;
        table.get(tag, deletedLids[i]);
        table.expunge(lids[i]);
        if (tag.updateSequenceNum > 0) {
            usn = comm->expungeTag(tag.guid);
            if (usn>maxUsn)
                maxUsn = usn;
        }
    }
    return maxUsn;
}




// Upload any saved searchs
qint32 SyncRunner::uploadNotebooks() {
    qint32 usn;
    qint32 maxUsn = 0;
    NotebookTable table(&db->conn);
    QList<qint32> lids;
    table.resetLinkedNotebooksDirty();
    table.getAllDirty(lids);
    for (int i=0; i<lids.size(); i++) {
        Notebook notebook;
        table.get(notebook, lids[i]);
        if (!table.isDeleted(lids[i])) {
            qint32 oldUsn = notebook.updateSequenceNum;
            usn = comm->uploadNotebook(notebook);
            if (usn == 0) {
                this->communicationErrorHandler();
                error = true;
                return maxUsn;
            }
            if (usn > maxUsn) {
                maxUsn = usn;
                if (oldUsn == 0)
                    table.updateGuid(lids[i], notebook.guid);
                table.setUpdateSequenceNumber(lids[i], usn);
            } else {
                error = true;
            }
        } else {
            QString guid;
            table.getGuid(guid, lids[i]);
            table.expunge(lids[i]);
            if (notebook.updateSequenceNum > 0) {
                usn = comm->expungeNotebook(guid.toStdString());
                if (usn>maxUsn)
                    maxUsn = usn;
            }
        }
    }
    return maxUsn;
}




// Upload notes that belong to me
qint32 SyncRunner::uploadPersonalNotes() {
    qint32 usn;
    qint32 maxUsn = 0;
    NotebookTable notebookTable(&db->conn);
    LinkedNotebookTable linkedNotebookTable(0,&db->conn);
    NoteTable noteTable(&db->conn);
    QList<qint32> lids, validLids, deletedLids;
    noteTable.getAllDirty(lids);

    // Get a list of all notes that are both dirty and in an account we own and isn't deleted
    for (int i=0; i<lids.size(); i++) {
        qint32 notebookLid = noteTable.getNotebookLid(lids[i]);
        if (!linkedNotebookTable.exists(notebookLid) && !notebookTable.isLocal(notebookLid)) {
            if (noteTable.isDeleted(lids[i]))
                deletedLids.append(lids[i]);
            else
                validLids.append(lids[i]);
        }
    }

    // Start uploading notes
    for (int i=0; i<validLids.size(); i++) {
        Note note;
        noteTable.get(note, validLids[i],true,true);
        qint32 oldUsn = note.updateSequenceNum;
        usn = comm->uploadNote(note);
        if (usn == 0) {
            this->communicationErrorHandler();
            error = true;
            return maxUsn;
        }
        if (usn > maxUsn) {
            maxUsn = usn;
            if (oldUsn == 0)
                noteTable.updateGuid(validLids[i], note.guid);
            noteTable.setUpdateSequenceNumber(validLids[i], usn);
            noteTable.setDirty(validLids[i], false);
            emit(noteSynchronized(validLids[i], false));
        } else {
            error = true;
        }
    }

    // Start deleting notes
    for (int i=0; i<deletedLids.size(); i++) {
        QString guid = noteTable.getGuid(deletedLids[i]);
        noteTable.setDirty(lids[i], false);
        usn = comm->deleteNote(guid.toStdString());
        if (usn > maxUsn) {
            maxUsn = usn;
            noteTable.setUpdateSequenceNumber(deletedLids[i], usn);
            noteTable.setDirty(deletedLids[i], false);
            emit(noteSynchronized(deletedLids[i], false));
        }
    }
    return maxUsn;
}





// Return a pointer to the CommunicationManager error class
CommunicationError* SyncRunner::getError() {
    return &comm->error;
}


// If a communication error happened, try to determine what the error is and
// notify the user
void SyncRunner::communicationErrorHandler() {
    QString emitMsg;
    if (comm->error.type == CommunicationError::TTransportException) {
        if (comm->error.message != "")
            emitMsg = "Network Transport error: " +comm->error.message;
        else
            emitMsg = "Network Transport error communicating with Evernote";
        emit(setMessage(emitMsg, 0));
        return;
    }

    if (comm->error.type == CommunicationError::EDAMSystemException) {
        if (comm->error.message != "")
            emitMsg = "Evernote System Error: " +comm->error.message;
        else
            emitMsg = "Evernote System Error communicating with Evernote.";
        emit(setMessage(emitMsg, 0));
        return;
    }

    if (comm->error.type == CommunicationError::EDAMNotFoundException) {
        if (comm->error.message != "")
            emitMsg = comm->error.message;
        else
            emitMsg = "Evernote \"Not Found\" error.";
        emit(setMessage(emitMsg, 0));
        return;
    }

    if (comm->error.type == CommunicationError::EDAMUserException) {
        CommunicationError *e = &comm->error;

        if (e->code == EDAMErrorCode::UNKNOWN)
            emitMsg = "An unknown error has occurred : " +e->message;

        if (e->code == EDAMErrorCode::BAD_DATA_FORMAT)
            emitMsg = "Bad data format : " +e->message;

        if (e->code == EDAMErrorCode::PERMISSION_DENIED)
            emitMsg = "Permission denied : " +e->message;

        if (e->code == EDAMErrorCode::INTERNAL_ERROR)
            emitMsg = "Internal Evernote error : " +e->message + " Please try again later.";

        if (e->code == EDAMErrorCode::DATA_REQUIRED)
            emitMsg = "Communication Error - Data required : " +e->message;

        if (e->code == EDAMErrorCode::INVALID_AUTH)
            emitMsg = "Invalid authorization : " +e->message;

        if (e->code == EDAMErrorCode::AUTH_EXPIRED) {
            emitMsg = "Authorization token has expired or been revoked.";
            global.accountsManager->setOAuthToken("");
        }

        if (e->code == EDAMErrorCode::DATA_CONFLICT)
            emitMsg = "Communication Error - Data conflict : " +e->message;

        if (e->code == EDAMErrorCode::ENML_VALIDATION)
            emitMsg = "Unable to update note.  Invalid note structure : " +e->message;

        if (e->code == EDAMErrorCode::LIMIT_REACHED)
            emitMsg = "Communication Error - limit reached : " +e->message;

        if (e->code == EDAMErrorCode::QUOTA_REACHED)
            emitMsg = "Communication Error - User quota exceeded : " +e->message;

        if (e->code == EDAMErrorCode::SHARD_UNAVAILABLE)
            emitMsg = "Communication Error - Shard unavailable.  Please try again later. " +e->message;

        if (e->code == EDAMErrorCode::LEN_TOO_SHORT)
            emitMsg = "Communication Error - Length too short : " +e->message;

        if (e->code == EDAMErrorCode::LEN_TOO_LONG)
            emitMsg = "Communication Error - Length too long : " +e->message;

        if (e->code == EDAMErrorCode::TOO_FEW)
            emitMsg = "Communication Error - Length \"too few\" error : " +e->message;

        if (e->code == EDAMErrorCode::TOO_MANY)
            emitMsg = "Communication Error - \"too many\" error : " +e->message;

        if (e->code == EDAMErrorCode::UNSUPPORTED_OPERATION)
            emitMsg = "Communication Error - Unsupported operation " +e->message;

        emit(setMessage(emitMsg, 0));
        return;
    }

    if (comm->error.type == CommunicationError::TSSLException) {
        CommunicationError *e = &comm->error;
        emitMsg = "Communication Error - SSL Exception: " +e->message;
        emit(setMessage(emitMsg, 0));
        return;
    }

    if (comm->error.type == CommunicationError::StdException) {
        CommunicationError *e = &comm->error;
        emitMsg = "Internal Error: " +e->message;
        emit(setMessage(emitMsg, 0));
        return;
    }

}
