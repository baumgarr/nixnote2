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

#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include "qevercloud/include/QEverCloud.h"

#include "global.h"

#include <QString>
#include "communication/communicationerror.h"

#include <inttypes.h>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>

#include <QObject>
#include <QNetworkAccessManager>
#include <QSqlDatabase>

#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;

typedef unsigned int SOCKET;

//using boost::shared_ptr;

using namespace std;

//* Userid/password key & secret


//* OAuth key & secret
#define EDAM_CONSUMER_KEY = "baumgarr-3523"
#define EDAM_CONSUMER_SECRET = "8d5ee175f8a5d3ec"

#define SYNC_CHUNK_NOTEBOOKS            0x0001
#define SYNC_CHUNK_TAGS                 0x0002
#define SYNC_CHUNK_LINKED_NOTEBOOKS     0x0004
#define SYNC_CHKUNK_EXPUNGED            0x0008
#define SYNC_CHUNK_SEARCHES             0x0010
#define SYNC_CHUNK_NOTES                0x0020
#define SYNC_CHUNK_RESOURCES            0x0040

class CommunicationManager : public QObject
{
    Q_OBJECT

private:
    bool initComplete;
    QSqlDatabase *db;
    bool initNoteStore();
    QUrl *postData;

    QString userStorePath;
    QString noteStorePath;
    QString clientName;
    QString evernoteHost;

    QString linkedNoteStorePath;
    AuthenticationResult linkedAuth;
    QString linkedAuthToken;

    void downloadInkNoteImage(QString guid, Resource *r, QString shard, QString authToken);
    void checkForInkNotes(QList<Resource> &resources, QString shard, QString authToken);

    QString authToken;
    bool init();
    QNetworkAccessManager *networkAccessManager;
    void handleEDAMSystemException(EDAMSystemException e);
    void handleEDAMNotFoundException(EDAMNotFoundException e);
    UserStore *userStore;
    NoteStore *noteStore;
    NoteStore *linkedNoteStore;
    NoteStore *myNoteStore;
    void processSyncChunk(SyncChunk &chunk, QString token);


public:
    CommunicationManager(QSqlDatabase *db);
    ~CommunicationManager();
    CommunicationError error;
    bool connect();
    bool getSyncState(QString authToken, SyncState &syncState);
    bool getSyncChunk(SyncChunk &chunk, int start, int chunkSize, int type, bool fullSync, QString token="");
    bool getLinkedNotebookSyncState(SyncState &syncState, LinkedNotebook &book);
    bool getLinkedNotebookSyncChunk(SyncChunk &chunk, LinkedNotebook &book, int start, int chunkSize, bool fullSync);
    void disconnect();
    bool authenticateToLinkedNotebookShard(LinkedNotebook &book);
    bool authenticateToLinkedNotebook(AuthenticationResult &authResult, LinkedNotebook &book);
    bool getUserInfo(User &user);
    QList< QPair<QString, QImage*>* > *inkNoteList;
    QList< QPair<QString, QImage*>* > *thumbnailList;
    QHash<QString,QString> *tagGuidMap;
    bool getSharedNotebookByAuth(SharedNotebook &sharedNotebook);

    qint32 uploadSavedSearch(SavedSearch &search);
    qint32 expungeSavedSearch(Guid guid);

    qint32 uploadTag(Tag &tag);
    qint32 expungeTag(Guid guid);

    qint32 uploadNotebook(Notebook &notebook);
    qint32 expungeNotebook(Guid guid);

    qint32 uploadNote(Note &note, QString token="");
    qint32 uploadLinkedNote(Note &note);
    qint32 deleteNote(Guid guid, QString token="");
    qint32 deleteLinkedNote(Guid guid);

    bool getNotebookList(QList<Notebook> &list);
    bool getTagList(QList<Tag> &list);

    bool listNoteVersions(QList<NoteVersionId> &list, QString guid);
    bool getNoteVersion(Note &note, QString guid, qint32 usn, bool withResourceData=true, bool withResourceRecognition=true, bool withResourceAlternateData=true);
    void loadTagGuidMap();

public slots:
    int inkNoteReady(QImage *newImage, QImage *replyImage, int position);
};

#endif // COMMUNICATIONMANAGER_H
