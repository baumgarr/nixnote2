#ifndef OAUTHWINDOW_H
#define OAUTHWINDOW_H

#include <QDialog>
#include <QWebView>
#include <QGridLayout>

class OAuthWindow : public QDialog
{
    Q_OBJECT
private:
    QString iconPath;
    QWebView tempPage;
    QWebView authPage;
    QWebView tokenPage;
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
    void tokenFound(QString token);

public:
    explicit OAuthWindow(QWidget *parent = 0);
    QString response;
    bool error;
    QString errorMessage;
    
signals:
    void permanentTokenFound(QString token);
    
public slots:
    void temporaryCredentialsReceived(bool rc);
    void permanentCredentialsReceived(bool rc);
    void replyReceived(QNetworkReply *reply);

};

#endif // OAUTHWINDOW_H
