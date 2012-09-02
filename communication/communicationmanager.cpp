#include "communicationmanager.h"
#include "oauth/oauthtokenizer.h"
#include "global.h"

extern Global global;

CommunicationManager::CommunicationManager()
{
    initComplete = false;
//    evernoteHost = global.server;
    evernoteHost = global.server.toStdString();
    userStorePath = "/edam/user";
    clientName = "NixNote/Linux";
}


CommunicationManager::~CommunicationManager() {
    if (sslSocketUserStore != NULL) {
        sslSocketUserStore->setRecvTimeout(10);
        sslSocketUserStore->close();
    }
    if (sslSocketNoteStore != NULL) {
        sslSocketNoteStore->setRecvTimeout(10);
        sslSocketNoteStore->close();
    }
}


bool CommunicationManager::connect() {
    // Get the oAuth token
    QFile oauthFile(global.fileManager.getHomeDirPath("oauth.txt"));
    oauthFile.open(QIODevice::ReadOnly);
    OAuthTokenizer tokenizer;
    QByteArray data = oauthFile.readAll();
    oauthFile.close();
    tokenizer.tokenize(data);
    authToken = tokenizer.oauth_token.toStdString();
    return init();
}


bool CommunicationManager::getSyncState(string token, SyncState &syncState) {
    if (token == "")
        token = authToken;
    try {
        noteStoreClient->getSyncState(syncState, token);
        cout << "New count: "<< syncState.updateCount << endl;
        return true;
    } catch (EDAMUserException e) {
        QLOG_ERROR() << "EDAMUserException:" << e.errorCode << endl;
        return false;
    } catch (EDAMSystemException e) {
        QLOG_ERROR() << "EDAMSystemException:" << QString::fromStdString(e.message) << endl;
        return false;
    } catch (TTransportException e) {
        QLOG_ERROR() << "TTransportException:" << e.what() << endl;
        return false;
    }
}



bool CommunicationManager::getSyncChunk(string token, SyncChunk &chunk, int start, int chunkSize, bool fullSync) {
    if (token == "")
        token = authToken;
    try {
        noteStoreClient->getSyncChunk(chunk, token, start, chunkSize, fullSync);
        for (unsigned int i=0; chunk.__isset.notes && i<chunk.notes.size(); i++) {
            QLOG_DEBUG() << "Fetching chunk item: " << i << ": " << QString::fromStdString(chunk.notes[i].title);
            Note n;
            noteStoreClient->getNote(n, token, chunk.notes[i].guid, true, fullSync, fullSync, fullSync);
            chunk.notes[i] = n;
        }
    } catch (EDAMUserException e) {
        QLOG_ERROR() << "EDAMUserException:" << e.errorCode << endl;
        return false;
    } catch (EDAMSystemException e) {
        QLOG_ERROR() << "EDAMSystemException:" << QString::fromStdString(e.message) << endl;
        return false;
    } catch (TTransportException e) {
        QLOG_ERROR() << "TTransportException:" << e.what() << endl;
        return false;
    }
    return true;
}




bool CommunicationManager::init() {
    if (initComplete)
        return true;
    if (!initUserStore())
        return false;
    if (!initNoteStore())
        return false;
    initComplete = true;
    return true;
}


string CommunicationManager::getToken() {
    //return authenticationResult->authenticationToken;
    return authToken;
}

bool CommunicationManager::initUserStore() {
    QLOG_DEBUG() << "Inside CommunicationManager::initUserStore()";
    try {



        shared_ptr<TSSLSocketFactory> sslSocketFactory(new TSSLSocketFactory());
        QString pgmDir = global.getProgramDirPath() + "/certs/PCA-3G2.pem";
        sslSocketFactory->loadTrustedCertificates(pgmDir.toStdString().c_str());
        sslSocketFactory->authenticate(true);

        sslSocketUserStore = sslSocketFactory->createSocket(evernoteHost, 443);

        shared_ptr<TBufferedTransport> bufferedTransport(new TBufferedTransport(sslSocketUserStore));
        userStoreHttpClient = shared_ptr<TTransport>(new THttpClient(bufferedTransport, evernoteHost, userStorePath));

        userStoreHttpClient->open();
        shared_ptr<TProtocol> iprot(new TBinaryProtocol(userStoreHttpClient));
        userStoreClient = shared_ptr<UserStoreClient>(new UserStoreClient(iprot));

//        authenticationResult = shared_ptr<AuthenticationResult>(new AuthenticationResult());

//        QLOG_DEBUG() << "authenticating";
//        userStoreClient->authenticate(
//                    *authenticationResult,
//                    "<userid>",
//                    "<password>",
//                    EDAM_CONSUMER_KEY,
//                    EDAM_CONSUMER_SECRET);

    } catch (EDAMUserException e) {
        QLOG_ERROR() << "EDAMUserException:" << e.errorCode << endl;
        return false;
    } catch (EDAMSystemException e) {
        QLOG_ERROR() << "EDAMSystemException:" << QString::fromStdString(e.message) << endl;
        return false;
    } catch (TTransportException e) {
        QLOG_ERROR() << "TTransportException:" << e.what() << endl;
        return false;
    }
    QLOG_DEBUG() << "Leaving CommunicationManager::initUserStore()";
    return true;
}


bool CommunicationManager::initNoteStore() {


    QLOG_DEBUG() << "Inside CommunicationManager::initNoteStore()";

    try {

        shared_ptr<TSSLSocketFactory> sslSocketFactory(new TSSLSocketFactory());
        sslSocketFactory->loadTrustedCertificates("/home/randy/Dropbox/PCA-3G2.pem");
        sslSocketFactory->authenticate(true);

        sslSocketNoteStore = sslSocketFactory->createSocket(evernoteHost, 443);
        shared_ptr<TBufferedTransport> bufferedTransport(new TBufferedTransport(sslSocketNoteStore));
        User user;
        userStoreClient->getUser(user, authToken);
        noteStorePath = "/edam/note/" +user.shardId;
        noteStoreHttpClient = shared_ptr<TTransport>(new THttpClient(bufferedTransport, evernoteHost, noteStorePath));

        noteStoreHttpClient->open();
        shared_ptr<TProtocol> noteStoreProtocol(new TBinaryProtocol(noteStoreHttpClient));
        noteStoreClient = shared_ptr<NoteStoreClient>(new NoteStoreClient(noteStoreProtocol));

        SyncState syncState;
        noteStoreClient->getSyncState(syncState, authToken);

    } catch (EDAMUserException e) {
        QLOG_ERROR() << "EDAMUserException:" << e.errorCode << endl;
        return false;
    } catch (EDAMSystemException e) {
        QLOG_ERROR() << "EDAMSystemException:" << QString::fromStdString(e.message) << endl;
        return false;
    } catch (TTransportException e) {
        QLOG_ERROR() << "TTransportException:" << e.what() << endl;
        return false;
    }
    QLOG_DEBUG() << "Leaving CommunicationManager::initNoteStore()";
    return true;
}
