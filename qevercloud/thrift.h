#ifndef QEVERCLOUD_THRIFT_H
#define QEVERCLOUD_THRIFT_H

#include <QByteArray>
#include <QtEndian>
#include <cstring>
#include <limits>


namespace qevercloud {

// Use this to get around strict aliasing rules.
// For example, uint64_t i = bitwise_cast<uint64_t>(returns_double());
// The most obvious implementation is to just cast a pointer,
// but that doesn't work.

// http://cellperformance.beyond3d.com/articles/2006/06/understanding-strict-aliasing.html
// http://stackoverflow.com/questions/98650/what-is-the-strict-aliasing-rule
// http://stackoverflow.com/questions/2906365/gcc-strict-aliasing-and-casting-through-a-union

template <typename To, typename From>
static inline To bitwise_cast(From from) {

    //  static_assert(sizeof(From) == sizeof(To), "bitwise_cast: incompatible types");

  // BAD!!!  These are all broken with -O2.
  //return *reinterpret_cast<To*>(&from);  // BAD!!!
  //return *static_cast<To*>(static_cast<void*>(&from));  // BAD!!!
  //return *(To*)(void*)&from;  // BAD!!!

  // Super clean and partially blessed by section 3.9 of the standard.
  //unsigned char c[sizeof(from)];
  //memcpy(c, &from, sizeof(from));
  //To to;
  //memcpy(&to, c, sizeof(c));
  //return to;

  // Slightly more questionable.
  // Same code emitted by GCC.
  //To to;
  //memcpy(&to, &from, sizeof(from));
  //return to;

  // Technically undefined, but almost universally supported,
  // and the most efficient implementation.
  union {
    From f;
    To t;
  } u;
  u.f = from;
  return u.t;
}

struct ThriftFieldType {
    enum type {
      T_STOP       = 0,
      T_VOID       = 1,
      T_BOOL       = 2,
      T_BYTE       = 3,
      T_I08        = 3,
      T_I16        = 6,
      T_I32        = 8,
      T_U64        = 9,
      T_I64        = 10,
      T_DOUBLE     = 4,
      T_STRING     = 11,
      T_UTF7       = 11,
      T_STRUCT     = 12,
      T_MAP        = 13,
      T_SET        = 14,
      T_LIST       = 15,
      T_UTF8       = 16,
      T_UTF16      = 17
    };
};

struct ThriftMessageType {
    enum type {
      T_CALL       = 1,
      T_REPLY      = 2,
      T_EXCEPTION  = 3,
      T_ONEWAY     = 4
    };
};

static const qint32 VERSION_1 = ((qint32)0x80010000);
static const qint32 VERSION_MASK = ((qint32)0xffff0000);

class ThriftBinaryBufferWriter {
private:
    QByteArray buf;
    bool strict;

    void write(const quint8* bytes, const int bytesCount) {
        buf.append(reinterpret_cast<const char*>(bytes), bytesCount);
    }

public:
    ThriftBinaryBufferWriter(): strict(true) {}
    void setStrictMode(bool on) {strict = on;}

    QByteArray buffer() {return buf;}

    quint32 writeMessageBegin(QString name, const ThriftMessageType::type messageType, const qint32 seqid) {
        if (strict) {
          qint32 version = (VERSION_1) | ((qint32)messageType);
          quint32 wsize = 0;
          wsize += writeI32(version);
          wsize += writeString(name);
          wsize += writeI32(seqid);
          return wsize;
        } else {
          quint32 wsize = 0;
          wsize += writeString(name);
          wsize += writeByte((qint8)messageType);
          wsize += writeI32(seqid);
          return wsize;
        }
    }

    quint32 writeMessageEnd() {return 0;}


    inline quint32 writeStructBegin(QString name)
    {
        (void) name;
        return 0;
    }

    inline quint32 writeStructEnd() {return 0;}

    inline quint32 writeFieldBegin(QString name, const ThriftFieldType::type fieldType, const qint16 fieldId)
    {
        (void) name;
        quint32 wsize = 0;
        wsize += writeByte(static_cast<qint8>(fieldType));
        wsize += writeI16(fieldId);
        return wsize;
    }

    inline quint32 writeFieldEnd() {return 0;}
    inline quint32 writeFieldStop() {return writeByte((qint8)ThriftFieldType::T_STOP);}


    inline quint32 writeMapBegin(const ThriftFieldType::type keyType, const ThriftFieldType::type valType, const quint32 size)
    {
        quint32 wsize = 0;
        wsize += writeByte(static_cast<qint8>(keyType));
        wsize += writeByte(static_cast<qint8>(valType));
        wsize += writeI32(static_cast<qint32>(size));
        return wsize;
    }
    inline quint32 writeMapEnd() {return 0;}

    inline quint32 writeListBegin(const ThriftFieldType::type elemType, const quint32 size)
    {
        quint32 wsize = 0;
        wsize += writeByte(static_cast<qint8>(elemType));
        wsize += writeI32(static_cast<qint32>(size));
        return wsize;
    }
    inline quint32 writeListEnd() {return 0;}

    inline quint32 writeSetBegin(const ThriftFieldType::type elemType, const quint32 size)
    {
        return writeListBegin(elemType, size);
    }
    inline quint32 writeSetEnd() {return 0;}

    inline quint32 writeBool(bool value)
    {
        quint8 tmp = value ? 1 : 0;
        write(&tmp, 1);
        return 1;
    }

    inline quint32 writeByte(const qint8 byte)
    {
        write(reinterpret_cast<const quint8*>(&byte), 1);
        return 1;
    }

    inline quint32 writeI16(const qint16 i16)
    {
        qint16 net;
        qToBigEndian(i16, reinterpret_cast<uchar*>(&net));
        write(reinterpret_cast<const quint8*>(&net), 2);
        return 2;
    }

    inline quint32 writeI32(const qint32 i32)
    {
        qint32 net;
        qToBigEndian(i32, reinterpret_cast<uchar*>(&net));
        write(reinterpret_cast<const quint8*>(&net), 4);
        return 4;
    }

    inline quint32 writeI64(const qint64 i64)
    {
        qint64 net;
        qToBigEndian(i64, reinterpret_cast<uchar*>(&net));
        write(reinterpret_cast<const quint8*>(&net), 8);
        return 8;
    }

    inline quint32 writeDouble(const double dub)
    {
        // static_assert(sizeof(double) == sizeof(qint64) && std::numeric_limits<double>::is_iec559, "incompatible double type");

        qint64 bits = bitwise_cast<quint64>(dub);
        qToBigEndian(bits, reinterpret_cast<uchar*>(&bits));
        write(reinterpret_cast<quint8*>(&bits), 8);
        return 8;
    }

    inline quint32 writeString(const QString& str)
    {
        QByteArray utf8 = str.toUtf8();
        return writeBinary(utf8);
    }

    inline quint32 writeBinary(const QByteArray& bytes)
    {
        qint32 size = static_cast<qint32>(bytes.length());
        if(size > std::numeric_limits<qint32>::max()) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "The data is too big");
        }
        quint32 result = writeI32((qint32)size);
        if (size > 0) {
          write((const quint8*)bytes.constData(), size);
        }
        return result + size;
    }

};


class ThriftBinaryBufferReader {
    QByteArray buf;
    qint32 pos;
    qint32 stringLimit;
    bool strict;

    void read(quint8* dest, qint32 bytesCount) {
        if((pos + bytesCount) > buf.length()) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Unexpected end of data");
        }
        std::memcpy(dest, buf.mid(pos, bytesCount).constData(), bytesCount);
        pos += bytesCount;
    }

public:

    ThriftBinaryBufferReader(QByteArray buffer): buf(buffer), pos(0), stringLimit(0), strict(false) {}
    void setStringLimit(qint32 limit) {stringLimit = limit;}
    void setStrictMode(bool on) {strict = on;}

    quint32 readMessageBegin(QString& name, ThriftMessageType::type& messageType, qint32& seqid)
    {
        quint32 result = 0;
        qint32 sz;
        result += readI32(sz);

        if (sz < 0) {
          // Check for correct version number
          qint32 version = sz & VERSION_MASK;
          if (version != VERSION_1) {
              throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Bad version identifier");
          }
          messageType = static_cast<ThriftMessageType::type>(sz & 0x000000ff);
          result += readString(name);
          result += readI32(seqid);
        } else {
          if (strict) {
              throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "No version identifier... old protocol client in strict mode?");
          } else {
            // Handle pre-versioned input
            qint8 type;
            pos -= 4;
            result += readString(name);
            result += readByte(type);
            messageType = static_cast<ThriftMessageType::type>(type);
            result += readI32(seqid);
          }
        }
        return result;
    }

    quint32 readMessageEnd() {return 0;}

    inline quint32 readStructBegin(QString& name) {name = ""; return 0;}
    inline quint32 readStructEnd() {return 0;}
    inline quint32 readFieldBegin(QString& name, ThriftFieldType::type& fieldType, qint16& fieldId)
    {
        (void) name;
        quint32 result = 0;
        qint8 type;
        result += readByte(type);
        fieldType = static_cast<ThriftFieldType::type>(type);
        if (fieldType == ThriftFieldType::T_STOP) {
          fieldId = 0;
          return result;
        }
        result += readI16(fieldId);
        return result;
    }

    inline quint32 readFieldEnd() {return 0;}
    inline quint32 readMapBegin(ThriftFieldType::type& keyType, ThriftFieldType::type& valType, quint32& size)
    {
        qint8 k, v;
        quint32 result = 0;
        qint32 sizei;
        result += readByte(k);
        keyType = static_cast<ThriftFieldType::type>(k);
        result += readByte(v);
        valType = static_cast<ThriftFieldType::type>(v);
        result += readI32(sizei);
        if (sizei < 0) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Negative size!");
        } else if (stringLimit > 0 && sizei > stringLimit) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "The size limit is exceeded.");
        }
        size = (quint32)sizei;
        return result;
    }

    inline quint32 readMapEnd() {return 0;}
    inline quint32 readListBegin(ThriftFieldType::type& elemType, quint32& size)
    {
        qint8 e;
        quint32 result = 0;
        qint32 sizei;
        result += readByte(e);
        elemType = static_cast<ThriftFieldType::type>(e);
        result += readI32(sizei);
        if (sizei < 0) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Negative size!");
        } else if (stringLimit > 0 && sizei > stringLimit) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "The size limit is exceeded.");
        }
        size = (quint32)sizei;
        return result;
    }

    inline quint32 readListEnd() {return 0;}
    inline quint32 readSetBegin(ThriftFieldType::type& elemType, quint32& size) {return readListBegin(elemType, size);}
    inline quint32 readSetEnd() {return 0;}

    inline quint32 readBool(bool& value)
    {
        quint8 b[1];
        read(b, 1);
        value = *(qint8*)b != 0;
        return 1;
    }

    inline quint32 readByte(qint8& byte)
    {
        quint8 b[1];
        read(b, 1);
        byte = *(qint8*)b;
        return 1;
    }

    inline quint32 readI16(qint16& i16)
    {
        union bytes {
          quint8 b[2];
          qint16 all;
        } theBytes;
        read(theBytes.b, 2);
        i16 = qFromBigEndian<qint16>(theBytes.all);
        return 2;
    }

    inline quint32 readI32(qint32& i32)
    {
        union bytes {
          quint8 b[4];
          qint32 all;
        } theBytes;
        read(theBytes.b, 4);
        i32 = qFromBigEndian<qint32>(theBytes.all);
        return 4;
    }

    inline quint32 readI64(qint64& i64)
    {
        union bytes {
          quint8 b[8];
          qint64 all;
        } theBytes;
        read(theBytes.b, 8);
        i64 = qFromBigEndian<qint64>(theBytes.all);
        return 8;
    }

    inline quint32 readDouble(double& dub)
    {
        // static_assert(sizeof(double) == sizeof(qint64) && std::numeric_limits<double>::is_iec559, "incompatible double type");

        union bytes {
          quint8 b[8];
          qint64 all;
        } theBytes;
        read(theBytes.b, 8);
        theBytes.all = qFromBigEndian<qint64>(theBytes.all);
        dub = bitwise_cast<double>(theBytes.all);
        return 8;
    }

    inline quint32 readString(QString& str)
    {
        quint32 result;
        qint32 size;
        result = readI32(size);

        if (size < 0) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Negative size!");
        }
        if (stringLimit > 0 && size > stringLimit) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "The size limit is exceeded.");
        }

        // Catch empty string case
        if (size == 0) {
          str.clear();
          return result;
        }

        if((pos + size) > buf.length()) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Unexpected end of data");
        }

        str = QString::fromUtf8(buf.constData() + pos, size);
        pos += size;
        result += size;

        return result;
    }

    inline quint32 readBinary(QByteArray& str)
    {
        quint32 result;
        qint32 size;
        result = readI32(size);

        if (size < 0) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Negative size!");
        }
        if (stringLimit > 0 && size > stringLimit) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "The size limit is exceeded.");
        }

        // Catch empty string case
        if (size == 0) {
          str.clear();
          return result;
        }

        if((pos + size) > buf.length()) {
            throw ThriftException(ThriftException::Type::PROTOCOL_ERROR, "Unexpected end of data");
        }

        str = buf.mid(pos, size);
        pos += size;
        result += size;

        return result;
    }

    inline quint32 skip(ThriftFieldType::type type)
    {
      switch (type) {
      case ThriftFieldType::T_BOOL:
        {
          bool boolv;
          return readBool(boolv);
        }
      case ThriftFieldType::T_BYTE:
        {
          qint8 bytev;
          return readByte(bytev);
        }
      case ThriftFieldType::T_I16:
        {
          qint16 i16;
          return readI16(i16);
        }
      case ThriftFieldType::T_I32:
        {
          qint32 i32;
          return readI32(i32);
        }
      case ThriftFieldType::T_I64:
        {
          qint64 i64;
          return readI64(i64);
        }
      case ThriftFieldType::T_DOUBLE:
        {
          double dub;
          return readDouble(dub);
        }
      case ThriftFieldType::T_STRING:
        {
          QByteArray str;
          return readBinary(str);
        }
      case ThriftFieldType::T_STRUCT:
        {
          quint32 result = 0;
          QString name;
          qint16 fid;
          ThriftFieldType::type ftype;
          result += readStructBegin(name);
          while (true) {
            result += readFieldBegin(name, ftype, fid);
            if (ftype == ThriftFieldType::T_STOP) {
              break;
            }
            result += skip(ftype);
            result += readFieldEnd();
          }
          result += readStructEnd();
          return result;
        }
      case ThriftFieldType::T_MAP:
        {
          quint32 result = 0;
          ThriftFieldType::type keyType;
          ThriftFieldType::type valType;
          quint32 i, size;
          result += readMapBegin(keyType, valType, size);
          for (i = 0; i < size; i++) {
            result += skip(keyType);
            result += skip(valType);
          }
          result += readMapEnd();
          return result;
        }
      case ThriftFieldType::T_SET:
        {
          quint32 result = 0;
          ThriftFieldType::type elemType;
          quint32 i, size;
          result += readSetBegin(elemType, size);
          for (i = 0; i < size; i++) {
            result += skip(elemType);
          }
          result += readSetEnd();
          return result;
        }
      case ThriftFieldType::T_LIST:
        {
          quint32 result = 0;
          ThriftFieldType::type elemType;
          quint32 i, size;
          result += readListBegin(elemType, size);
          for (i = 0; i < size; i++) {
            result += skip(elemType);
          }
          result += readListEnd();
          return result;
        }
      case ThriftFieldType::T_STOP:
      case ThriftFieldType::T_VOID:
      case ThriftFieldType::T_U64:
      case ThriftFieldType::T_UTF8:
      case ThriftFieldType::T_UTF16:
        break;
      }
      return 0;
    }

};


}

#endif // QEVERCLOUD_THRIFT_H
