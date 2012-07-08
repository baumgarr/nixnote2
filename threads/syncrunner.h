#ifndef SYNCRUNNER_H
#define SYNCRUNNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QTimer>


#include "evernote/UserStore.h"
#include "evernote/NoteStore.h"

#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <server/TThreadedServer.h>
#include <server/TThreadPoolServer.h>

#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TSSLServerSocket.h>
#include <transport/TSSLSocket.h>
#include <transport/THttpClient.h>
#include "java/javamachine.h"

#include <protocol/TBinaryProtocol.h>

#include <iostream>
#include <string>
#include <stdio.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace boost;
using namespace apache::thrift::transport;
using namespace evernote::edam;
using namespace std;

class SyncRunner : public QThread
{
    Q_OBJECT
private:
    bool idle;
    bool error;
    long evernoteUpdateCount;
    JavaMachine *jvm;


    QVector<QString> errorSharedNotebooks;
    QMap<QString,QString> errorShoredNotebooksIgnored;


    string consumerKey;
    string secret;
    string authToken;
    string userAgent;

    shared_ptr<THttpClient> userStoreHttpClient;
    shared_ptr<UserStoreClient> userStoreClient;
    shared_ptr<THttpClient> noteStoreHttpClient;
    shared_ptr<NoteStoreClient> noteStoreClient;
    AuthenticationResult authResult;
    shared_ptr<AuthenticationResult> linkedAuthResult;
    shared_ptr<User> user;

    long authTimeRemaining;
    long authRefreshTime;
    bool authRefreshNeeded;
    long failedRefreshes;
    long sequenceDate;
    int updateSequenceNumber;
    bool fullSync;
    QTimer *refreshTimer;
    QHash<QString, QString> changedNotebooks;
    QHash<QString, QString> changedTags;

    void evernoteSync();
    void syncRemoteToLocal(shared_ptr<NoteStoreClient> client);
    bool refreshConnection();
    void syncRemoteTags(vector<Tag> tag);
    void syncRemoteSearches(vector<SavedSearch> searches);
    void syncRemoteNotebooks(vector<Notebook> books);
    void syncRemoteNotes(vector<Note> notes, shared_ptr<NoteStoreClient> client, bool fullSync, string token);
    void updateNoteTableTags();
    void updateNoteTableNotebooks();


public:
    bool keepRunning;
    string userStoreUrl;
    string noteStoreUrl;
    SyncRunner();
    ~SyncRunner();
    void run();
    bool enConnect();

signals:
    void syncComplete();
    void searchUpdated(int lid, QString name);
    void tagUpdated(int lid, QString name);
    void notebookUpdated(int lid, QString name);
    void noteUpdated(int lid);

 public slots:
    void synchronize();
};

#endif // SYNCRUNNER_H
