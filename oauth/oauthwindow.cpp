#include "oauthwindow.h"

#include <stdio.h>
#include <sys/timeb.h>
#include <QSsl>
#include <QSslSocket>
#include <QWebView>
#include <QWebFrame>
#include <QNetworkReply>

#include "global.h"

extern Global global;

OAuthWindow::OAuthWindow(QWidget *parent) :
    QDialog(parent)
{

    consumerKey = "baumgarr";
    consumerSecret = "60d4cdedb074b0ac";
    urlBase = "https://" +global.server;
    requestTokenUrl = urlBase + "/oauth";
    accessTokenUrl = urlBase + "/oauth";
    authorizationUrlBase = urlBase + "/OAuth.action";
    callbackUrl = "index.jsp?action=callbackReturn";

    struct timeb tmb;

    ftime(&tmb);
    int time = tmb.time;
    int millis = tmb.millitm;


    // Create the URLs needed for authentication with Evernote
    temporaryCredUrl = "https://"+global.server + "/oauth?oauth_consumer_key=" +consumerKey + "&oauth_signature=" +
            consumerSecret + "%26&oauth_signature_method=PLAINTEXT&oauth_timestamp="+QString(time)+
            "&oauth_nonce="+QString(millis) +"&oauth_callback=nnoauth";

    permanentCredUrl = "https://"+global.server + "/oauth?oauth_consumer_key=" +consumerKey + "&oauth_signature=" +
            consumerSecret + "%26&oauth_signature_method=PLAINTEXT&oauth_timestamp="+QString(time)+
            "&oauth_nonce="+QString(millis) +"&oauth_token=";


    // Build the window
    setWindowTitle(tr("Please Grant Nixnote Access"));
    setWindowIcon(QIcon(":password.png"));
    setLayout(&grid);
    grid.addWidget(&authPage);
    connect(&tempPage, SIGNAL(loadFinished(bool)), this, SLOT(temporaryCredentialsReceived(bool)));

    error = false;
    errorMessage = "";

    // Check that SSL sockets are supported
    if (!QSslSocket::supportsSsl()) {
        errorMessage = tr("SSL Support not found.  Aborting connection");
        error = true;
        close();
        return;
    }

    // Load the temporary URL to start the authentication procesess.  When
    // finished, this QWebView will contain the URL to start the
    // authentication process.
    QUrl tu(temporaryCredUrl);
    tempPage.load(tu);
}


void OAuthWindow::temporaryCredentialsReceived(bool rc) {
    QLOG_DEBUG() << "Temporary credentials received from Evernote";
    if (!rc) {
        errorMessage = tr("Error receiving temporary credentials");
        error = true;
        close();
        return;
    }


    QWebFrame *mainFrame = tempPage.page()->mainFrame();
    QString contents = mainFrame->toPlainText();
    int index = contents.indexOf("&oauth_token_secret");
    contents = contents.left(index);
    QUrl accessUrl(urlBase+"/OAuth.action?" +contents);

    connect(authPage.page()->networkAccessManager(),SIGNAL(finished(QNetworkReply*)),this,SLOT(replyReceived(QNetworkReply*)));

    authPage.load(accessUrl);
    grid.addWidget(&authPage);
}


void OAuthWindow::tokenFound(QString token) {
    QLOG_DEBUG() << "OAuth Token Found: " +token;
    if (token.indexOf("auth_verifier") <= 0) {
        errorMessage = tr("Error receiving authorization");
        error = true;
        close();
        return;
    }

    connect(&tokenPage, SIGNAL(loadFinished(bool)), this, SLOT(permanentCredentialsReceived(bool)));
    QLOG_DEBUG() << "Loading URL";
    tokenPage.load(QUrl(permanentCredUrl+token));
    QLOG_DEBUG() << "Leaving oauthwindow::tokenfound";
}


void OAuthWindow::permanentCredentialsReceived(bool rc) {
    QLOG_DEBUG() << "Permanent credentials received received from Evernote";
    rc=rc; // suppress unused;
//    if (!rc) {
//        errorMessage = tr("Error receiving permanent credentials");
//        error = true;
//        close();
//    }

    QWebFrame *mainFrame;
    mainFrame = tokenPage.page()->mainFrame();
    QString contents = mainFrame->toPlainText();
    QLOG_DEBUG() << "Response: " << contents;
    if (contents.startsWith("oauth_token=S%3D")) {
        QString decoded;
        QByteArray enc;
        enc.append(contents);
        decoded = QUrl::fromEncoded(enc).toString();
        response = decoded;
        close();
    }
}

void OAuthWindow::replyReceived(QNetworkReply *reply) {
    QLOG_DEBUG() << "Authentication reply received from Evernote";
    QString searchReq = "nnoauth?oauth_token=";
    QString k = reply->url().toString();
    int pos = reply->url().toString().indexOf(searchReq);
    if (pos>0) {
        QString token = reply->url().toString();
        token = token.mid(pos+searchReq.length());
        tokenFound(token);
    }
}


