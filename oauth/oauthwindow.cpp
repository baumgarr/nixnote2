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

#include "oauthwindow.h"

#include <stdio.h>
#include <sys/timeb.h>
#include <QSsl>
#include <QSslSocket>
#include <QWebView>
#include <QWebFrame>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QMainWindow>


#include "global.h"

extern Global global;

OAuthWindow::OAuthWindow(QMainWindow *parent) :
    QMainWindow(parent)
{

    tempAuthPage = new QWebView();
    userLoginPage = new QWebView();
    authRequestPage = new QWebView();
    consumerKey = "baumgarr-3523";
    consumerSecret = "8d5ee175f8a5d3ec";
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
            consumerSecret + "%26&oauth_signature_method=PLAINTEXT&oauth_timestamp="+QString::number(time)+
            "&oauth_nonce="+QString::number(millis) +"&oauth_callback=nnoauth";

    permanentCredUrl = "https://"+global.server + "/oauth?oauth_consumer_key=" +consumerKey + "&oauth_signature=" +
            consumerSecret + "%26&oauth_signature_method=PLAINTEXT&oauth_timestamp="+QString::number(time)+
            "&oauth_nonce="+QString::number(millis) +"&oauth_token=";


    // Build the window
    setWindowTitle(tr("Please Grant NixNote Access"));
    setWindowIcon(QIcon(":password.png"));
    //setLayout(&grid);
    //grid.addWidget(userLoginPage);
    setCentralWidget(userLoginPage);

    error = false;
    errorMessage = "";

    // Check that SSL sockets are supported
    if (!QSslSocket::supportsSsl()) {
        errorMessage = tr("SSL Support not found.  Aborting connection");
        error = true;
        close();
        return;
    }


    // Turn on TLS (sometimes it isn't on by default)
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1);
    config.setProtocol(QSsl::SslV3);
    QSslConfiguration::setDefaultConfiguration(config);

    // Since this page loads async, we need flags to be sure we don't load something twice
    authTokenReceived = false;
    userLoginPageLoaded = false;


    // Load the temporary URL to start the authentication procesess.  When
    // finished, this QWebView will contain the URL to start the
    // authentication process.
    QUrl tu(temporaryCredUrl);
    connect(tempAuthPage, SIGNAL(loadFinished(bool)), this, SLOT(tempAuthPageLoaded(bool)));
    connect(tempAuthPage->page()->networkAccessManager(),SIGNAL(finished(QNetworkReply*)), this, SLOT(tempAuthPageReply(QNetworkReply*)));
    tempAuthPage->load(tu);
}


void OAuthWindow::reset() {
    authTokenReceived = false;
    userLoginPageLoaded = false;
    error = false;
    errorMessage = "";

    // Load the temporary URL to start the authentication procesess.  When
    // finished, this QWebView will contain the URL to start the
    // authentication process.
    QUrl tu(temporaryCredUrl);
    connect(tempAuthPage, SIGNAL(loadFinished(bool)), this, SLOT(tempAuthPageLoaded(bool)));
    connect(tempAuthPage->page()->networkAccessManager(),SIGNAL(finished(QNetworkReply*)), this, SLOT(tempAuthPageReply(QNetworkReply*)));
    tempAuthPage->load(tu);
}


void OAuthWindow::tempAuthPageLoaded(bool rc) {
    QLOG_DEBUG() << "Temporary credentials received from Evernote";
    if (!rc) {
        errorMessage = tr("Error receiving temporary credentials");
        error = true;
        QWebFrame *mainFrame = tempAuthPage->page()->mainFrame();
        QString contents = mainFrame->toHtml();
        QLOG_DEBUG() << "Reply contents:" << contents;
        emit closed();
        close();
        return;
    }

    QWebFrame *mainFrame = tempAuthPage->page()->mainFrame();
    QString contents = mainFrame->toPlainText();
//    QLOG_DEBUG() << "Temporary Cred Contents: " << contents;
    int index = contents.indexOf("&oauth_token_secret");
    contents = contents.left(index);
    QUrl accessUrl(urlBase+"/OAuth.action?" +contents);

    connect(userLoginPage->page()->networkAccessManager(),SIGNAL(finished(QNetworkReply*)),this,SLOT(userLoginReply(QNetworkReply*)));
    userLoginPage->load(accessUrl);
    //grid.addWidget(userLoginPage);
}


void OAuthWindow::tempAuthPageReply(QNetworkReply* reply) {
    QLOG_DEBUG() << "error: " << reply->error();
    if (reply->error() != QNetworkReply::NoError) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QLOG_DEBUG() << "status:" << statusCode;
//        QLOG_DEBUG() << "error: " << reply->error();
        return;
    }
}



void OAuthWindow::permanentCredentialsReceived(bool rc) {
    if (authTokenReceived)
        return;
    QWebFrame *mainFrame;
    mainFrame = authRequestPage->page()->mainFrame();
    QString contents = mainFrame->toPlainText();


    if (!rc) {
        errorMessage = tr("Error receiving permanent credentials");
        QLOG_DEBUG() << "Bad return code while receiveng permanent credentials";
        error = true;
        emit closed();
        close();
    }

    if (contents.startsWith("oauth_token=S%3D")) {
        QLOG_DEBUG() << "Permanent Auth Response: " << contents;
        QLOG_DEBUG() << "Permanent credentials received received from Evernote";
        authTokenReceived = true;
        QLOG_DEBUG() << "Good authorization token received.";
        QString decoded;
        QByteArray enc;
        enc.append(contents);
        decoded = QUrl::fromEncoded(enc).toString();
        response = decoded;
        userLoginPage->disconnect(this);
        tempAuthPage->disconnect(this);
        authRequestPage->disconnect(this);
        error = false;
        errorMessage = "";
        emit closed();
        close();
    }
}




void OAuthWindow::userLoginReply(QNetworkReply *reply) {
    if (userLoginPageLoaded)
        return;
//    QLOG_DEBUG() << "Authentication reply received from Evernote";
//    QLOG_DEBUG() << "error: " << reply->error();
    QString searchReq = "?oauth_token=";
//    QLOG_DEBUG() << "Reply:" << reply->url().toString();

    int pos = reply->url().toString().indexOf(searchReq);
    if (pos>0) {
        QString token = reply->url().toString();
        token = token.mid(pos+searchReq.length());
        if (token.indexOf("auth_verifier") <= 0) {
            errorMessage = tr("Error receiving authorization");
            error = true;
            return;
        }

        if (reply->isFinished()) {
            QLOG_DEBUG() << "Loading URL";
            QLOG_DEBUG() << "Permanent URL: " << permanentCredUrl;
            QLOG_DEBUG() << "Token: " << token;
            connect(authRequestPage, SIGNAL(loadFinished(bool)), this, SLOT(permanentCredentialsReceived(bool)));
            authRequestPage->load(QUrl(permanentCredUrl+token));
            userLoginPageLoaded = true;
        }
    }
}






