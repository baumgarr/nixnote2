
#include "generated/types.h"
#include "generated/types_impl.h"
#include "impl.h"

/** @cond HIDDEN_SYMBOLS  */

namespace qevercloud {


std::string strEDAMErrorCode(EDAMErrorCode::type errorCode)
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
    if(err_.empty()) {
        err_ = "EDAMUserException: " + strEDAMErrorCode(errorCode);
        if(parameter.isSet()) {
            err_ += " parameter=" + parameter->toStdString();
        }
    }
    return EvernoteException::what();
}

const char *EDAMSystemException::what() const throw()
{
    if(err_.empty()) {
        err_ = "EDAMSystemException: " + strEDAMErrorCode(errorCode);
        if(message.isSet()) {
            err_ += " " + message->toStdString();
        }
        if(rateLimitDuration.isSet()) {
            err_ += QString(" rateLimitDuration= %1 sec.").arg(rateLimitDuration).toStdString();
        }
    }
    return EvernoteException::what();
}

const char * EDAMNotFoundException::what() const throw()
{
    if(err_.empty()) {
        err_ = "EDAMNotFoundException: ";
        if(identifier.isSet()) {
            err_ += " identifier=" + identifier->toStdString();
        }
        if(key.isSet()) {
            err_ += " key=" + key->toStdString();
        }
    }
    return EvernoteException::what();
}

void ThriftException::read(void *reader)
{
    ThriftBinaryBufferReader& r = *reinterpret_cast<ThriftBinaryBufferReader*>(reader);
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
                err_ = str.toStdString();
            } else {
                r.skip(fieldType);
            }
            break;
        case 2:
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 t;
                r.readI32(t);
                type_ = static_cast<Type::type>(t);
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
    if(!errorCode_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "EDAMUserException.errorCode has no value");
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
    if(!errorCode_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "EDAMSystemException.errorCode has no value");
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



}

/** @endcond  */
