#ifndef QEVERCLOUD_OAUTH_H
#define QEVERCLOUD_OAUTH_H

#include "./generated/types.h"
#include <QWebView>
#include <QDialog>

/** @page oauth_include Reasons for QEverCloudOAuth.h header file
 *
 * Using EvernoteOAuthDialog or EvernoteOAuthWebView implies the use of QtWidgets and QtWebkit modules so you have to use them in your app:
@code
# Somewhere in your Qt 5 .pro file
QT += widgets webkitwidgets
@endcode
 *
 * Other parts of QEverCloud depend only on QtCore and QtNetwork modules so all OAuth functionality is moved into a separate inculde file
 * named QEverCloudOAuth.h. If you do not use the provided OAuth functionality (say, you write a small app for personal use which relies on developer tokens only)
 * and do not include the header then your app will not depend on the QtWidgets and QtWebkit modules.
 *
 */


namespace qevercloud {

/**
 * @brief Sets the function to use for nonce generation for OAuth authentication.
 *
 * The default algorithm uses qrand() so do not forget to call qsrand() in your application!
 *
 * qrand() is not guaranteed to be cryptographically strong. I try to amend the fact by using
 *  QUuid::createUuid() which uses /dev/urandom if it's availabe. But this is no guarantee either.
 * So if you want total control over nonce generation you can write you own algorithm.
 *
 * setNonceGenerator is NOT thread safe.
 */
void setNonceGenerator(quint64 (*nonceGenerator)());

/**
 * @brief The class is tailored specifically for OAuth authorization with Evernote.
 *
 * While it is functional by itself you probably will prefer to use EvernoteOAuthDialog.
 *
 * %Note that you have to include @link oauth_include QEverCloudOAuth.h header@endlink.
 *
 * By deafult EvernoteOAuthWebView uses qrand() for generating nonce so do not forget to call qsrand()
 * in your application. See @link setNonceGenerator @endlink If you want more control over nonce generation.
 */
class EvernoteOAuthWebView: public QWebView {
    Q_OBJECT
public:
    EvernoteOAuthWebView(QWidget* parent = 0);

    /**
     * This function starts the OAuth sequence. In the end of the sequence will be emitted one of the signals: authenticationSuceeded or authenticationFailed.
     *
     * Do not call the function while its call is in effect, i.e. one of the signals is not emitted.
     *
     * @param host
     * Evernote host to authorize with. You need one of this:
     * <ul>
     * <li>"www.evernote.com" - the production service. It's the default value.</li>
     * <li>"sandox.evernote.com" - the developers "sandbox" service</li>
     * </ul>
     * @param consumerKey
     * get it <a href="http://dev.evernote.com/doc/">from the Evernote</a>
     * @param consumerSecret
     * along with this
     */
    void authenticate(QString host, QString consumerKey, QString consumerSecret);

    /** @return true if the last call to authenticate resulted in a successful authentication. */
    bool isSucceeded() {return isSucceeded_;}

    /** @return error message resulted from the last call to authenticate */
    QString oauthError() {return errorText_;}

    /** Holds data that is returned by Evernote on a succesful authentication */
    struct OAuthResult {
        QString noteStoreUrl; ///< note store url for the user; no need to question UserStore::getNoteStoreUrl for it.
        Timestamp expires; ///< authenticationToken time of expiration.
        QString shardId; ///< usually is not used
        UserID userId; ///< same as PublicUserInfo::userId
        QString webApiUrlPrefix; ///< see PublicUserInfo::webApiUrlPrefix
        QString authenticationToken; ///< This is what this all was for!
    };

    /** @returns the result of the last authentication, i.e. authenticate() call.*/
    OAuthResult oauthResult() {return oauthResult_;}

    /** The method is useful to specify default size for a EverOAuthWebView. */
    void setSizeHint(QSize sizeHint);

    QSize sizeHint() const {return sizeHint_;}

signals:
    /** Emitted when the OAuth sequence started with authenticate() call is finished */
    void authenticationFinished(bool success);

    /** Emitted when the OAuth sequence is succesfully finished. Call oauthResult() to get the data.*/
    void authenticationSuceeded();

    /** Emitted when the OAuth sequence is finished with a failure. Some error info may be availabe with errorText().*/
    void authenticationFailed();

private slots:
    void temporaryFinished(QObject* rf);
    void permanentFinished(QObject* rf);
    void onUrlChanged(const QUrl& url);
    void clearHtml();


private:
    bool isSucceeded_;
    QSize sizeHint_;
    QString errorText_;
    QString oauthUrlBase_;
    QString host_;
    OAuthResult oauthResult_;
    void setError(QString errorText);
};

/**
 * @brief Authorizes your app with the Evernote service by means of OAuth authentication.
 *
 * Intended usage:
 *
@code
#include <QEverCloudOAuth.h>

OAuthDialog d(myConsumerKey, myConsumerSecret);
if(d.exec() == QDialog::Accepted) {
    OAuthDialog::OAuthResult res = d.oauthResult();
    // Connect to Evernote
    ...
} else {
    QString errorText = d.oauthError();
    // handle an authentication error
    ...
}

@endcode
 *
 * %Note that you have to include @link oauth_include QEverCloudOAuth.h header@endlink.
 *
 * By deafult EvernoteOAuthDialog uses qrand() for generating nonce so do not forget to call qsrand()
 * in your application. See @link setNonceGenerator @endlink If you want more control over nonce generation.
 */

class EvernoteOAuthDialog: public QDialog {
public:
    typedef EvernoteOAuthWebView::OAuthResult OAuthResult;

    /** Constructs the dialog.
     *
     * @param host
     * Evernote host to authorize with. You need one of this:
     * <ul>
     * <li>"www.evernote.com" - the production service. It's the default value.</li>
     * <li>"sandox.evernote.com" - the developers "sandbox" service</li>
     * </ul>
     * @param consumerKey
     * get it <a href="http://dev.evernote.com/doc/">from the Evernote</a>
     * @param consumerSecret
     * along with this
    */
    EvernoteOAuthDialog(QString consumerKey, QString consumerSecret, QString host = "www.evernote.com", QWidget* parent = 0);
    ~EvernoteOAuthDialog();

    /**
     * The dialog adjusts its initial size automatically based on the conatined QWebView preffered size.
     * Use this method to set the size.
     *
     * @param sizeHint will be used as the preffered size of the contained QWebView.
     */
    void setWebViewSizeHint(QSize sizeHint) {webView_->setSizeHint(sizeHint);}

    /** @return true in case of a succesful authentication.
     * You probably better chech exec() return value instead.
     */
    bool isSucceeded() {return webView_->isSucceeded();}

    /**
     * @return In case of an authentification error may return some information about the error.
     */
    QString oauthError() {return webView_->oauthError();}

    /**
     * @return the result of a succesful authentication.
     */
    OAuthResult oauthResult() {return webView_->oauthResult();}

    /**
     * @return
     *   QDialog::Accepted on a succesful authentication.
     */
    int exec();

    /** Shows the dialog as a window modal dialog, returning immediately.
     */
    void open();

private:
   EvernoteOAuthWebView* webView_;
   QString host_;
   QString consumerKey_;
   QString consumerSecret_;
};


}

#endif // QEVERCLOUD_OAUTH_H
