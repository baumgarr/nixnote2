#ifndef QEVERCLOUD_ASYNCRESULT_H
#define QEVERCLOUD_ASYNCRESULT_H

#include <QObject>
#include "EverCloudException.h"
#include "http.h"

namespace qevercloud {

/**
 * @brief Returned by asynchonous versions of functions.
 *
 * Wait for AsyncResult::finished signal.
 *
 * Intended usage is something like this:
 *
 * @code
NoteStore* ns;
Note note;
...
QObject::connect(ns->createNoteAsync(note), &AsyncResult::finished, [ns](QVariant result, QSharedPointer<EverCloudExceptionData> error) {
    if(!error.isNull()) {
        // do something in case of an error
    } else {
        Note note = result.value<Note>();
        // process returned result
    }
});
 @endcode
 */
class AsyncResult: public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(AsyncResult)

private:
    static QVariant asIs(QByteArray replyData);


public:
    /** @cond HIDDEN_SYMBOLS  */
    typedef QVariant (*ReadFunctionType)(QByteArray replyData);

    AsyncResult(QString url, QByteArray postData, ReadFunctionType readFunction = AsyncResult::asIs, bool autoDelete = true, QObject *parent = 0);
    AsyncResult(QNetworkRequest request, QByteArray postData, ReadFunctionType readFunction = AsyncResult::asIs, bool autoDelete = true, QObject *parent = 0);
    /** @endcond  */

    /**
     * @brief Wait for asyncronous operation to complete.
     * @param timeout
     * Maximum time to wait in milliseconds. Forever if < 0.
     * @return true if finished succesfully, flase in case of the timeout
     */
    bool waitForFinished(int timeout = -1);
signals:
    /**
     * @brief Emitted upon asyncronous call completition.
     * @param result
     * @param error
     * error.isNull() != true in case of an error. See EverCloudExceptionData for more details.
     *
     * AsyncResult deletes itself after emitting this signal. You don't have to manage it's lifetime explicitly.
     */
    void finished(QVariant result, QSharedPointer<EverCloudExceptionData> error);

    /** @cond HIDDEN_SYMBOLS  */

private slots:
    void onReplyFetched(QObject* rp);
    void start();

private:
    QNetworkRequest request_;
    QByteArray postData_;
    ReadFunctionType readFunction_;
    bool autoDelete_;

    /** @endcond  */
};


}

#endif // AQEVERCLOUD_SYNCRESULT_H
