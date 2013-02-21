
#include "communicationmanager.h"
#include "oauth/oauthtokenizer.h"
#include "global.h"

#include <execinfo.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "evernote/UserStore_constants.h"
#include "sql/resourcetable.h"
#include <sql/usertable.h>
#include <QObject>
#include <QPainter>

extern Global global;

CommunicationManager::CommunicationManager(QObject *parent) :
    QObject(parent)
{
    initComplete = false;
    evernoteHost = global.server.toStdString();
    userStorePath = "/edam/user";
    clientName = "NixNote/Linux";
    networkAccessManager = new QNetworkAccessManager();
    inkNoteList = new QList< QPair<QString, QImage*>* >();
    //networkAccessManager->connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(inkNoteReady(QNetworkReply*)));
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
//    QFile oauthFile(global.fileManager.getHomeDirPath("oauth.txt"));
//    oauthFile.open(QIODevice::ReadOnly);
    OAuthTokenizer tokenizer;
//    QByteArray data = oauthFile.readAll();
//    oauthFile.close();
    QString data = global.accountsManager->getOAuthToken();
    tokenizer.tokenize(data);
    authToken = tokenizer.oauth_token.toStdString();
    return init();
}


bool CommunicationManager::getSyncState(string token, SyncState &syncState) {
    if (token == "")
        token = authToken;
    try {
        noteStoreClient->getSyncState(syncState, token);
        QLOG_DEBUG() << "New count: "<< syncState.updateCount;
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

    // Get rid of old stuff from last chunk
    while(inkNoteList->size() > 0) {
        QPair<QString, QImage*> *pair = inkNoteList->takeLast();
        delete pair->second;
        delete pair;
    }
    inkNoteList->empty();

    // Try to get the chunk
    try {
        noteStoreClient->getSyncChunk(chunk, token, start, chunkSize, fullSync);
        for (unsigned int i=0; chunk.__isset.notes && i<chunk.notes.size(); i++) {
            QLOG_DEBUG() << "Fetching chunk item: " << i << ": " << QString::fromStdString(chunk.notes[i].title);
            Note n;
            noteStoreClient->getNote(n, token, chunk.notes[i].guid, true, fullSync, fullSync, fullSync);
            chunk.notes[i] = n;
            if (n.__isset.resources && n.resources.size() > 0) {
                checkForInkNotes(n.resources);
            }
        }
        for (unsigned int i=0; chunk.__isset.resources && i<chunk.resources.size(); i++) {
            QLOG_DEBUG() << "Fetching chunk resource item: " << i << ": " << QString::fromStdString(chunk.resources[i].guid);
            Resource r;
            noteStoreClient->getResource(r, token, chunk.resources[i].guid, true, true, true, true);
            chunk.resources[i] = r;
        }
        if (chunk.__isset.resources && chunk.resources.size()>0)
            checkForInkNotes(chunk.resources);
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


bool CommunicationManager::getSharedNotebookByAuth(SharedNotebook &sharedNotebook) {
    try {
        linkedNoteStoreClient->getSharedNotebookByAuth(sharedNotebook, linkedAuthToken.authenticationToken);
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


bool CommunicationManager::authenticateToLinkedNotebookShard(LinkedNotebook book) {


    QLOG_DEBUG() << "Inside CommunicationManager::authenticateToLinkedNotebook()";

    disconnectFromLinkedNotebook();

    try {

        shared_ptr<TSSLSocketFactory> sslSocketFactory(new TSSLSocketFactory());
        QString pgmDir = global.getProgramDirPath() + "/certs/verisign_certs.pem";
        sslSocketFactory->loadTrustedCertificates(pgmDir.toStdString().c_str());
        sslSocketFactory->authenticate(true);

        linkedSslSocketNoteStore = sslSocketFactory->createSocket(evernoteHost, 443);
        shared_ptr<TBufferedTransport> bufferedTransport(new TBufferedTransport(linkedSslSocketNoteStore));

        linkedNoteStorePath = "/edam/note/" +book.shardId;
        linkedNoteStoreHttpClient = shared_ptr<TTransport>(new THttpClient(bufferedTransport, evernoteHost, linkedNoteStorePath));

        linkedNoteStoreHttpClient->open();
        shared_ptr<TProtocol> noteStoreProtocol(new TBinaryProtocol(linkedNoteStoreHttpClient));
        linkedNoteStoreClient = shared_ptr<NoteStoreClient>(new NoteStoreClient(noteStoreProtocol));

        linkedNoteStoreClient->authenticateToSharedNotebook(linkedAuthToken, book.shareKey, authToken);

    } catch (EDAMUserException e) {
        QLOG_ERROR() << "EDAMUserException:" << e.what() << endl;
        return false;
    } catch (EDAMSystemException e) {
        QLOG_ERROR() << "EDAMSystemException:" << QString::fromStdString(e.message) << endl;
        return false;
    } catch (TTransportException e) {
        QLOG_ERROR() << "TTransportException:" << e.what() << endl;
        return false;
    }
    QLOG_DEBUG() << "Leaving CommunicationManager::authenticateToLinkedNotebookShard()";
    return true;
}



bool CommunicationManager::getLinkedNotebookSyncState(SyncState &syncState, LinkedNotebook linkedNotebook) {
    try {
        linkedNoteStoreClient->getLinkedNotebookSyncState(syncState, linkedAuthToken.authenticationToken, linkedNotebook);
        QLOG_DEBUG() << "New linked notebook count: "<< syncState.updateCount;
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




bool CommunicationManager::getLinkedNotebookSyncChunk(SyncChunk &chunk, LinkedNotebook linkedNotebook, int start, int chunkSize, bool fullSync) {

    // Get rid of old stuff from last chunk
    while(inkNoteList->size() > 0) {
        QPair<QString, QImage*> *pair = inkNoteList->takeLast();
        delete pair->second;
        delete pair;
    }
    inkNoteList->empty();

    // Try to get the chunk
    try {
        linkedNoteStoreClient->getLinkedNotebookSyncChunk(chunk, authToken, linkedNotebook, start, chunkSize, fullSync);
        for (unsigned int i=0; chunk.__isset.notes && i<chunk.notes.size(); i++) {
            QLOG_DEBUG() << "Fetching chunk item: " << i << ": " << QString::fromStdString(chunk.notes[i].title);
            Note n;
            linkedNoteStoreClient->getNote(n, authToken, chunk.notes[i].guid, true, fullSync, fullSync, fullSync);
            //n.notebookGuid = linkedNotebook.guid;
            chunk.notes[i] = n;
            if (n.__isset.resources && n.resources.size() > 0) {
                checkForInkNotes(n.resources);
            }
        }
        for (unsigned int i=0; chunk.__isset.resources && i<chunk.resources.size(); i++) {
            QLOG_DEBUG() << "Fetching chunk resource item: " << i << ": " << QString::fromStdString(chunk.resources[i].guid);
            Resource r;
            linkedNoteStoreClient->getResource(r, authToken, chunk.resources[i].guid, true, true, true, true);
            chunk.resources[i] = r;
        }
        if (chunk.__isset.resources && chunk.resources.size()>0)
            checkForInkNotes(chunk.resources);
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
        QString pgmDir = global.getProgramDirPath() + "/certs/verisign_certs.pem";
        sslSocketFactory->loadTrustedCertificates(pgmDir.toStdString().c_str());
        sslSocketFactory->authenticate(true);

        sslSocketUserStore = sslSocketFactory->createSocket(evernoteHost, 443);
        shared_ptr<TBufferedTransport> bufferedTransport(new TBufferedTransport(sslSocketUserStore));
        userStoreHttpClient = shared_ptr<TTransport>(new THttpClient(bufferedTransport, evernoteHost, userStorePath));

        userStoreHttpClient->open();
        shared_ptr<TProtocol> iprot(new TBinaryProtocol(userStoreHttpClient));
        userStoreClient = shared_ptr<UserStoreClient>(new UserStoreClient(iprot));
        UserStoreConstants version;
        if (!userStoreClient->checkVersion(clientName, version.EDAM_VERSION_MAJOR, version.EDAM_VERSION_MINOR)) {
                QLOG_ERROR() << "Incompatible Evernote API version";
                return false;
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
    QLOG_DEBUG() << "Leaving CommunicationManager::initUserStore()";
    return true;
}



bool CommunicationManager::initNoteStore() {


    QLOG_DEBUG() << "Inside CommunicationManager::initNoteStore()";

    try {


        shared_ptr<TSSLSocketFactory> sslSocketFactory(new TSSLSocketFactory());
        QString pgmDir = global.getProgramDirPath() + "/certs/verisign_certs.pem";
        sslSocketFactory->loadTrustedCertificates(pgmDir.toStdString().c_str());
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
        QLOG_ERROR() << "\n\nTTransportException:" << e.what() << endl;
        return false;
    }
    QLOG_DEBUG() << "Leaving CommunicationManager::initNoteStore()";
    return true;
}


void CommunicationManager::disconnect() {
    if (noteStoreHttpClient != NULL && noteStoreHttpClient->isOpen()) {
        noteStoreHttpClient->flush();
        noteStoreHttpClient->close();
    }
    if (userStoreHttpClient != NULL && userStoreHttpClient->isOpen()) {
        userStoreHttpClient->flush();
        userStoreHttpClient->close();
    }
    disconnectFromLinkedNotebook();
    initComplete=false;
}

void  CommunicationManager::disconnectFromLinkedNotebook() {
    if (linkedNoteStoreClient != NULL && linkedNoteStoreHttpClient->isOpen()) {
        linkedNoteStoreHttpClient->flush();
        linkedNoteStoreHttpClient->close();
    }
}



bool CommunicationManager::getUserInfo(User &user) {
    QLOG_DEBUG() << "Inside CommunicationManager::getUserInfo";
    try {
       userStoreClient->getUser(user, authToken);
    } catch (EDAMUserException e) {
        QLOG_ERROR() << "EDAMUserException:" << e.errorCode << endl;
        return false;
    } catch (EDAMSystemException e) {
        QLOG_ERROR() << "EDAMSystemException:" << QString::fromStdString(e.message) << endl;
        return false;
    }
    return true;
}



void CommunicationManager::checkForInkNotes(vector<Resource> &resources) {
    for (unsigned int i=0; i<resources.size(); i++) {
        Resource *r = &resources[i];
        if (r->__isset.mime && r->mime == "application/vnd.evernote.ink") {
            downloadInkNoteImage(QString::fromStdString(r->guid), r);
        }
    }
}




void CommunicationManager::downloadInkNoteImage(QString guid, Resource *r) {
    UserTable userTable;
    User u;
    userTable.getUser(u);
    QString urlBase = QString::fromStdString("https://")+QString::fromStdString(evernoteHost)
            +QString("/shard/")
            +QString::fromStdString(u.shardId)
            +QString("/res/")
            +guid +QString(".ink?slice=");
    int sliceCount = 1+((r->height-1)/600);
    QSize size;
    size.setHeight(r->height);
    size.setWidth(r->width);
    QUrl *postData = new QUrl();
    postData->addQueryItem("auth", QString::fromStdString(authToken));

    QEventLoop loop;
    QObject::connect(networkAccessManager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    int position = 0;
    QImage *newImage = NULL;
    for (int i=0; i<sliceCount && position >=0; i++) {
        QUrl url(urlBase+QString::number(i+1));

        QNetworkRequest request(url);

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QNetworkReply *reply = networkAccessManager->post(request,postData->encodedQuery());

        // Execute the event loop here, now we will wait here until readyRead() signal is emitted
        // which in turn will trigger event loop quit.
        loop.exec();
        QImage replyImage;
        replyImage.loadFromData(reply->readAll());
        if (newImage == NULL)
            newImage= new QImage(size, replyImage.format());
        position = inkNoteReady(newImage, &replyImage, position);
        if (position == -1) {
            QLOG_ERROR() << "Error fetching ink note slice " << reply->errorString();
        }
    }
    QPair<QString, QImage*> *newPair = new QPair<QString, QImage*>();
    newPair->first = guid;
    newPair->second = newImage;
    inkNoteList->append(newPair);

    QObject::disconnect(&loop, SLOT(quit()));

}




int CommunicationManager::inkNoteReady(QImage *img, QImage *replyImage, int position) {
    int priorPosition = position;
    position = position+replyImage->height();
    if (!replyImage->isNull()) {
        QPainter p(img);
        p.drawImage(QRect(0,priorPosition, replyImage->width(), position), *replyImage);
        p.end();
        return position;
    }
    return -1;
}

