// This file is generated from Evernote Thrift API and is a part of the QEverCloud project

#include "services.h"
#include "../impl.h"
#include "types_impl.h"

namespace qevercloud {

SyncState NoteStore::getSyncState(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getSyncState", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getSyncState_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getSyncState") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SyncState result = SyncState();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SyncState v;
                readSyncState(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getSyncState: missing result");
    return result;
}

SyncState NoteStore::getSyncStateWithMetrics(ClientUsageMetrics clientMetrics, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getSyncStateWithMetrics", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getSyncStateWithMetrics_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("clientMetrics", ThriftFieldType::T_STRUCT, 2);
    writeClientUsageMetrics(w, clientMetrics);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getSyncStateWithMetrics") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SyncState result = SyncState();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SyncState v;
                readSyncState(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getSyncStateWithMetrics: missing result");
    return result;
}

SyncChunk NoteStore::getSyncChunk(qint32 afterUSN, qint32 maxEntries, bool fullSyncOnly, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getSyncChunk", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getSyncChunk_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("afterUSN", ThriftFieldType::T_I32, 2);
    w.writeI32(afterUSN);
    w.writeFieldEnd();
    w.writeFieldBegin("maxEntries", ThriftFieldType::T_I32, 3);
    w.writeI32(maxEntries);
    w.writeFieldEnd();
    w.writeFieldBegin("fullSyncOnly", ThriftFieldType::T_BOOL, 4);
    w.writeBool(fullSyncOnly);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getSyncChunk") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SyncChunk result = SyncChunk();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SyncChunk v;
                readSyncChunk(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getSyncChunk: missing result");
    return result;
}

SyncChunk NoteStore::getFilteredSyncChunk(qint32 afterUSN, qint32 maxEntries, SyncChunkFilter filter, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getFilteredSyncChunk", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getFilteredSyncChunk_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("afterUSN", ThriftFieldType::T_I32, 2);
    w.writeI32(afterUSN);
    w.writeFieldEnd();
    w.writeFieldBegin("maxEntries", ThriftFieldType::T_I32, 3);
    w.writeI32(maxEntries);
    w.writeFieldEnd();
    w.writeFieldBegin("filter", ThriftFieldType::T_STRUCT, 4);
    writeSyncChunkFilter(w, filter);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getFilteredSyncChunk") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SyncChunk result = SyncChunk();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SyncChunk v;
                readSyncChunk(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getFilteredSyncChunk: missing result");
    return result;
}

SyncState NoteStore::getLinkedNotebookSyncState(LinkedNotebook linkedNotebook, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getLinkedNotebookSyncState", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getLinkedNotebookSyncState_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("linkedNotebook", ThriftFieldType::T_STRUCT, 2);
    writeLinkedNotebook(w, linkedNotebook);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getLinkedNotebookSyncState") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SyncState result = SyncState();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SyncState v;
                readSyncState(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getLinkedNotebookSyncState: missing result");
    return result;
}

SyncChunk NoteStore::getLinkedNotebookSyncChunk(LinkedNotebook linkedNotebook, qint32 afterUSN, qint32 maxEntries, bool fullSyncOnly, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getLinkedNotebookSyncChunk", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getLinkedNotebookSyncChunk_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("linkedNotebook", ThriftFieldType::T_STRUCT, 2);
    writeLinkedNotebook(w, linkedNotebook);
    w.writeFieldEnd();
    w.writeFieldBegin("afterUSN", ThriftFieldType::T_I32, 3);
    w.writeI32(afterUSN);
    w.writeFieldEnd();
    w.writeFieldBegin("maxEntries", ThriftFieldType::T_I32, 4);
    w.writeI32(maxEntries);
    w.writeFieldEnd();
    w.writeFieldBegin("fullSyncOnly", ThriftFieldType::T_BOOL, 5);
    w.writeBool(fullSyncOnly);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getLinkedNotebookSyncChunk") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SyncChunk result = SyncChunk();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SyncChunk v;
                readSyncChunk(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getLinkedNotebookSyncChunk: missing result");
    return result;
}

QList< Notebook > NoteStore::listNotebooks(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("listNotebooks", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_listNotebooks_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("listNotebooks") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QList< Notebook > result = QList< Notebook >();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QList< Notebook > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (listNotebooks.result)");
                for(quint32 i = 0; i < size; i++) {
                    Notebook elem;
                    readNotebook(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "listNotebooks: missing result");
    return result;
}

Notebook NoteStore::getNotebook(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Notebook result = Notebook();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Notebook v;
                readNotebook(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNotebook: missing result");
    return result;
}

Notebook NoteStore::getDefaultNotebook(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getDefaultNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getDefaultNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getDefaultNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Notebook result = Notebook();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Notebook v;
                readNotebook(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getDefaultNotebook: missing result");
    return result;
}

Notebook NoteStore::createNotebook(Notebook notebook, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("createNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_createNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("notebook", ThriftFieldType::T_STRUCT, 2);
    writeNotebook(w, notebook);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("createNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Notebook result = Notebook();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Notebook v;
                readNotebook(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "createNotebook: missing result");
    return result;
}

qint32 NoteStore::updateNotebook(Notebook notebook, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("updateNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_updateNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("notebook", ThriftFieldType::T_STRUCT, 2);
    writeNotebook(w, notebook);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("updateNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "updateNotebook: missing result");
    return result;
}

qint32 NoteStore::expungeNotebook(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeNotebook: missing result");
    return result;
}

QList< Tag > NoteStore::listTags(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("listTags", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_listTags_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("listTags") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QList< Tag > result = QList< Tag >();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QList< Tag > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (listTags.result)");
                for(quint32 i = 0; i < size; i++) {
                    Tag elem;
                    readTag(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "listTags: missing result");
    return result;
}

QList< Tag > NoteStore::listTagsByNotebook(Guid notebookGuid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("listTagsByNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_listTagsByNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("notebookGuid", ThriftFieldType::T_STRING, 2);
    w.writeString(notebookGuid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("listTagsByNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QList< Tag > result = QList< Tag >();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QList< Tag > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (listTagsByNotebook.result)");
                for(quint32 i = 0; i < size; i++) {
                    Tag elem;
                    readTag(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "listTagsByNotebook: missing result");
    return result;
}

Tag NoteStore::getTag(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getTag", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getTag_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getTag") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Tag result = Tag();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Tag v;
                readTag(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getTag: missing result");
    return result;
}

Tag NoteStore::createTag(Tag tag, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("createTag", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_createTag_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("tag", ThriftFieldType::T_STRUCT, 2);
    writeTag(w, tag);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("createTag") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Tag result = Tag();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Tag v;
                readTag(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "createTag: missing result");
    return result;
}

qint32 NoteStore::updateTag(Tag tag, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("updateTag", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_updateTag_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("tag", ThriftFieldType::T_STRUCT, 2);
    writeTag(w, tag);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("updateTag") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "updateTag: missing result");
    return result;
}

void NoteStore::untagAll(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("untagAll", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_untagAll_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("untagAll") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
}

qint32 NoteStore::expungeTag(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeTag", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeTag_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeTag") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeTag: missing result");
    return result;
}

QList< SavedSearch > NoteStore::listSearches(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("listSearches", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_listSearches_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("listSearches") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QList< SavedSearch > result = QList< SavedSearch >();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QList< SavedSearch > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (listSearches.result)");
                for(quint32 i = 0; i < size; i++) {
                    SavedSearch elem;
                    readSavedSearch(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "listSearches: missing result");
    return result;
}

SavedSearch NoteStore::getSearch(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getSearch", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getSearch_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getSearch") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SavedSearch result = SavedSearch();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SavedSearch v;
                readSavedSearch(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getSearch: missing result");
    return result;
}

SavedSearch NoteStore::createSearch(SavedSearch search, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("createSearch", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_createSearch_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("search", ThriftFieldType::T_STRUCT, 2);
    writeSavedSearch(w, search);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("createSearch") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SavedSearch result = SavedSearch();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SavedSearch v;
                readSavedSearch(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "createSearch: missing result");
    return result;
}

qint32 NoteStore::updateSearch(SavedSearch search, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("updateSearch", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_updateSearch_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("search", ThriftFieldType::T_STRUCT, 2);
    writeSavedSearch(w, search);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("updateSearch") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "updateSearch: missing result");
    return result;
}

qint32 NoteStore::expungeSearch(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeSearch", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeSearch_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeSearch") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeSearch: missing result");
    return result;
}

NoteList NoteStore::findNotes(NoteFilter filter, qint32 offset, qint32 maxNotes, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("findNotes", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_findNotes_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("filter", ThriftFieldType::T_STRUCT, 2);
    writeNoteFilter(w, filter);
    w.writeFieldEnd();
    w.writeFieldBegin("offset", ThriftFieldType::T_I32, 3);
    w.writeI32(offset);
    w.writeFieldEnd();
    w.writeFieldBegin("maxNotes", ThriftFieldType::T_I32, 4);
    w.writeI32(maxNotes);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("findNotes") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    NoteList result = NoteList();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                NoteList v;
                readNoteList(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "findNotes: missing result");
    return result;
}

qint32 NoteStore::findNoteOffset(NoteFilter filter, Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("findNoteOffset", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_findNoteOffset_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("filter", ThriftFieldType::T_STRUCT, 2);
    writeNoteFilter(w, filter);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 3);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("findNoteOffset") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "findNoteOffset: missing result");
    return result;
}

NotesMetadataList NoteStore::findNotesMetadata(NoteFilter filter, qint32 offset, qint32 maxNotes, NotesMetadataResultSpec resultSpec, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("findNotesMetadata", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_findNotesMetadata_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("filter", ThriftFieldType::T_STRUCT, 2);
    writeNoteFilter(w, filter);
    w.writeFieldEnd();
    w.writeFieldBegin("offset", ThriftFieldType::T_I32, 3);
    w.writeI32(offset);
    w.writeFieldEnd();
    w.writeFieldBegin("maxNotes", ThriftFieldType::T_I32, 4);
    w.writeI32(maxNotes);
    w.writeFieldEnd();
    w.writeFieldBegin("resultSpec", ThriftFieldType::T_STRUCT, 5);
    writeNotesMetadataResultSpec(w, resultSpec);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("findNotesMetadata") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    NotesMetadataList result = NotesMetadataList();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                NotesMetadataList v;
                readNotesMetadataList(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "findNotesMetadata: missing result");
    return result;
}

NoteCollectionCounts NoteStore::findNoteCounts(NoteFilter filter, bool withTrash, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("findNoteCounts", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_findNoteCounts_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("filter", ThriftFieldType::T_STRUCT, 2);
    writeNoteFilter(w, filter);
    w.writeFieldEnd();
    w.writeFieldBegin("withTrash", ThriftFieldType::T_BOOL, 3);
    w.writeBool(withTrash);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("findNoteCounts") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    NoteCollectionCounts result = NoteCollectionCounts();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                NoteCollectionCounts v;
                readNoteCollectionCounts(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "findNoteCounts: missing result");
    return result;
}

Note NoteStore::getNote(Guid guid, bool withContent, bool withResourcesData, bool withResourcesRecognition, bool withResourcesAlternateData, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("withContent", ThriftFieldType::T_BOOL, 3);
    w.writeBool(withContent);
    w.writeFieldEnd();
    w.writeFieldBegin("withResourcesData", ThriftFieldType::T_BOOL, 4);
    w.writeBool(withResourcesData);
    w.writeFieldEnd();
    w.writeFieldBegin("withResourcesRecognition", ThriftFieldType::T_BOOL, 5);
    w.writeBool(withResourcesRecognition);
    w.writeFieldEnd();
    w.writeFieldBegin("withResourcesAlternateData", ThriftFieldType::T_BOOL, 6);
    w.writeBool(withResourcesAlternateData);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Note result = Note();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Note v;
                readNote(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNote: missing result");
    return result;
}

LazyMap NoteStore::getNoteApplicationData(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNoteApplicationData", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNoteApplicationData_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNoteApplicationData") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    LazyMap result = LazyMap();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                LazyMap v;
                readLazyMap(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNoteApplicationData: missing result");
    return result;
}

QString NoteStore::getNoteApplicationDataEntry(Guid guid, QString key, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNoteApplicationDataEntry", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNoteApplicationDataEntry_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("key", ThriftFieldType::T_STRING, 3);
    w.writeString(key);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNoteApplicationDataEntry") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QString result = QString();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QString v;
                r.readString(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNoteApplicationDataEntry: missing result");
    return result;
}

qint32 NoteStore::setNoteApplicationDataEntry(Guid guid, QString key, QString value, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("setNoteApplicationDataEntry", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_setNoteApplicationDataEntry_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("key", ThriftFieldType::T_STRING, 3);
    w.writeString(key);
    w.writeFieldEnd();
    w.writeFieldBegin("value", ThriftFieldType::T_STRING, 4);
    w.writeString(value);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("setNoteApplicationDataEntry") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "setNoteApplicationDataEntry: missing result");
    return result;
}

qint32 NoteStore::unsetNoteApplicationDataEntry(Guid guid, QString key, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("unsetNoteApplicationDataEntry", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_unsetNoteApplicationDataEntry_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("key", ThriftFieldType::T_STRING, 3);
    w.writeString(key);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("unsetNoteApplicationDataEntry") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "unsetNoteApplicationDataEntry: missing result");
    return result;
}

QString NoteStore::getNoteContent(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNoteContent", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNoteContent_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNoteContent") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QString result = QString();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QString v;
                r.readString(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNoteContent: missing result");
    return result;
}

QString NoteStore::getNoteSearchText(Guid guid, bool noteOnly, bool tokenizeForIndexing, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNoteSearchText", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNoteSearchText_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("noteOnly", ThriftFieldType::T_BOOL, 3);
    w.writeBool(noteOnly);
    w.writeFieldEnd();
    w.writeFieldBegin("tokenizeForIndexing", ThriftFieldType::T_BOOL, 4);
    w.writeBool(tokenizeForIndexing);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNoteSearchText") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QString result = QString();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QString v;
                r.readString(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNoteSearchText: missing result");
    return result;
}

QString NoteStore::getResourceSearchText(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceSearchText", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceSearchText_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceSearchText") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QString result = QString();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QString v;
                r.readString(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceSearchText: missing result");
    return result;
}

QStringList NoteStore::getNoteTagNames(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNoteTagNames", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNoteTagNames_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNoteTagNames") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QStringList result = QStringList();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (getNoteTagNames.result)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNoteTagNames: missing result");
    return result;
}

Note NoteStore::createNote(Note note, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("createNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_createNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("note", ThriftFieldType::T_STRUCT, 2);
    writeNote(w, note);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("createNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Note result = Note();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Note v;
                readNote(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "createNote: missing result");
    return result;
}

Note NoteStore::updateNote(Note note, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("updateNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_updateNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("note", ThriftFieldType::T_STRUCT, 2);
    writeNote(w, note);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("updateNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Note result = Note();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Note v;
                readNote(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "updateNote: missing result");
    return result;
}

qint32 NoteStore::deleteNote(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("deleteNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_deleteNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("deleteNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "deleteNote: missing result");
    return result;
}

qint32 NoteStore::expungeNote(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeNote: missing result");
    return result;
}

qint32 NoteStore::expungeNotes(QList< Guid > noteGuids, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeNotes", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeNotes_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("noteGuids", ThriftFieldType::T_LIST, 2);
    w.writeListBegin(ThriftFieldType::T_STRING, noteGuids.length());
    Q_FOREACH(const Guid& elem, noteGuids) {
        w.writeString(elem);
    }
    w.writeListEnd();
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeNotes") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeNotes: missing result");
    return result;
}

qint32 NoteStore::expungeInactiveNotes(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeInactiveNotes", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeInactiveNotes_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeInactiveNotes") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeInactiveNotes: missing result");
    return result;
}

Note NoteStore::copyNote(Guid noteGuid, Guid toNotebookGuid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("copyNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_copyNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("noteGuid", ThriftFieldType::T_STRING, 2);
    w.writeString(noteGuid);
    w.writeFieldEnd();
    w.writeFieldBegin("toNotebookGuid", ThriftFieldType::T_STRING, 3);
    w.writeString(toNotebookGuid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("copyNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Note result = Note();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Note v;
                readNote(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "copyNote: missing result");
    return result;
}

QList< NoteVersionId > NoteStore::listNoteVersions(Guid noteGuid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("listNoteVersions", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_listNoteVersions_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("noteGuid", ThriftFieldType::T_STRING, 2);
    w.writeString(noteGuid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("listNoteVersions") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QList< NoteVersionId > result = QList< NoteVersionId >();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QList< NoteVersionId > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (listNoteVersions.result)");
                for(quint32 i = 0; i < size; i++) {
                    NoteVersionId elem;
                    readNoteVersionId(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "listNoteVersions: missing result");
    return result;
}

Note NoteStore::getNoteVersion(Guid noteGuid, qint32 updateSequenceNum, bool withResourcesData, bool withResourcesRecognition, bool withResourcesAlternateData, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNoteVersion", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getNoteVersion_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("noteGuid", ThriftFieldType::T_STRING, 2);
    w.writeString(noteGuid);
    w.writeFieldEnd();
    w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 3);
    w.writeI32(updateSequenceNum);
    w.writeFieldEnd();
    w.writeFieldBegin("withResourcesData", ThriftFieldType::T_BOOL, 4);
    w.writeBool(withResourcesData);
    w.writeFieldEnd();
    w.writeFieldBegin("withResourcesRecognition", ThriftFieldType::T_BOOL, 5);
    w.writeBool(withResourcesRecognition);
    w.writeFieldEnd();
    w.writeFieldBegin("withResourcesAlternateData", ThriftFieldType::T_BOOL, 6);
    w.writeBool(withResourcesAlternateData);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNoteVersion") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Note result = Note();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Note v;
                readNote(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNoteVersion: missing result");
    return result;
}

Resource NoteStore::getResource(Guid guid, bool withData, bool withRecognition, bool withAttributes, bool withAlternateData, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResource", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResource_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("withData", ThriftFieldType::T_BOOL, 3);
    w.writeBool(withData);
    w.writeFieldEnd();
    w.writeFieldBegin("withRecognition", ThriftFieldType::T_BOOL, 4);
    w.writeBool(withRecognition);
    w.writeFieldEnd();
    w.writeFieldBegin("withAttributes", ThriftFieldType::T_BOOL, 5);
    w.writeBool(withAttributes);
    w.writeFieldEnd();
    w.writeFieldBegin("withAlternateData", ThriftFieldType::T_BOOL, 6);
    w.writeBool(withAlternateData);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResource") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Resource result = Resource();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Resource v;
                readResource(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResource: missing result");
    return result;
}

LazyMap NoteStore::getResourceApplicationData(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceApplicationData", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceApplicationData_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceApplicationData") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    LazyMap result = LazyMap();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                LazyMap v;
                readLazyMap(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceApplicationData: missing result");
    return result;
}

QString NoteStore::getResourceApplicationDataEntry(Guid guid, QString key, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceApplicationDataEntry", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceApplicationDataEntry_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("key", ThriftFieldType::T_STRING, 3);
    w.writeString(key);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceApplicationDataEntry") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QString result = QString();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QString v;
                r.readString(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceApplicationDataEntry: missing result");
    return result;
}

qint32 NoteStore::setResourceApplicationDataEntry(Guid guid, QString key, QString value, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("setResourceApplicationDataEntry", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_setResourceApplicationDataEntry_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("key", ThriftFieldType::T_STRING, 3);
    w.writeString(key);
    w.writeFieldEnd();
    w.writeFieldBegin("value", ThriftFieldType::T_STRING, 4);
    w.writeString(value);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("setResourceApplicationDataEntry") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "setResourceApplicationDataEntry: missing result");
    return result;
}

qint32 NoteStore::unsetResourceApplicationDataEntry(Guid guid, QString key, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("unsetResourceApplicationDataEntry", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_unsetResourceApplicationDataEntry_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("key", ThriftFieldType::T_STRING, 3);
    w.writeString(key);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("unsetResourceApplicationDataEntry") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "unsetResourceApplicationDataEntry: missing result");
    return result;
}

qint32 NoteStore::updateResource(Resource resource, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("updateResource", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_updateResource_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("resource", ThriftFieldType::T_STRUCT, 2);
    writeResource(w, resource);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("updateResource") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "updateResource: missing result");
    return result;
}

QByteArray NoteStore::getResourceData(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceData", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceData_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceData") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QByteArray result = QByteArray();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QByteArray v;
                r.readBinary(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceData: missing result");
    return result;
}

Resource NoteStore::getResourceByHash(Guid noteGuid, QByteArray contentHash, bool withData, bool withRecognition, bool withAlternateData, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceByHash", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceByHash_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("noteGuid", ThriftFieldType::T_STRING, 2);
    w.writeString(noteGuid);
    w.writeFieldEnd();
    w.writeFieldBegin("contentHash", ThriftFieldType::T_STRING, 3);
    w.writeBinary(contentHash);
    w.writeFieldEnd();
    w.writeFieldBegin("withData", ThriftFieldType::T_BOOL, 4);
    w.writeBool(withData);
    w.writeFieldEnd();
    w.writeFieldBegin("withRecognition", ThriftFieldType::T_BOOL, 5);
    w.writeBool(withRecognition);
    w.writeFieldEnd();
    w.writeFieldBegin("withAlternateData", ThriftFieldType::T_BOOL, 6);
    w.writeBool(withAlternateData);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceByHash") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Resource result = Resource();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Resource v;
                readResource(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceByHash: missing result");
    return result;
}

QByteArray NoteStore::getResourceRecognition(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceRecognition", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceRecognition_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceRecognition") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QByteArray result = QByteArray();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QByteArray v;
                r.readBinary(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceRecognition: missing result");
    return result;
}

QByteArray NoteStore::getResourceAlternateData(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceAlternateData", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceAlternateData_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceAlternateData") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QByteArray result = QByteArray();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QByteArray v;
                r.readBinary(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceAlternateData: missing result");
    return result;
}

ResourceAttributes NoteStore::getResourceAttributes(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getResourceAttributes", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getResourceAttributes_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getResourceAttributes") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    ResourceAttributes result = ResourceAttributes();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                ResourceAttributes v;
                readResourceAttributes(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getResourceAttributes: missing result");
    return result;
}

Notebook NoteStore::getPublicNotebook(UserID userId, QString publicUri)
{
    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getPublicNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getPublicNotebook_pargs");
    w.writeFieldBegin("userId", ThriftFieldType::T_I32, 1);
    w.writeI32(userId);
    w.writeFieldEnd();
    w.writeFieldBegin("publicUri", ThriftFieldType::T_STRING, 2);
    w.writeString(publicUri);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getPublicNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    Notebook result = Notebook();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                Notebook v;
                readNotebook(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getPublicNotebook: missing result");
    return result;
}

SharedNotebook NoteStore::createSharedNotebook(SharedNotebook sharedNotebook, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("createSharedNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_createSharedNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("sharedNotebook", ThriftFieldType::T_STRUCT, 2);
    writeSharedNotebook(w, sharedNotebook);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("createSharedNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SharedNotebook result = SharedNotebook();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SharedNotebook v;
                readSharedNotebook(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "createSharedNotebook: missing result");
    return result;
}

qint32 NoteStore::updateSharedNotebook(SharedNotebook sharedNotebook, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("updateSharedNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_updateSharedNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("sharedNotebook", ThriftFieldType::T_STRUCT, 2);
    writeSharedNotebook(w, sharedNotebook);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("updateSharedNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "updateSharedNotebook: missing result");
    return result;
}

qint32 NoteStore::setSharedNotebookRecipientSettings(qint64 sharedNotebookId, SharedNotebookRecipientSettings recipientSettings, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("setSharedNotebookRecipientSettings", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_setSharedNotebookRecipientSettings_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("sharedNotebookId", ThriftFieldType::T_I64, 2);
    w.writeI64(sharedNotebookId);
    w.writeFieldEnd();
    w.writeFieldBegin("recipientSettings", ThriftFieldType::T_STRUCT, 3);
    writeSharedNotebookRecipientSettings(w, recipientSettings);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("setSharedNotebookRecipientSettings") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "setSharedNotebookRecipientSettings: missing result");
    return result;
}

qint32 NoteStore::sendMessageToSharedNotebookMembers(Guid notebookGuid, QString messageText, QStringList recipients, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("sendMessageToSharedNotebookMembers", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_sendMessageToSharedNotebookMembers_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("notebookGuid", ThriftFieldType::T_STRING, 2);
    w.writeString(notebookGuid);
    w.writeFieldEnd();
    w.writeFieldBegin("messageText", ThriftFieldType::T_STRING, 3);
    w.writeString(messageText);
    w.writeFieldEnd();
    w.writeFieldBegin("recipients", ThriftFieldType::T_LIST, 4);
    w.writeListBegin(ThriftFieldType::T_STRING, recipients.length());
    Q_FOREACH(const QString& elem, recipients) {
        w.writeString(elem);
    }
    w.writeListEnd();
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("sendMessageToSharedNotebookMembers") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "sendMessageToSharedNotebookMembers: missing result");
    return result;
}

QList< SharedNotebook > NoteStore::listSharedNotebooks(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("listSharedNotebooks", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_listSharedNotebooks_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("listSharedNotebooks") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QList< SharedNotebook > result = QList< SharedNotebook >();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QList< SharedNotebook > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (listSharedNotebooks.result)");
                for(quint32 i = 0; i < size; i++) {
                    SharedNotebook elem;
                    readSharedNotebook(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "listSharedNotebooks: missing result");
    return result;
}

qint32 NoteStore::expungeSharedNotebooks(QList< qint64 > sharedNotebookIds, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeSharedNotebooks", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeSharedNotebooks_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("sharedNotebookIds", ThriftFieldType::T_LIST, 2);
    w.writeListBegin(ThriftFieldType::T_I64, sharedNotebookIds.length());
    Q_FOREACH(const qint64& elem, sharedNotebookIds) {
        w.writeI64(elem);
    }
    w.writeListEnd();
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeSharedNotebooks") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeSharedNotebooks: missing result");
    return result;
}

LinkedNotebook NoteStore::createLinkedNotebook(LinkedNotebook linkedNotebook, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("createLinkedNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_createLinkedNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("linkedNotebook", ThriftFieldType::T_STRUCT, 2);
    writeLinkedNotebook(w, linkedNotebook);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("createLinkedNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    LinkedNotebook result = LinkedNotebook();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                LinkedNotebook v;
                readLinkedNotebook(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "createLinkedNotebook: missing result");
    return result;
}

qint32 NoteStore::updateLinkedNotebook(LinkedNotebook linkedNotebook, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("updateLinkedNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_updateLinkedNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("linkedNotebook", ThriftFieldType::T_STRUCT, 2);
    writeLinkedNotebook(w, linkedNotebook);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("updateLinkedNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "updateLinkedNotebook: missing result");
    return result;
}

QList< LinkedNotebook > NoteStore::listLinkedNotebooks(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("listLinkedNotebooks", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_listLinkedNotebooks_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("listLinkedNotebooks") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QList< LinkedNotebook > result = QList< LinkedNotebook >();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_LIST) {
                result_isset = true;
                QList< LinkedNotebook > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (listLinkedNotebooks.result)");
                for(quint32 i = 0; i < size; i++) {
                    LinkedNotebook elem;
                    readLinkedNotebook(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "listLinkedNotebooks: missing result");
    return result;
}

qint32 NoteStore::expungeLinkedNotebook(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("expungeLinkedNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_expungeLinkedNotebook_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("expungeLinkedNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    qint32 result = qint32();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_I32) {
                result_isset = true;
                qint32 v;
                r.readI32(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "expungeLinkedNotebook: missing result");
    return result;
}

AuthenticationResult NoteStore::authenticateToSharedNotebook(QString shareKey, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("authenticateToSharedNotebook", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_authenticateToSharedNotebook_pargs");
    w.writeFieldBegin("shareKey", ThriftFieldType::T_STRING, 1);
    w.writeString(shareKey);
    w.writeFieldEnd();
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 2);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("authenticateToSharedNotebook") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    AuthenticationResult result = AuthenticationResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                AuthenticationResult v;
                readAuthenticationResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "authenticateToSharedNotebook: missing result");
    return result;
}

SharedNotebook NoteStore::getSharedNotebookByAuth(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getSharedNotebookByAuth", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_getSharedNotebookByAuth_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getSharedNotebookByAuth") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    SharedNotebook result = SharedNotebook();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                SharedNotebook v;
                readSharedNotebook(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getSharedNotebookByAuth: missing result");
    return result;
}

void NoteStore::emailNote(NoteEmailParameters parameters, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("emailNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_emailNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("parameters", ThriftFieldType::T_STRUCT, 2);
    writeNoteEmailParameters(w, parameters);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("emailNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
}

QString NoteStore::shareNote(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("shareNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_shareNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("shareNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QString result = QString();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QString v;
                r.readString(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "shareNote: missing result");
    return result;
}

void NoteStore::stopSharingNote(Guid guid, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("stopSharingNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_stopSharingNote_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 2);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("stopSharingNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
}

AuthenticationResult NoteStore::authenticateToSharedNote(QString guid, QString noteKey, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("authenticateToSharedNote", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_authenticateToSharedNote_pargs");
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
    w.writeString(guid);
    w.writeFieldEnd();
    w.writeFieldBegin("noteKey", ThriftFieldType::T_STRING, 2);
    w.writeString(noteKey);
    w.writeFieldEnd();
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 3);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("authenticateToSharedNote") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    AuthenticationResult result = AuthenticationResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                AuthenticationResult v;
                readAuthenticationResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "authenticateToSharedNote: missing result");
    return result;
}

RelatedResult NoteStore::findRelated(RelatedQuery query, RelatedResultSpec resultSpec, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("findRelated", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("NoteStore_findRelated_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("query", ThriftFieldType::T_STRUCT, 2);
    writeRelatedQuery(w, query);
    w.writeFieldEnd();
    w.writeFieldBegin("resultSpec", ThriftFieldType::T_STRUCT, 3);
    writeRelatedResultSpec(w, resultSpec);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("findRelated") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    RelatedResult result = RelatedResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                RelatedResult v;
                readRelatedResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "findRelated: missing result");
    return result;
}

bool UserStore::checkVersion(QString clientName, qint16 edamVersionMajor, qint16 edamVersionMinor)
{
    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("checkVersion", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_checkVersion_pargs");
    w.writeFieldBegin("clientName", ThriftFieldType::T_STRING, 1);
    w.writeString(clientName);
    w.writeFieldEnd();
    w.writeFieldBegin("edamVersionMajor", ThriftFieldType::T_I16, 2);
    w.writeI16(edamVersionMajor);
    w.writeFieldEnd();
    w.writeFieldBegin("edamVersionMinor", ThriftFieldType::T_I16, 3);
    w.writeI16(edamVersionMinor);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("checkVersion") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    bool result = bool();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                result_isset = true;
                bool v;
                r.readBool(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "checkVersion: missing result");
    return result;
}

BootstrapInfo UserStore::getBootstrapInfo(QString locale)
{
    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getBootstrapInfo", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_getBootstrapInfo_pargs");
    w.writeFieldBegin("locale", ThriftFieldType::T_STRING, 1);
    w.writeString(locale);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getBootstrapInfo") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    BootstrapInfo result = BootstrapInfo();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                BootstrapInfo v;
                readBootstrapInfo(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getBootstrapInfo: missing result");
    return result;
}

AuthenticationResult UserStore::authenticate(QString username, QString password, QString consumerKey, QString consumerSecret, bool supportsTwoFactor)
{
    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("authenticate", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_authenticate_pargs");
    w.writeFieldBegin("username", ThriftFieldType::T_STRING, 1);
    w.writeString(username);
    w.writeFieldEnd();
    w.writeFieldBegin("password", ThriftFieldType::T_STRING, 2);
    w.writeString(password);
    w.writeFieldEnd();
    w.writeFieldBegin("consumerKey", ThriftFieldType::T_STRING, 3);
    w.writeString(consumerKey);
    w.writeFieldEnd();
    w.writeFieldBegin("consumerSecret", ThriftFieldType::T_STRING, 4);
    w.writeString(consumerSecret);
    w.writeFieldEnd();
    w.writeFieldBegin("supportsTwoFactor", ThriftFieldType::T_BOOL, 5);
    w.writeBool(supportsTwoFactor);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("authenticate") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    AuthenticationResult result = AuthenticationResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                AuthenticationResult v;
                readAuthenticationResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "authenticate: missing result");
    return result;
}

AuthenticationResult UserStore::authenticateLongSession(QString username, QString password, QString consumerKey, QString consumerSecret, QString deviceIdentifier, QString deviceDescription, bool supportsTwoFactor)
{
    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("authenticateLongSession", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_authenticateLongSession_pargs");
    w.writeFieldBegin("username", ThriftFieldType::T_STRING, 1);
    w.writeString(username);
    w.writeFieldEnd();
    w.writeFieldBegin("password", ThriftFieldType::T_STRING, 2);
    w.writeString(password);
    w.writeFieldEnd();
    w.writeFieldBegin("consumerKey", ThriftFieldType::T_STRING, 3);
    w.writeString(consumerKey);
    w.writeFieldEnd();
    w.writeFieldBegin("consumerSecret", ThriftFieldType::T_STRING, 4);
    w.writeString(consumerSecret);
    w.writeFieldEnd();
    w.writeFieldBegin("deviceIdentifier", ThriftFieldType::T_STRING, 5);
    w.writeString(deviceIdentifier);
    w.writeFieldEnd();
    w.writeFieldBegin("deviceDescription", ThriftFieldType::T_STRING, 6);
    w.writeString(deviceDescription);
    w.writeFieldEnd();
    w.writeFieldBegin("supportsTwoFactor", ThriftFieldType::T_BOOL, 7);
    w.writeBool(supportsTwoFactor);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("authenticateLongSession") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    AuthenticationResult result = AuthenticationResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                AuthenticationResult v;
                readAuthenticationResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "authenticateLongSession: missing result");
    return result;
}

AuthenticationResult UserStore::completeTwoFactorAuthentication(QString oneTimeCode, QString deviceIdentifier, QString deviceDescription, QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("completeTwoFactorAuthentication", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_completeTwoFactorAuthentication_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("oneTimeCode", ThriftFieldType::T_STRING, 2);
    w.writeString(oneTimeCode);
    w.writeFieldEnd();
    w.writeFieldBegin("deviceIdentifier", ThriftFieldType::T_STRING, 3);
    w.writeString(deviceIdentifier);
    w.writeFieldEnd();
    w.writeFieldBegin("deviceDescription", ThriftFieldType::T_STRING, 4);
    w.writeString(deviceDescription);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("completeTwoFactorAuthentication") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    AuthenticationResult result = AuthenticationResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                AuthenticationResult v;
                readAuthenticationResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "completeTwoFactorAuthentication: missing result");
    return result;
}

void UserStore::revokeLongSession(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("revokeLongSession", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_revokeLongSession_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("revokeLongSession") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
}

AuthenticationResult UserStore::authenticateToBusiness(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("authenticateToBusiness", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_authenticateToBusiness_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("authenticateToBusiness") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    AuthenticationResult result = AuthenticationResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                AuthenticationResult v;
                readAuthenticationResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "authenticateToBusiness: missing result");
    return result;
}

AuthenticationResult UserStore::refreshAuthentication(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("refreshAuthentication", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_refreshAuthentication_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("refreshAuthentication") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    AuthenticationResult result = AuthenticationResult();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                AuthenticationResult v;
                readAuthenticationResult(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "refreshAuthentication: missing result");
    return result;
}

User UserStore::getUser(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getUser", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_getUser_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getUser") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    User result = User();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                User v;
                readUser(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getUser: missing result");
    return result;
}

PublicUserInfo UserStore::getPublicUserInfo(QString username)
{
    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getPublicUserInfo", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_getPublicUserInfo_pargs");
    w.writeFieldBegin("username", ThriftFieldType::T_STRING, 1);
    w.writeString(username);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getPublicUserInfo") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    PublicUserInfo result = PublicUserInfo();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                PublicUserInfo v;
                readPublicUserInfo(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMNotFoundException e;
                readEDAMNotFoundException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getPublicUserInfo: missing result");
    return result;
}

PremiumInfo UserStore::getPremiumInfo(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getPremiumInfo", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_getPremiumInfo_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getPremiumInfo") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    PremiumInfo result = PremiumInfo();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                result_isset = true;
                PremiumInfo v;
                readPremiumInfo(r, v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getPremiumInfo: missing result");
    return result;
}

QString UserStore::getNoteStoreUrl(QString authenticationToken)
{
    if(authenticationToken.isEmpty()) authenticationToken = this->authenticationToken_;

    ThriftBinaryBufferWriter w;
    qint32 cseqid = 0;
    w.writeMessageBegin("getNoteStoreUrl", ThriftMessageType::T_CALL, cseqid);
    w.writeStructBegin("UserStore_getNoteStoreUrl_pargs");
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 1);
    w.writeString(authenticationToken);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
    w.writeMessageEnd();

    ThriftBinaryBufferReader r(askEvernote(url_, w.buffer()));
    qint32 rseqid = 0;
    QString fname;
    ThriftMessageType::type mtype;
    r.readMessageBegin(fname, mtype, rseqid);
    if (mtype == ThriftMessageType::T_EXCEPTION) {
      ThriftException e;
      e.read(reinterpret_cast<void*>(&r));
      r.readMessageEnd();
      throw e;
    } 
    if (mtype != ThriftMessageType::T_REPLY) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::INVALID_MESSAGE_TYPE);
    }
    if (fname.compare("getNoteStoreUrl") != 0) {
      r.skip(ThriftFieldType::T_STRUCT);
      r.readMessageEnd();
      throw ThriftException(ThriftException::Type::WRONG_METHOD_NAME);
    }

    QString result = QString();
    bool result_isset = false;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 0) {
            if(fieldType == ThriftFieldType::T_STRING) {
                result_isset = true;
                QString v;
                r.readString(v);
                result = v;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMUserException e;
                readEDAMUserException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
       else if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                EDAMSystemException e;
                readEDAMSystemException(r, e);
                throw e;
            } else {
                r.skip(fieldType);
            }
        }
        else {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    r.readMessageEnd();
    if(!result_isset) throw ThriftException(ThriftException::Type::MISSING_RESULT, "getNoteStoreUrl: missing result");
    return result;
}


}
