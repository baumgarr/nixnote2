#ifndef COMMUNICATIONMANAGER_H
#define COMMUNICATIONMANAGER_H

#include "global.h"

#include <QString>
#include <evernote/NoteStore.h>
#include <evernote/UserStore.h>

#include <inttypes.h>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>

#include <protocol/TBinaryProtocol.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <transport/TSSLServerSocket.h>
#include <transport/TSSLSocket.h>
#include <transport/THttpClient.h>
#include <server/TSimpleServer.h>
#include <QObject>
#include <QNetworkAccessManager>

using boost::shared_ptr;

using namespace std;
using namespace evernote::edam;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

//* Userid/password key & secret
//#define EDAM_CONSUMER_KEY "baumgarte"
//#define EDAM_CONSUMER_SECRET "eb8b5740e17cb55f"


//* OAuth key & secret
#define EDAM_CONSUMER_KEY = "baumgarr"
#define EDAM_CONSUMER_SECRET = "60d4cdedb074b0ac"


//#define EVERNOTE_HOST   "www.evernote.com"
//#define EDAM_USER_STORE_PATH "/edam/user"
//#define EDAM_NOTE_STORE_PATH "/edam/note/s1"
//#define EDAM_CLIENT_NAME "NixNote/Linux"

class CommunicationManager : public QObject
{
    Q_OBJECT

private:
    bool initComplete;
    bool initNoteStore();
    bool initUserStore();
    string userStorePath;
    string noteStorePath;
    string clientName;
    string evernoteHost;
    shared_ptr<TSocket> sslSocketNoteStore;
    shared_ptr<TTransport> noteStoreHttpClient;
    shared_ptr<NoteStoreClient> noteStoreClient;
    void downloadInkNoteImage(QString guid, Resource *r);

    shared_ptr<TSocket> sslSocketUserStore;
    shared_ptr<TTransport> userStoreHttpClient;
    shared_ptr<UserStoreClient> userStoreClient;
    //shared_ptr<AuthenticationResult> authenticationResult;
    string authToken;
    bool init();
    QNetworkAccessManager *networkAccessManager;
    void checkForInkNotes(vector<Resource> &resources);

public:
    CommunicationManager(QObject *parent = 0);
    ~CommunicationManager();
    bool connect();
    bool getSyncState(string authToken, SyncState &syncState);
    bool getSyncChunk(string token, SyncChunk &chunk, int start, int chunkSize, bool fullSync=false);
    string getToken();
    void disconnect();
    bool getUserInfo(User &user);
    QList< QPair<QString, QImage*>* > *inkNoteList;

public slots:
    int inkNoteReady(QImage *newImage, QImage *replyImage, int position);
};

#endif // COMMUNICATIONMANAGER_H
