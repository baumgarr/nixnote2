
#include "generated/services.h"

#include <QUrl>

/**
 * @brief Constructs UserStore object.
 * @param host
 *   www.evernote.com or sandbox.evernote.com
 * @param authenticationToken
 *   This token that will be used as the default token.
 */
qevercloud::UserStore::UserStore(QString host, QString authenticationToken, QObject *parent): QObject(parent)
{
    QUrl url;
    url.setScheme(QStringLiteral("https"));
    url.setHost(host);
    url.setPath("/edam/user");
    url_ = url.toString(QUrl::StripTrailingSlash);
    setAuthenticationToken(authenticationToken);
}

/**
 * Constructs NoteStore object.
 * @param noteStoreUrl
 * EDAM NoteStore service url. In general it's different for different users.
 * @param authenticationToken
 *  This token that will be used as the default token.
 *
 */
qevercloud::NoteStore::NoteStore(QString noteStoreUrl, QString authenticationToken, QObject *parent): QObject(parent)
{
    setNoteStoreUrl(noteStoreUrl);
    setAuthenticationToken(authenticationToken);
}

/**
 * Constructs NoteStore object.
 *
 * noteStoreUrl and possibly authenticationToken are expected to be specified later.
 */
qevercloud::NoteStore::NoteStore(QObject *parent): QObject(parent)
{

}

/** @fn qevercloud::UserStore::setAuthenticationToken
 * Sets a value that will be used as the default token.
 * */

/** @fn qevercloud::UserStore::authenticationToken
 * @returns the default authentication token value.
 * */

/** @fn qevercloud::NoteStore::setAuthenticationToken
 * Sets a value that will be used as the default token.
 * */

/** @fn qevercloud::NoteStore::authenticationToken
 * @returns the default authentication token value.
 * */

/** @fn qevercloud::NoteStore::setNoteStoreUrl
 * Sets a value that will be used as EDAM NoteStore service url by this object.
 * */

/** @fn qevercloud::NoteStore::authenticationToken
 * @returns EDAM NoteStore service url that is used by this NoteStore object.
 * */
