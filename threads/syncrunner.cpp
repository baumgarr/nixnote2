
//#include <QDebug>
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
#include "html/thumbnailer.h"
#include "nixnote.h"

extern Global global;

SyncRunner::SyncRunner()
{
    moveToThread(this);

    consumerKey = "baumgarte";
    secret = "eb8b5740e17cb55f";

    // Setup the user agent
    userAgent = "NixNote/Linux";

    userStoreUrl = "http://www.evernote.com/edam/user";
}

SyncRunner::~SyncRunner() {
}




void SyncRunner::run() {
    QLOG_DEBUG() << "Starting SyncRunner";
    jvm = new JavaMachine();
    jvm->create_jvm();
    refreshTimer = new QTimer();
    refreshTimer->setInterval(300000);  // Check refresh timer every 5 minutes
//    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(refreshConnection()));
    refreshTimer->start();
    exec();
}

void SyncRunner::synchronize() {
    keepRunning = true;
    if (enConnect())  {
       evernoteSync();

       // End the connection
       QLOG_DEBUG() << "Emitting syncComplete()";
       emit syncComplete();
    }
}




void SyncRunner::evernoteSync() {
    if (!global.connected)
        return;

    SyncState syncState;
    jvm->getSyncState(syncState);

    return;
    ////////////////////////////
    QLOG_TRACE() << "Entering SyncRunner::evernoteSync()";
    //SyncState syncState;
    int highSequenceNumber = 0;

    try {
        UserTable userTable;

        noteStoreClient->getSyncState(syncState, authToken);

        qlonglong lastSyncDate = userTable.getLastSyncDate();
        updateSequenceNumber = userTable.getLastSyncNumber();

        if (syncState.fullSyncBefore > lastSyncDate) {
            QLOG_DEBUG() <<  "Full sequence date has expired";
            lastSyncDate = 0;
            updateSequenceNumber = 0;
         }

        // If there are remote changes
        QLOG_DEBUG() <<  "Current Sequence Number: " << syncState.updateCount;
        QLOG_DEBUG() <<  "Last Sequence Number: " << updateSequenceNumber;

        if (syncState.updateCount > updateSequenceNumber) {
                QLOG_DEBUG() <<  "Remote changes found";
                QLOG_DEBUG() << "Downloading changes";
                highSequenceNumber = syncState.updateCount;
                syncRemoteToLocal(noteStoreClient);
        }
        userTable.updateSyncState(syncState);

    } catch (EDAMUserException e) {
        QLOG_FATAL() << e.what();
        return;
    }  catch (EDAMSystemException e) {
        QLOG_FATAL() << e.what() ;
        return;
    } catch (EDAMNotFoundException e) {
        QLOG_FATAL() << e.what();
        return;
    }
    QLOG_TRACE() << "Leaving SyncRunner::evernoteSync()";
}



void SyncRunner::syncRemoteToLocal(shared_ptr<NoteStoreClient> client) {
    /*
    QLOG_TRACE() << "Entering SyncRunner::evernoteSync()";

    int chunkSize = 100;
    fullSync = false;
    bool more = true;
    SyncChunk chunk;

    if (updateSequenceNumber == 0)
        fullSync = true;

    int sequence = updateSequenceNumber;
    while(more && keepRunning)  {
        if (authRefreshNeeded)
            if (!refreshConnection())
                return;
        try {
            client->getSyncChunk(chunk, authToken, sequence, chunkSize, fullSync);

        } catch(EDAMUserException e) {
            error = true;
            QLOG_FATAL() << "EDAMUserException: " << e.what();
            return;
        } catch(EDAMSystemException e) {
            error = true;
            QLOG_FATAL() << "EDAMSystemException: " << e.what();
            return;
        } catch(TException e) {
            error = true;
            QLOG_FATAL() << "TException: " << e.what();
            return;
        }

        QSqlQuery q;
        q.exec("begin");
        syncRemoteTags(chunk.tags);
        chunk.tags.clear();
        syncRemoteSearches(chunk.searches);
        chunk.searches.clear();
        syncRemoteNotebooks(chunk.notebooks);
        chunk.notebooks.clear();
        syncRemoteNotes(chunk.notes, client, fullSync, authToken);
        chunk.notes.clear();
        if (fullSync)
            chunk.resources.clear();

        sequence = chunk.chunkHighUSN;
        QLOG_DEBUG() << "Chunk high sequence number: " << chunk.chunkHighUSN;
        QLOG_DEBUG() << "Maximum sequence number: " << chunk.updateCount;
        if (chunk.chunkHighUSN >= chunk.updateCount) {
            more = false;
        }
        q.exec("commit");

    }
    updateNoteTableTags();
    updateNoteTableNotebooks();
    QLOG_TRACE() << "Leaving SyncRunner::evernoteSync()";
    */
}


bool SyncRunner::enConnect() {
    if (global.connected)
        return true;
    jvm->connect();
    return true;
}

// Refresh a connection before the timer expires
bool SyncRunner::refreshConnection() {
    if (!global.connected)
        return false;
}


// Synchronize remote tags with the current database
// If there is a conflict, the remote wins
void SyncRunner::syncRemoteTags(vector<Tag> tags) {
    return;
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteTags";
    TagTable tagTable;

    for (unsigned int i=0; i<tags.size() && keepRunning; i++) {
        Tag t = tags.at(i);
        int lid = tagTable.findByName(t.name);
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
    return;
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteSearches";
    SearchTable searchTable;

    for (unsigned int i=0; i<searches.size() && keepRunning; i++) {
        SavedSearch t = searches.at(i);
        int lid = searchTable.getLid(t.guid);
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
    return;
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteNotebooks";
    NotebookTable notebookTable;

    for (unsigned int i=0; i<books.size() && keepRunning; i++) {
        Notebook t = books.at(i);
        int lid = notebookTable.findByName(t.name);
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
void SyncRunner::syncRemoteNotes(vector<Note> notes, shared_ptr<NoteStoreClient> client, bool full, string token) {
    return;
    QLOG_TRACE() << "Entering SyncRunner::syncRemoteNotes";
    NoteTable noteTable;

    Note n;
    for (unsigned int i=0; i<notes.size() && keepRunning; i++) {
        Note t = notes[i];
        int lid = noteTable.getLid(t.guid);
        string guid = t.guid;
        client->getNote(n, token, guid, true, full, true,true);
        if (lid > 0) {
            noteTable.sync(lid, notes.at(i));
        } else {
            noteTable.sync(n);
            lid = noteTable.getLid(t.guid);
        }
        //hammer->setNote(lid, n);
        emit noteUpdated(lid);
    }

    QLOG_TRACE() << "Leaving SyncRunner::syncRemoteNotes";
}



// Update the note table with any notebooks or tags that have changed
void SyncRunner::updateNoteTableTags() {
    return;
    QHashIterator<QString, QString> keys(changedTags);
    NoteTable noteTable;
    TagTable tagTable;
    QList<int> notesChanged;
    QHash<QString,QString> tagHash;

    // Create a list of tags.  We'll use this multiple times to avoid needing
    // to do multiple lookups.
    QList<int> tagLids;
    tagTable.getAll(tagLids);
    for (int i=0; i<tagLids.size(); i++) {
        Tag tag;
        tagTable.get(tag, tagLids.at(i));
        tagHash.insert(QString::fromStdString(tag.guid), QString::fromStdString(tag.name));
    }

    while (keys.hasNext()) {
        keys.next();

        // Find out which notes need to have the tag names updated
        QList<int> noteLids;
        noteTable.findNotesByTag(noteLids, keys.key());

        // Go through the list of notes and rebuild the tag name string
        for (int i=0; i<noteLids.size(); i++) {

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
    return;
    QHashIterator<QString, QString> keys(changedNotebooks);
    NoteTable noteTable;

    // Go through the list of changed notebooks
    while (keys.hasNext()) {
        keys.next();
        noteTable.updateNoteListNotebooks(keys.key(), keys.value());
    }

}
