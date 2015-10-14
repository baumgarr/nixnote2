#include "oauth.h"
#include "http.h"
#include <QVBoxLayout>
#include <QNetworkReply>

#ifdef USE_QT_WEB_ENGINE
#include <QWebEngineHistory>
#else
#include <QWebSettings>
#include <QWebHistory>
#endif

#include <QUuid>
#include <cstring>

namespace {

    quint64 random64()
    {
        quint64 res = 0;
        for(int i = 0; i < 8; i++) {
            res += static_cast<quint64>(qrand() % 256) << i*8;
        }

        QByteArray randomData = QUuid::createUuid().toRfc4122();
        quint64 random;
        std::memcpy(&random, &randomData.constData()[0], sizeof(random));
        res ^= random;
        std::memcpy(&random, &randomData.constData()[sizeof(random)], sizeof(random));
        res ^= random;

        return res;
    }

    typedef quint64 (*NonceGenerator)();
    NonceGenerator nonceGenerator_ = random64;

    NonceGenerator nonceGenerator() {return nonceGenerator_;}
}

void setNonceGenerator(quint64 (*nonceGenerator)())
{
    nonceGenerator_ = nonceGenerator;
}


qevercloud::EvernoteOAuthWebView::EvernoteOAuthWebView(QWidget *parent)
#ifdef USE_QT_WEB_ENGINE
    : QWebEngineView(parent), isSucceeded_(false)
#else
    : QWebView(parent), isSucceeded_(false)
#endif
{
#ifndef USE_QT_WEB_ENGINE
    page()->setNetworkAccessManager(evernoteNetworkAccessManager());
#endif
}

void qevercloud::EvernoteOAuthWebView::setSizeHint(QSize sizeHint)
{
    sizeHint_ = sizeHint;
    updateGeometry();
}


void qevercloud::EvernoteOAuthWebView::authenticate(QString host, QString consumerKey, QString consumerSecret)
{
    host_ = host;
    isSucceeded_ = false;
    this->setHtml("");
    this->history()->clear();

    qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;
    qint64 nonce = nonceGenerator()();
    oauthUrlBase_ = QStringLiteral("https://%1/oauth?oauth_consumer_key=%2&oauth_signature=%3&oauth_signature_method=PLAINTEXT&oauth_timestamp=%4&oauth_nonce=%5")
            .arg(host).arg(consumerKey).arg(consumerSecret).arg(timestamp).arg(nonce);

    // step 1: acquire temporary token
    ReplyFetcher* replyFetcher = new ReplyFetcher();
    connect(replyFetcher, QEC_SIGNAL(ReplyFetcher,replyFetched,qevercloud::ReplyFetcher*),
            this, QEC_SLOT(EvernoteOAuthWebView,temporaryFinished,QObject*));
    QUrl url(oauthUrlBase_ + QStringLiteral("&oauth_callback=nnoauth"));
#ifdef USE_QT_WEB_ENGINE
    replyFetcher->start(evernoteNetworkAccessManager(), url);
#else
    replyFetcher->start(page()->networkAccessManager(), url);
#endif
}

void qevercloud::EvernoteOAuthWebView::temporaryFinished(QObject *rf)
{
    ReplyFetcher* replyFetcher = qobject_cast<ReplyFetcher*>(rf);
    if(replyFetcher->isError()) {
        setError(replyFetcher->errorText());
    } else {
        QString reply = QString(replyFetcher->receivedData());
        int index = reply.indexOf(QStringLiteral("&oauth_token_secret"));
        QString token = reply.left(index);

        // step 2: directing a user to the login page
        connect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
        QUrl loginUrl(QStringLiteral("https://%1//OAuth.action?%2").arg(host_).arg(token));
        this->setUrl(loginUrl);
    }
    replyFetcher->deleteLater();
}

void qevercloud::EvernoteOAuthWebView::onUrlChanged(const QUrl &url)
{
    // step 3: catch the rediret to our callback url (nnoauth)
    QString s = url.toString();
    QString oauthMarker = QStringLiteral("?oauth_token=");
    if(s.contains(QStringLiteral("nnoauth?")) && s.contains(oauthMarker)) {
        if(s.contains(QStringLiteral("&oauth_verifier="))) { // success
            QString token = s.mid(s.indexOf(oauthMarker) + oauthMarker.length());

            // step 4: acquire permanent token
            ReplyFetcher* replyFetcher = new ReplyFetcher();
            connect(replyFetcher, QEC_SIGNAL(ReplyFetcher,replyFetched,qevercloud::ReplyFetcher*),
                    this, QEC_SLOT(EvernoteOAuthWebView,permanentFinished,QObject*));
            QUrl url(oauthUrlBase_ + QStringLiteral("&oauth_token=%1").arg(token));
#ifdef USE_QT_WEB_ENGINE
            replyFetcher->start(evernoteNetworkAccessManager(), url);
#else
            replyFetcher->start(page()->networkAccessManager(), url);
#endif
        } else {
            setError(QStringLiteral("Authentification failed."));
        }
        disconnect(this, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
        QMetaObject::invokeMethod(this, "clearHtml", Qt::QueuedConnection);
    }
}


void qevercloud::EvernoteOAuthWebView::permanentFinished(QObject *rf)
{
    ReplyFetcher* replyFetcher = qobject_cast<ReplyFetcher*>(rf);
    if(replyFetcher->isError()) {
        setError(replyFetcher->errorText());
    } else {
        isSucceeded_ = true;

        QByteArray reply = replyFetcher->receivedData();
        QMap<QString, QString> params;
        QList<QByteArray> vals = reply.split('&');
        for(int i = 0; i < vals.length(); i++) {
            QString decoded = QUrl::fromPercentEncoding(vals[i]);
            int pos = decoded.indexOf('=');
            params[decoded.left(pos).trimmed()] = decoded.mid(pos + 1);
        }
        oauthResult_.noteStoreUrl = params[QStringLiteral("edam_noteStoreUrl")];
        oauthResult_.expires = Timestamp(params[QStringLiteral("edam_expires")].toLongLong());
        oauthResult_.shardId = params[QStringLiteral("edam_shard")];
        oauthResult_.userId = params[QStringLiteral("edam_userId")].toInt();
        oauthResult_.webApiUrlPrefix = params[QStringLiteral("edam_webApiUrlPrefix")];
        oauthResult_.authenticationToken = params[QStringLiteral("oauth_token")];

        emit authenticationFinished(true);
        emit authenticationSuceeded();
    }
    replyFetcher->deleteLater();
}

void qevercloud::EvernoteOAuthWebView::setError(QString errorText)
{
    isSucceeded_ = false;
    this->setHtml("");
    this->history()->clear();
    errorText_ = errorText;
    emit authenticationFinished(false);
    emit authenticationFailed();
}

void qevercloud::EvernoteOAuthWebView::clearHtml()
{
    setHtml("");
}



qevercloud::EvernoteOAuthDialog::EvernoteOAuthDialog(QString consumerKey, QString consumerSecret, QString host, QWidget *parent)
    : QDialog(parent), host_(host), consumerKey_(consumerKey), consumerSecret_(consumerSecret)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    webView_ = new EvernoteOAuthWebView(this);
    connect(webView_, SIGNAL(authenticationSuceeded()), this, SLOT(accept()), Qt::QueuedConnection);
    connect(webView_, SIGNAL(authenticationFailed()), this, SLOT(reject()), Qt::QueuedConnection);

    QVBoxLayout* layout_ = new QVBoxLayout;
    layout_->addWidget(webView_);
    setLayout(layout_);
    webView_->setSizeHint(QSize(768,576));
    adjustSize();
}

qevercloud::EvernoteOAuthDialog::~EvernoteOAuthDialog()
{
#ifndef USE_QT_WEB_ENGINE
    QWebSettings::clearMemoryCaches();
#endif
}

int qevercloud::EvernoteOAuthDialog::exec()
{
    webView_->authenticate(host_, consumerKey_, consumerSecret_);
    return QDialog::exec();
}

void qevercloud::EvernoteOAuthDialog::open()
{
    QDialog::open();
    webView_->authenticate(host_, consumerKey_, consumerSecret_);
}

