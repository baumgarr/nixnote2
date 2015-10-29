#ifndef QEVERCLOUD_EVERCLOUDEXCEPTION_H
#define QEVERCLOUD_EVERCLOUDEXCEPTION_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include <exception>
#include "qt4helpers.h"

namespace qevercloud {

class EverCloudExceptionData;

/**
 * All exceptions throws by the library are of this class or its descendants.
 */
class EverCloudException: public std::exception {
protected:
    mutable QByteArray err_;
public:
    explicit EverCloudException() {}
    ~EverCloudException() throw() {}
    explicit EverCloudException(QString err): err_(err.toUtf8()) {}
    explicit EverCloudException(const std::string& err): err_(err.c_str()) {}
    explicit EverCloudException(const char* err): err_(err) {}
    const char* what() const throw() Q_DECL_OVERRIDE {return err_.constData();}

    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const;
};

/**
 * @brief EverCloudException counterpart for asynchronous API.
 *
 * Asynchronous functions cannot throw exceptions so descendants of EverCloudExceptionData are retunded instead
 * in case of an error. Every exception class has its own counterpart.
 * The EverCloudExceptionData descendants hierarchy is a copy of the EverCloudException descendants hierarchy.
 *
 * The main reason not to use exception classes directly is that dynamic_cast does not work across module (exe, dll, etc) boundaries
 * in general, while `qobject_cast` do work as expected. That's why I decided to inherit my error classes from QObject.
 *
 * In general error checking in asynchronous API look like this:
 *
 * @code
NoteStore* ns;
...
QObject::connect(ns->getNotebook(notebookGuid), &AsyncResult::finished, [](QVariant result, QSharedPointer<EverCloudExceptionData> error) {
    if(!error.isNull()) {
        QSharedPointer<EDAMNotFoundExceptionData> errorNotFound = error.objectCast<EDAMNotFoundExceptionData>();
        QSharedPointer<EDAMUserExceptionData> errorUser = error.objectCast<EDAMUserExceptionData>();
        QSharedPointer<EDAMSystemExceptionData> errorSystem = error.objectCast<EDAMSystemExceptionData>();
        if(!errorNotFound.isNull()) {
            qDebug() << "notebook not found" << errorNotFound.identifier << errorNotFound.key;
        } else if(!errorUser.isNull()) {
            qDebug() << errorUser.errorMessage;
        } else if(!errorSystem.isNull()) {
            if(errorSystem.errorCode == EDAMErrorCode::RATE_LIMIT_REACHED) {
                qDebug() << "Evernote API rate limits are reached";
            } else if(errorSystem.errorCode == EDAMErrorCode::AUTH_EXPIRED) {
                qDebug() << "Authorization token is inspired";
            } else {
                // some other Evernote trouble
                qDebug() << errorSystem.errorMessage;
            }
        } else {
            // some unexpected error
            qDebug() << error.errorMessage;
        }
    } else {
      // success
    }
});

 @endcode

*/
class EverCloudExceptionData: public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(EverCloudExceptionData)
public:
    /**
     * Contains an error message. It's the std::exception::what() counterpart.
     */
    QString errorMessage;
    explicit EverCloudExceptionData(QString err) : errorMessage(err) {}

    /**
     * If you want to throw an exception that corresponds to a recrived EverCloudExceptionData
     * descendant than call this function. Do not use `throw` statement, it's not polymorphic.
     */
    virtual void throwException() const {throw EverCloudException(errorMessage);}
};

inline QSharedPointer<EverCloudExceptionData> EverCloudException::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new EverCloudExceptionData(what()));
}

/**
 * All exception sent by Evernote servers (as opposed to other error conditions, for example http errors) are
 * descendants of this class.
 */
class EvernoteException: public EverCloudException {
public:
    explicit EvernoteException(): EverCloudException() {}
    explicit EvernoteException(QString err): EverCloudException(err) {}
    explicit EvernoteException(const std::string& err): EverCloudException(err) {}
    explicit EvernoteException(const char* err): EverCloudException(err) {}

    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const Q_DECL_OVERRIDE;
};

/** Asynchronous API conterpart of EvernoteException. See EverCloudExceptionData for more details.*/
class EvernoteExceptionData: public EverCloudExceptionData {
    Q_OBJECT
    Q_DISABLE_COPY(EvernoteExceptionData)
public:
    explicit EvernoteExceptionData(QString err) : EverCloudExceptionData(err) {}
    virtual void throwException() const Q_DECL_OVERRIDE {throw EvernoteException(errorMessage);}
};

inline QSharedPointer<EverCloudExceptionData> EvernoteException::exceptionData() const
{
    return QSharedPointer<EverCloudExceptionData>(new EvernoteExceptionData(what()));
}


}
#endif // QEVERCLOUD_EVERCLOUDEXCEPTION_H
