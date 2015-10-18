#ifndef QEVERCLOUD_EXCEPTIONS_H
#define QEVERCLOUD_EXCEPTIONS_H

#include <QString>
#include <QObject>
#include <QSharedPointer>
#include "Optional.h"
#include "./generated/EDAMErrorCode.h"
#include "EverCloudException.h"
#include "./generated/types.h"

namespace qevercloud {


/**
 * Errors of the Thrift protocol level. It could be wrongly formatted parameters
 * or return values for example.
 */
class ThriftException : public EverCloudException {
public:

    struct Type {
        enum type {
            UNKNOWN = 0,
            UNKNOWN_METHOD = 1,
            INVALID_MESSAGE_TYPE = 2,
            WRONG_METHOD_NAME = 3,
            BAD_SEQUENCE_ID = 4,
            MISSING_RESULT = 5,
            INTERNAL_ERROR = 6,
            PROTOCOL_ERROR = 7,
            INVALID_DATA = 8
        };
    };

    ThriftException() : EverCloudException(), type_(Type::UNKNOWN) {}
    ThriftException(Type::type type) : EverCloudException(), type_(type) {}
    ThriftException(Type::type type, QString message) : EverCloudException(message), type_(type) {}

    Type::type type() const {
        return type_;
    }

    const char* what() const throw() Q_DECL_OVERRIDE;

    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const Q_DECL_OVERRIDE;

protected:
    Type::type type_;
};

/** Asynchronous API conterpart of ThriftException. See EverCloudExceptionData for more details.*/
class ThriftExceptionData: public EverCloudExceptionData {
    Q_OBJECT
    Q_DISABLE_COPY(ThriftExceptionData)
public:
    ThriftException::Type::type type;
    explicit ThriftExceptionData(QString err, ThriftException::Type::type type) : EverCloudExceptionData(err), type(type) {}
    virtual void throwException() const Q_DECL_OVERRIDE {throw ThriftException(type, errorMessage);}
};

inline QSharedPointer<EverCloudExceptionData> ThriftException::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new ThriftExceptionData(what(), type()));
}

/** Asynchronous API conterpart of EDAMUserException. See EverCloudExceptionData for more details.*/
class EDAMUserExceptionData: public EvernoteExceptionData {
    Q_OBJECT
    Q_DISABLE_COPY(EDAMUserExceptionData)
public:
    EDAMErrorCode::type errorCode;
    Optional<QString> parameter;
    explicit EDAMUserExceptionData(QString err, EDAMErrorCode::type errorCode, Optional<QString> parameter) : EvernoteExceptionData(err), errorCode(errorCode), parameter(parameter) {}
    virtual void throwException() const Q_DECL_OVERRIDE;
};

/** Asynchronous API conterpart of EDAMSystemException. See EverCloudExceptionData for more details.*/
class EDAMSystemExceptionData: public EvernoteExceptionData {
    Q_OBJECT
    Q_DISABLE_COPY(EDAMSystemExceptionData)
public:
    EDAMErrorCode::type errorCode;
    Optional<QString> message;
    Optional<qint32> rateLimitDuration;
    explicit EDAMSystemExceptionData(QString err, EDAMErrorCode::type errorCode, Optional<QString> message, Optional<qint32> rateLimitDuration) : EvernoteExceptionData(err), errorCode(errorCode), message(message), rateLimitDuration(rateLimitDuration) {}
    virtual void throwException() const Q_DECL_OVERRIDE;
};

/** Asynchronous API conterpart of EDAMNotFoundException. See EverCloudExceptionData for more details.*/
class EDAMNotFoundExceptionData: public EvernoteExceptionData {
    Q_OBJECT
    Q_DISABLE_COPY(EDAMNotFoundExceptionData)
public:
    Optional<QString> identifier;
    Optional<QString> key;
    explicit EDAMNotFoundExceptionData(QString err, Optional<QString> identifier, Optional<QString> key) : EvernoteExceptionData(err), identifier(identifier), key(key) {}
    virtual void throwException() const Q_DECL_OVERRIDE;
};

/**
 *  EDAMSystemException for `errorCode = RATE_LIMIT_REACHED`
 */
class EDAMSystemExceptionRateLimitReached: public EDAMSystemException {
public:
    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const Q_DECL_OVERRIDE;
};

/** Asynchronous API conterpart of EDAMSystemExceptionRateLimitReached. See EverCloudExceptionData for more details.*/
class EDAMSystemExceptionRateLimitReachedData: public EDAMSystemExceptionData {
    Q_OBJECT
    Q_DISABLE_COPY(EDAMSystemExceptionRateLimitReachedData)
public:
    explicit EDAMSystemExceptionRateLimitReachedData(QString err, EDAMErrorCode::type errorCode, Optional<QString> message, Optional<qint32> rateLimitDuration) : EDAMSystemExceptionData(err, errorCode, message, rateLimitDuration) {}
    virtual void throwException() const Q_DECL_OVERRIDE;
};

/**
 *  EDAMSystemException for `errorCode = AUTH_EXPIRED`
 */
class EDAMSystemExceptionAuthExpired: public EDAMSystemException {
public:
    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const Q_DECL_OVERRIDE;
};

/** Asynchronous API conterpart of EDAMSystemExceptionAuthExpired. See EverCloudExceptionData for more details.*/
class EDAMSystemExceptionAuthExpiredData: public EDAMSystemExceptionData {
    Q_OBJECT
    Q_DISABLE_COPY(EDAMSystemExceptionAuthExpiredData)
public:
    explicit EDAMSystemExceptionAuthExpiredData(QString err, EDAMErrorCode::type errorCode, Optional<QString> message, Optional<qint32> rateLimitDuration) : EDAMSystemExceptionData(err, errorCode, message, rateLimitDuration) {}
    virtual void throwException() const Q_DECL_OVERRIDE;
};



}

#endif // EXCEPTIONS_H
