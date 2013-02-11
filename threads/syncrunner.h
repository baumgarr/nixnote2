#ifndef SYNCRUNNER_H
#define SYNCRUNNER_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QTimer>


#include <evernote/UserStore.h>
#include <evernote/NoteStore.h>

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
//#include "java/javamachine.h"
#include "communication/communicationmanager.h"

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
    CommunicationManager *comm;
    //JavaMachine *jvm;


    QVector<QString> errorSharedNotebooks;
    QMap<QString,QString> errorShoredNotebooksIgnored;


    string consumerKey;
    string secret;
    string authToken;
    string userAgent;

    string username;
    string password;

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
    qint32 updateSequenceNumber;
    bool fullSync;
    QHash<QString, QString> changedNotebooks;
    QHash<QString, QString> changedTags;

    void evernoteSync();
    void syncRemoteToLocal(qint32 highSequence);
    //bool refreshConnection();
    void syncRemoteExpungedNotes(vector<string> guids);
    void syncRemoteExpungedNotebooks(vector<string> guids);
    void syncRemoteExpungedTags(vector<string> guids);
    void syncRemoteExpungedSavedSearches(vector<string> guid);

    void syncRemoteTags(vector<Tag> tag);
    void syncRemoteSearches(vector<SavedSearch> searches);
    void syncRemoteNotebooks(vector<Notebook> books);
    void syncRemoteNotes(vector<Note> notes);
    void syncRemoteResources(vector<Resource> resources);
    void syncRemoteLinkedNotebooks(vector<LinkedNotebook> books);

    void updateNoteTableTags();
    void updateNoteTableNotebooks();

    void checkForInkNotes(vector<Resource> &resources);


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
    void setMessage(QString message);
    void searchUpdated(qint32 lid, QString name);
    void tagUpdated(qint32 lid, QString name);
    void notebookUpdated(qint32 lid, QString name);
    void noteUpdated(qint32 lid);
    void tagExpunged(qint32 lid);
    void notebookExpunged(qint32 lid);
    void searchExpunged(qint32 lid);

 public slots:
    void synchronize();
};

#endif // SYNCRUNNER_H
