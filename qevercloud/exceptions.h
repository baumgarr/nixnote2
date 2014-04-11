#ifndef QEVERCLOUD_EXCEPTIONS_H
#define QEVERCLOUD_EXCEPTIONS_H

#include <exception>
#include <string>
#include <QString>

namespace qevercloud {

/**
 * All exceptions throws by the library are of this class or its descendants.
 */
class EverCloudException: public std::exception {
protected:
    mutable std::string err_;
public:
    explicit EverCloudException() {}
    ~EverCloudException() throw() {}
    explicit EverCloudException(QString err): err_(err.toUtf8().constData()) {}
    explicit EverCloudException(const std::string& err): err_(err) {}
    explicit EverCloudException(const char* err): err_(err) {}
    const char* what() const throw() {return err_.c_str();}
};

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
};

/**
 * Errors of th Thrift protocol level. It could be wrongly formatted parameters
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
  ThriftException(Type::type type, const std::string& message) : EverCloudException(message), type_(type) {}

  Type::type type() {
    return type_;
  }

  virtual const char* what() const throw() {
    if (err_.empty()) {
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
      return err_.c_str();
    }
  }

  // internal
  void read(void* reader);

protected:
  Type::type type_;
};


}

#endif // EXCEPTIONS_H
