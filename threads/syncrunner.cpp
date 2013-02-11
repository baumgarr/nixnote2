
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
#include "html/thumbnailer.h"
#include "nixnote.h"
#include "communication/communicationmanager.h"

extern Global global;

SyncRunner::SyncRunner()
{
    moveToThread(this);

    consumerKey = "baumgarte";
    secret = "eb8b5740e17cb55f";

    // Setup the user agent
    userAgent = "NixNote2/Linux";

    userStoreUrl = "http://www.evernote.com/edam/user";
    updateSequenceNumber = 0;
}

SyncRunner::~SyncRunner() {
}




void SyncRunner::run() {
    QLOG_DEBUG() << "SyncRunner starting";
    this->setPriority(QThread::LowPriority);
    comm = new CommunicationManager(this);
    exec();
    QLOG_DEBUG() << "Syncrunner exiting.";
}

void SyncRunner::synchronize() {
    QLOG_DEBUG() << "Starting SyncRunner.synchronize()";
    if (!comm->connect()) {
        QLOG_ERROR() << "Error initializing socket connections";
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
    UserTable userTable;
    comm->getUserInfo(user);
    userTable.updateUser(user);

    SyncState syncState;
    comm->getSyncState("", syncState);

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

    emit setMessage(tr("Beginning Sync"));
    // If there are remote changes
    QLOG_DEBUG() <<  "--->>>  Current Chunk High Sequence Number: " << syncState.updateCount;
    QLOG_DEBUG() <<  "--->>>  Last User High Sequence Number: " <<  updateSequenceNumber;

    if (syncState.updateCount > updateSequenceNumber) {
        QLOG_DEBUG() <<  "Remote changes found";
        QLOG_DEBUG() << "Downloading changes";
        emit setMessage(tr("Downloading changes"));
        syncRemoteToLocal(syncState.updateCount);
    }

    updateNoteTableTags();
    comm->getUserInfo(user);
    userTable.updateUser(user);
    comm->getSyncState("", syncState);
    userTable.updateSyncState(syncState);

    emit setMessage(tr("Sync Complete"));
    QLOG_TRACE() << "Leaving SyncRunner::evernoteSync()";
}



void SyncRunner::syncRemoteToLocal(qint32 updateCount) {
    QLOG_TRACE() << "Entering SyncRunner::SyncRemoteToLocal()";

    int chunkSize = 50;
    bool more = true;
    SyncChunk chunk;

    bool rc;
    int startingSequenceNumber = updateSequenceNumber;
    while(more && keepRunning)  {

        QSqlQuery query;
        query.exec("begin");
        rc = comm->getSyncChunk("", chunk, updateSequenceNumber, chunkSize, fullSync);
        if (!rc) {
            QLOG_ERROR() << "Error retrieving chunk";
            return;
        }
        QLOG_DEBUG() << "------>>>>  Old USN:" << updateSequenceNumber << " New USN:" << chunk.chunkHighUSN;
        int pct = (updateSequenceNumber-startingSequenceNumber)*100/(updateCount-startingSequenceNumber);
        emit setMessage(tr("Download ") +QString::number(pct) + tr("% complete."));

        if (chunk.expungedNotes.size() > 0)
            syncRemoteExpungedNotes(chunk.expungedNotes);
        if (chunk.expungedTags.size() > 0)
            syncRemoteExpungedTags(chunk.expungedTags);
        if (chunk.expungedNotebooks.size() > 0)
            syncRemoteExpungedNotebooks(chunk.expungedNotebooks);
        if (chunk.expungedSearches.size() > 0)
            syncRemoteExpungedSavedSearches(chunk.expungedSearches);
        if (chunk.tags.size() > 0)
            syncRemoteTags(chunk.tags);
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

        // Save any ink notes
        while (comm->inkNoteList->size() > 0) {
            QPair<QString, QImage *> *pair = comm->inkNoteList->takeFirst();
            ResourceTable resTable;
            qint32 resLid = resTable.getLid(pair->first);
            if (resLid > 0) {
                QString filename = global.fileManager.getDbaDirPath() + QString::number(resLid) + QString(".png");
                pair->second->save(filename);
            }
            delete pair->second;
            delete pair;
        }

        if (chunk.chunkHighUSN >= updateCount)
            more = false;
        updateSequenceNumber = chunk.chunkHighUSN;
        UserTable userTable;
        userTable.updateLastSyncNumber(updateSequenceNumber);
        userTable.updateLastSyncDate(chunk.currentTime);
        query.exec("commit");
      }
}


bool SyncRunner::enConnect() {
    if (global.connected)
        return true;
    return comm->connect();
}

// Expunge deleted notes from the local database
void SyncRunner::syncRemoteExpungedNotes(vector<string> guids) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteExpungedNotes";
    NoteTable noteTable;
    for (unsigned int i=0; i<guids.size(); i++) {
        noteTable.expunge(guids[i]);
    }
    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteExpungedNotes";
}


// Expunge deleted notebooks from the local database
void SyncRunner::syncRemoteExpungedNotebooks(vector<string> guids) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteExpungedNotebooks";
    NotebookTable notebookTable;
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
    TagTable tagTable;
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
    SearchTable searchTable;
    for (unsigned int i=0; i<guids.size(); i++) {
        int lid = searchTable.getLid(guids[i]);
        searchTable.expunge(guids[i]);
        emit searchExpunged(lid);
    }
    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteExpungedSavedSearches";
}

// Synchronize remote tags with the current database
// If there is a conflict, the remote wins
void SyncRunner::syncRemoteTags(vector<Tag> tags) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteTags";
    TagTable tagTable;

    for (unsigned int i=0; i<tags.size() && keepRunning; i++) {
        Tag t = tags.at(i);

        // There are two ways to get the tag.  We can get
        // it by name or by guid.  We check both.  We'll find it by
        // name if a new tag was created locally with the same name
        // as an unsynced remote.  We then merge them.  We'll find it by guid
        // if a note was synchrozied with this tag before a chunk
        // with this tag was downloaded.
        qint32 lid = tagTable.findByName(t.name);
        if (lid == 0)
            lid = tagTable.getLid(t.guid);

        if (lid > 0) {
            Tag currentTag;
            tagTable.get(currentTag, lid);
            if (currentTag.name != t.name)
                changedTags.insert(QString::fromStdString(t.guid), QString::fromStdString(t.name));
            tagTable.sync(lid, t);
        } else {
            tagTable.sync(t);
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
    SearchTable searchTable;

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
    NotebookTable notebookTable;

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
    NoteTable noteTable;

    for (unsigned int i=0; i<notes.size() && keepRunning; i++) {
        Note t = notes[i];
        qint32 lid = noteTable.getLid(t.guid);
        if (lid > 0) {
            noteTable.sync(lid, notes.at(i));
        } else {
            noteTable.sync(t);
            lid = noteTable.getLid(t.guid);
        }
        // Remove it from the cache (if it exists)
        if (global.cache.contains(lid))
            global.cache.remove(lid);
        emit noteUpdated(lid);
    }

    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteNotes";
}



// Synchronize remote resources with the current database
void SyncRunner::syncRemoteResources(vector<Resource> resources) {
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteResources";
    ResourceTable resTable;

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
    NoteTable noteTable;
    TagTable tagTable;
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
    NoteTable noteTable;

    // Go through the list of changed notebooks
    while (keys.hasNext()) {
        keys.next();
        noteTable.updateNoteListNotebooks(keys.key(), keys.value());
    }

}



// Synchronize remote resources with the current database
void SyncRunner::syncRemoteLinkedNotebooks(vector<LinkedNotebook> books) {
    LinkedNotebookTable btable;
    for (int i=0; i<books.size(); i++)
        btable.sync(books[i]);
}

