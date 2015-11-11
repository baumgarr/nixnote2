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


#ifndef SYNCRUNNER_H
#define SYNCRUNNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QTimer>
#include "communication/communicationmanager.h"
#include "sql/databaseconnection.h"

#include <iostream>
#include <string>
#include <stdio.h>

#include "qevercloud/include/QEverCloud.h"
using namespace qevercloud;

class SyncRunner : public QObject
{
    Q_OBJECT
private:
    bool idle;
    bool init;
    int defaultMsgTimeout;
    long evernoteUpdateCount;
    DatabaseConnection *db;
    CommunicationManager *comm;

    QVector<QString> errorSharedNotebooks;
    QMap<QString,QString> errorSharedNotebooksIgnored;


    string consumerKey;
    string secret;
    string authToken;
    string userAgent;

    string username;
    string password;

    long authTimeRemaining;
    long authRefreshTime;
    bool authRefreshNeeded;
    long failedRefreshes;
    long sequenceDate;
    qint32 updateSequenceNumber;
    bool fullSync;
    QHash<QString, QString> changedNotebooks;
    QHash<QString, QString> changedTags;

    void evernoteSync();
    bool syncRemoteToLocal(qint32 highSequence);
    void syncRemoteExpungedNotes(QList<Guid> guids);
    void syncRemoteExpungedNotebooks(QList<Guid> guids);
    void processSyncChunk(SyncChunk &chunk, qint32 linkedNotebook=0);
    void syncRemoteExpungedTags(QList<Guid> guids);
    void syncRemoteExpungedSavedSearches(QList<Guid> guid);

    void syncRemoteTags(QList<Tag> tag, qint32 account=0);
    void syncRemoteSearches(QList<SavedSearch> searches);
    void syncRemoteNotebooks(QList<Notebook> books, qint32 account=0);
    void syncRemoteNotes(QList<Note> notes, qint32 account=0);
    void syncRemoteResources(QList<Resource> resources);
    void syncRemoteLinkedNotebooksChunk(QList<LinkedNotebook> books);
    void syncRemoteExpungedLinkedNotebooks(QList<Guid> guids);
    bool syncRemoteLinkedNotebooksActual();

    //void checkForInkNotes(QList<Resource> &resources);

    qint32 uploadSavedSearches();
    qint32 uploadTags();
    qint32 uploadNotebooks();
    qint32 uploadPersonalNotes();
    qint32 uploadLinkedNotes(qint32 notebookLid);


public:
    bool keepRunning;
    string userStoreUrl;
    string noteStoreUrl;
    SyncRunner();
    ~SyncRunner();
    bool error;
    CommunicationError* getError();
    void communicationErrorHandler();
    bool finalSync;

signals:
    void syncComplete();
    void setMessage(QString message, int timeout);
    void searchUpdated(qint32 lid, QString name);
    void tagUpdated(qint32 lid, QString name, QString parentGuid, qint32 account);
    void notebookUpdated(qint32 lid, QString name, QString stack, bool linked, bool shared);
    void noteUpdated(qint32 lid);
    void tagExpunged(qint32 lid);
    void notebookExpunged(qint32 lid);
    void searchExpunged(qint32 lid);
    void noteSynchronized(qint32, bool);

 public slots:
    void synchronize();
    void applicationException(QString);
};

#endif // SYNCRUNNER_H
