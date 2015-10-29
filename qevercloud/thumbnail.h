#ifndef QEVERCLOUD_THUMBNAIL_H
#define QEVERCLOUD_THUMBNAIL_H

#include "generated/types.h"
#include <QByteArray>
#include <QString>
#include <QNetworkAccessManager>
#include "AsyncResult.h"

namespace qevercloud {

/**
 * @brief The class is for downloading thumbnails for notes and resources from Evernote servers.
 *
 * This thumbnails are available not with general EDAM Thrift interface as explained in the <a href="http://dev.evernote.com/doc/articles/thumbnails.php">documentation.</a>
 *
 * Usage:
@code

Thumbnail thumb("www.evernote.com", sharId, authenticationToken);
QByteArray pngImage = thumb.download(noteGuid);

@endcode
 *
 * By defualt 300x300 PNG images are requested.
 */
class Thumbnail {
public:
    /**
     * Specifies image type of the returned thumbnail.
     *
     * Can be PNG, JPEG, GIF or BMP.
     */
    struct ImageType {
        enum type {PNG, JPEG, GIF, BMP};
    };

    /**
     * @brief Default constructor.
     *
     * host, shardId, authenticationToken have to be specified before calling @link download @endlink or @link createPostRequest @endlink
     */
    Thumbnail(): size_(300), imageType_(ImageType::PNG) {}

    /**
     * @brief Constructs Thumbnail.
     * @param host
     * www.evernote.com or sandbox.evernote.com
     * @param shardId
     * You can get the value from UserStore service or as a result of an authentication.
     * @param authenticationToken
     * For working privite notes/resources you must supply a valid authentication token. For public resources the value specified is not used.
     * @param size
     * The size of the thumbnail. Evernote supports values from from 1 to 300. By defualt 300 is used.
     * @param imageType
     * Thumbnail image type. See ImageType. By default PNG is used.
     */
    Thumbnail(QString host, QString shardId, QString authenticationToken, int size = 300, ImageType::type imageType = ImageType::PNG)
        : host_(host), shardId_(shardId), authenticationToken_(authenticationToken), size_(size), imageType_(imageType) {}

    /**
     * @param host
     * www.evernote.com or sandbox.evernote.com
     */
    Thumbnail& setHost(QString host);

    /**
     * @param shardId
     * You can get the value from UserStore service or as a result of an authentication.
     */
    Thumbnail& setShardId(QString shardId);

    /**
     * @param authenticationToken
     * For working privite notes/resources you must supply a valid authentication token. For public resources the value specified is not used.
     */
    Thumbnail& setAuthenticationToken(QString authenticationToken);

    /**
     * @param size
     * The size of the thumbnail. Evernote supports values from from 1 to 300. By defualt 300 is used.
     */
    Thumbnail& setSize(int size);

    /**
     * @param imageType
     * Thumbnail image type. See ImageType. By default PNG is used.
     */
    Thumbnail& setImageType(ImageType::type imageType);

    /**
     * @brief Downloads the thumbnail for a resource or a note.
     * @param guid
     * The note or resource guid
     * @param isPublic
     * Specify true for public notes/resources. In this case authentication token is not sent to
     * with the request as it shoud be according to the docs.
     * @param isResourceGuid
     * true if guid denotes a resource and false if it denotes a note.
     * @return downloaded data.
     *
     */
    QByteArray download(Guid guid, bool isPublic = false, bool isResourceGuid = false);

    /** Asynchronous version of @link download @endlink function*/
    AsyncResult* downloadAsync(Guid guid, bool isPublic = false, bool isResourceGuid = false);

    /**
     * @brief Prepares a POST request for a thumbnail download.
     * @param guid
     * The note or resource guid
     * @param isPublic
     * Specify true for public notes/resources. In this case authentication token is not sent to
     * with the request as it shoud be according to the docs.
     * @param isResourceGuid
     * true if guid denotes a resource and false if it denotes a note.
     * @return a pair of QNetworkRequest for the POST request and data that must be posted with the request.
     */
    QPair<QNetworkRequest, QByteArray> createPostRequest(qevercloud::Guid guid, bool isPublic = false, bool isResourceGuid = false);

private:
    QString host_;
    QString shardId_;
    QString authenticationToken_;
    int size_;
    ImageType::type imageType_;
};


}

#endif // THUMBNAIL_H
