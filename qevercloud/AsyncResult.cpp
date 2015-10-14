
#include "AsyncResult.h"
#include <QEventLoop>
#include <QSignalMapper>
#include <typeinfo>
#include "http.h"
#include "EventLoopFinisher.h"
#include "qt4helpers.h"

QVariant qevercloud::AsyncResult::asIs(QByteArray replyData)
{
    return replyData;
}

qevercloud::AsyncResult::AsyncResult(QString url, QByteArray postData, qevercloud::AsyncResult::ReadFunctionType readFunction, bool autoDelete, QObject *parent)
    : QObject(parent), request_(createEvernoteRequest(url)), postData_(postData), readFunction_(readFunction), autoDelete_(autoDelete)
{
    QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
}

qevercloud::AsyncResult::AsyncResult(QNetworkRequest request, QByteArray postData, qevercloud::AsyncResult::ReadFunctionType readFunction, bool autoDelete, QObject *parent)
 : QObject(parent), request_(request), postData_(postData), readFunction_(readFunction), autoDelete_(autoDelete)
{
    QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
}

bool qevercloud::AsyncResult::waitForFinished(int timeout)
{
    QEventLoop loop;
    QObject::connect(this, SIGNAL(finished(QVariant,QSharedPointer<EverCloudExceptionData>)), &loop, SLOT(quit()));
    if(timeout >= 0) {
        QTimer timer;
        EventLoopFinisher finisher(&loop, 1);
        connect(&timer, SIGNAL(timeout()), &finisher, SLOT(stopEventLoop()));
        timer.setSingleShot(true);
        timer.setInterval(timeout);
        timer.start();
        return loop.exec(QEventLoop::ExcludeUserInputEvents) == 0;
    } else {
        return loop.exec(QEventLoop::ExcludeUserInputEvents) == 0;
    }
}

void qevercloud::AsyncResult::start()
{
    ReplyFetcher* f = new ReplyFetcher;
    QObject::connect(f, QEC_SIGNAL(ReplyFetcher,replyFetched,qevercloud::ReplyFetcher*),
                     this, QEC_SLOT(AsyncResult,onReplyFetched,QObject*));
    f->start(evernoteNetworkAccessManager(), request_, postData_);
}

void qevercloud::AsyncResult::onReplyFetched(QObject *rp)
{
    ReplyFetcher* reply = qobject_cast<ReplyFetcher*>(rp);
    QSharedPointer<EverCloudExceptionData> error;
    QVariant result;
    try {
        if(reply->isError()) {
            error = QSharedPointer<EverCloudExceptionData>(new EverCloudExceptionData(reply->errorText()));
        } else if(reply->httpStatusCode() != 200) {
            error = QSharedPointer<EverCloudExceptionData>(new EverCloudExceptionData(QStringLiteral("HTTP Status Code = %1").arg(reply->httpStatusCode())));
        } else {
            result = readFunction_(reply->receivedData());
        }
    } catch(const EverCloudException& e) {
        error = e.exceptionData();
    } catch(const std::exception& e) {
        error = QSharedPointer<EverCloudExceptionData>(new EverCloudExceptionData(QStringLiteral("Exception of type \"%1\" with the message: %2").arg(typeid(e).name()).arg(e.what())));
    } catch(...) {
        error = QSharedPointer<EverCloudExceptionData>(new EverCloudExceptionData(QStringLiteral("Unknown exception")));
    }
    emit finished(result, error);
    reply->deleteLater();
    if(autoDelete_) this->deleteLater();
}

