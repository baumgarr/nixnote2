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

#ifndef OAUTHWINDOW_H
#define OAUTHWINDOW_H

#include <QDialog>
#include <QWebView>
#include <QGridLayout>
#include <QNetworkRequest>
#include <QMainWindow>

class OAuthWindow : public QMainWindow
{
    Q_OBJECT
private:
    QString iconPath;
    QGridLayout grid;
    QString consumerKey;
    QString consumerSecret;
    QString temporaryCredUrl;
    QString permanentCredUrl;
    QString urlBase;
    QString requestTokenUrl;
    QString accessTokenUrl;
    QString authorizationUrlBase;
    QString callbackUrl;
    bool authTokenReceived;
    bool userLoginPageLoaded;
    QWebView *tempAuthPage;
    QWebView *userLoginPage;
    QWebView *authRequestPage;

public:
    explicit OAuthWindow(QMainWindow *parent = 0);
    QString response;
    bool error;
    QString errorMessage;
    
signals:
    void permanentTokenFound(QString token);
    void closed();

public slots:
    void tempAuthPageLoaded(bool rc);
    void permanentCredentialsReceived(bool rc);
    void userLoginReply(QNetworkReply *reply);
    void tempAuthPageReply(QNetworkReply *reply);

};

#endif // OAUTHWINDOW_H
