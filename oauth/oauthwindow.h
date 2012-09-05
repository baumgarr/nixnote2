#ifndef OAUTHWINDOW_H
#define OAUTHWINDOW_H

#include <QDialog>
#include <QWebView>
#include <QGridLayout>
#include <QNetworkRequest>

class OAuthWindow : public QDialog
{
    Q_OBJECT
private:
    QString iconPath;
    QWebView tempAuthPage;
    QWebView userLoginPage;
    QWebView authRequestPage;
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

public:
    explicit OAuthWindow(QWidget *parent = 0);
    QString response;
    bool error;
    QString errorMessage;
    
signals:
    void permanentTokenFound(QString token);
    
public slots:
    void tempAuthPageLoaded(bool rc);
    void permanentCredentialsReceived(bool rc);
    void userLoginReply(QNetworkReply *reply);
    void tempAuthPageReply(QNetworkReply *reply);

};

#endif // OAUTHWINDOW_H
