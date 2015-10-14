
#include "generated/types.h"
#include "generated/types_impl.h"
#include "impl.h"
#include "Optional.h"

/** @cond HIDDEN_SYMBOLS  */

namespace qevercloud {


QByteArray strEDAMErrorCode(EDAMErrorCode::type errorCode)
{
    switch(errorCode)  {
    case EDAMErrorCode::UNKNOWN: return "UNKNOWN";
    case EDAMErrorCode::BAD_DATA_FORMAT: return "BAD_DATA_FORMAT";
    case EDAMErrorCode::PERMISSION_DENIED: return "PERMISSION_DENIED";
    case EDAMErrorCode::INTERNAL_ERROR: return "INTERNAL_ERROR";
    case EDAMErrorCode::DATA_REQUIRED: return "DATA_REQUIRED";
    case EDAMErrorCode::LIMIT_REACHED: return "LIMIT_REACHED";
    case EDAMErrorCode::QUOTA_REACHED: return "QUOTA_REACHED";
    case EDAMErrorCode::INVALID_AUTH: return "INVALID_AUTH";
    case EDAMErrorCode::AUTH_EXPIRED: return "AUTH_EXPIRED";
    case EDAMErrorCode::DATA_CONFLICT: return "DATA_CONFLICT";
    case EDAMErrorCode::ENML_VALIDATION: return "ENML_VALIDATION";
    case EDAMErrorCode::SHARD_UNAVAILABLE: return "SHARD_UNAVAILABLE";
    case EDAMErrorCode::LEN_TOO_SHORT: return "LEN_TOO_SHORT";
    case EDAMErrorCode::LEN_TOO_LONG: return "LEN_TOO_LONG";
    case EDAMErrorCode::TOO_FEW: return "TOO_FEW";
    case EDAMErrorCode::TOO_MANY: return "TOO_MANY";
    case EDAMErrorCode::UNSUPPORTED_OPERATION: return "UNSUPPORTED_OPERATION";
    case EDAMErrorCode::TAKEN_DOWN: return "TAKEN_DOWN";
    case EDAMErrorCode::RATE_LIMIT_REACHED: return "RATE_LIMIT_REACHED";
    default: return "???";
    }
}

const char *EDAMUserException::what() const throw()
{
    if(err_.isEmpty()) {
        err_ = "EDAMUserException: " + strEDAMErrorCode(errorCode);
        if(parameter.isSet()) {
            err_ += " parameter=" + parameter->toUtf8();
        }
    }
    return EvernoteException::what();
}

const char *EDAMSystemException::what() const throw()
{
    if(err_.isEmpty()) {
        err_ = "EDAMSystemException: " + strEDAMErrorCode(errorCode);
        if(message.isSet()) {
            err_ += " " + message->toUtf8();
        }
        if(rateLimitDuration.isSet()) {
            err_ += QStringLiteral(" rateLimitDuration= %1 sec.").arg(rateLimitDuration).toUtf8();
        }
    }
    return EvernoteException::what();
}

const char * EDAMNotFoundException::what() const throw()
{
    if(err_.isEmpty()) {
        err_ = "EDAMNotFoundException: ";
        if(identifier.isSet()) {
            err_ += " identifier=" + identifier->toUtf8();
        }
        if(key.isSet()) {
            err_ += " key=" + key->toUtf8();
        }
    }
    return EvernoteException::what();
}

ThriftException readThriftException(ThriftBinaryBufferReader& r)
{
    QString name;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(name);
    QString err;
    ThriftException::Type::type type = ThriftException::Type::UNKNOWN;
    while(true) {
        r.readFieldBegin(name, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        switch(fieldId) {
        case 1:
            if(fieldType == ThriftFieldType::T_STRING) {
                QString str;
                r.readString(str);
                err = str;
            } else {
                r.skip(fieldType);
            }
            break;
        case 2:
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 t;
                r.readI32(t);
                type = static_cast<ThriftException::Type::type>(t);
            } else {
                r.skip(fieldType);
            }
            break;
        default:
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    return ThriftException(type, err);
}

void readEDAMUserException(ThriftBinaryBufferReader& r, EDAMUserException& e)
{
    QString name;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool errorCode_isset = false;
    r.readStructBegin(name);
    while(true) {
        r.readFieldBegin(name, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        switch(fieldId) {
        case 2:
            if(fieldType == ThriftFieldType::T_STRING) {
                QString str;
                r.readString(str);
                e.parameter = str;
            } else {
                r.skip(fieldType);
            }
            break;
        case 1:
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 t;
                r.readI32(t);
                e.errorCode = static_cast<EDAMErrorCode::type>(t);
                errorCode_isset = true;
            } else {
                r.skip(fieldType);
            }
            break;
        default:
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!errorCode_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, QStringLiteral("EDAMUserException.errorCode has no value"));
}

void readEDAMSystemException(ThriftBinaryBufferReader& r, EDAMSystemException& e)
{
    QString name;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool errorCode_isset = false;
    r.readStructBegin(name);
    while(true) {
        r.readFieldBegin(name, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        switch(fieldId) {
        case 2:
            if(fieldType == ThriftFieldType::T_STRING) {
                QString str;
                r.readString(str);
                e.message = str;
            } else {
                r.skip(fieldType);
            }
            break;
        case 1:
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 t;
                r.readI32(t);
                e.errorCode = static_cast<EDAMErrorCode::type>(t);
                errorCode_isset = true;
            } else {
                r.skip(fieldType);
            }
            break;
        case 3:
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 t;
                r.readI32(t);
                e.rateLimitDuration = t;
                errorCode_isset = true;
            } else {
                r.skip(fieldType);
            }
            break;
        default:
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!errorCode_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, QStringLiteral("EDAMSystemException.errorCode has no value"));
}

void readEDAMNotFoundException(ThriftBinaryBufferReader& r, EDAMNotFoundException& e)
{
    QString name;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(name);
    while(true) {
        r.readFieldBegin(name, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        switch(fieldId) {
        case 1:
            if(fieldType == ThriftFieldType::T_STRING) {
                QString str;
                r.readString(str);
                e.identifier = str;
            } else {
                r.skip(fieldType);
            }
            break;
        case 2:
            if(fieldType == ThriftFieldType::T_STRING) {
                QString str;
                r.readString(str);
                e.key = str;
            } else {
                r.skip(fieldType);
            }
            break;
        default:
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}


QSharedPointer<EverCloudExceptionData> EDAMUserException::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new EDAMUserExceptionData(what(), errorCode, parameter));
}

void EDAMUserExceptionData::throwException() const
{
    EDAMUserException e;
    e.errorCode = errorCode;
    e.parameter = parameter;
    throw e;
}


QSharedPointer<EverCloudExceptionData> EDAMSystemException::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new EDAMSystemExceptionData(what(), errorCode, message, rateLimitDuration));
}

void EDAMSystemExceptionData::throwException() const
{
    EDAMSystemException e;
    e.errorCode = errorCode;
    e.message = message;
    e.rateLimitDuration = rateLimitDuration;
    throw e;
}


QSharedPointer<EverCloudExceptionData> EDAMNotFoundException::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new EDAMNotFoundExceptionData(what(), identifier, key));
}

void EDAMNotFoundExceptionData::throwException() const
{
    EDAMNotFoundException e;
    e.identifier = identifier;
    e.key = key;
    throw e;
}

const char *ThriftException::what() const throw()
{
    if (err_.isEmpty()) {
      switch (type_) {
        case Type::UNKNOWN              : return "ThriftException: Unknown application exception";
        case Type::UNKNOWN_METHOD       : return "ThriftException: Unknown method";
        case Type::INVALID_MESSAGE_TYPE : return "ThriftException: Invalid message type";
        case Type::WRONG_METHOD_NAME    : return "ThriftException: Wrong method name";
        case Type::BAD_SEQUENCE_ID      : return "ThriftException: Bad sequence identifier";
        case Type::MISSING_RESULT       : return "ThriftException: Missing result";
        case Type::INVALID_DATA         : return "ThriftException: Invalid data";
        default                   : return "ThriftException: (Invalid exception type)";
      };
    } else {
      return err_.constData();
    }
}

void throwEDAMSystemException(const EDAMSystemException& e)
{
    if(e.errorCode == EDAMErrorCode::AUTH_EXPIRED) {
        EDAMSystemExceptionAuthExpired ee;
        ee.errorCode = e.errorCode;
        ee.message = e.message;
        ee.rateLimitDuration = e.rateLimitDuration;
        throw ee;
    }
    if(e.errorCode == EDAMErrorCode::RATE_LIMIT_REACHED) {
        EDAMSystemExceptionRateLimitReached ee;
        ee.errorCode = e.errorCode;
        ee.message = e.message;
        ee.rateLimitDuration = e.rateLimitDuration;
        throw ee;
    }
    throw e;
}

QSharedPointer<EverCloudExceptionData> EDAMSystemExceptionRateLimitReached::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new EDAMSystemExceptionRateLimitReachedData(what(), errorCode, message, rateLimitDuration));
}

void EDAMSystemExceptionRateLimitReachedData::throwException() const
{
    EDAMSystemExceptionRateLimitReached e;
    e.errorCode = errorCode;
    e.message = message;
    e.rateLimitDuration = rateLimitDuration;
    throw e;
}

QSharedPointer<EverCloudExceptionData> EDAMSystemExceptionAuthExpired::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new EDAMSystemExceptionAuthExpiredData(what(), errorCode, message, rateLimitDuration));
}

void EDAMSystemExceptionAuthExpiredData::throwException() const
{
    EDAMSystemExceptionAuthExpired e;
    e.errorCode = errorCode;
    e.message = message;
    e.rateLimitDuration = rateLimitDuration;
    throw e;
}




}

/** @endcond  */
