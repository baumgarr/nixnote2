// This file is generated from Evernote Thrift API and is a part of the QEverCloud project

#include "types.h"
#include "../impl.h"
#include "types_impl.h"

namespace qevercloud {

/** @cond HIDDEN_SYMBOLS  */

void writeSyncState(ThriftBinaryBufferWriter& w, const SyncState& s) {
    w.writeStructBegin("SyncState");
    w.writeFieldBegin("currentTime", ThriftFieldType::T_I64, 1);
    w.writeI64(s.currentTime);
    w.writeFieldEnd();
    w.writeFieldBegin("fullSyncBefore", ThriftFieldType::T_I64, 2);
    w.writeI64(s.fullSyncBefore);
    w.writeFieldEnd();
    w.writeFieldBegin("updateCount", ThriftFieldType::T_I32, 3);
    w.writeI32(s.updateCount);
    w.writeFieldEnd();
    if(s.uploaded.isSet()) {
        w.writeFieldBegin("uploaded", ThriftFieldType::T_I64, 4);
        w.writeI64(s.uploaded.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readSyncState(ThriftBinaryBufferReader& r, SyncState& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool currentTime_isset = false;
    bool fullSyncBefore_isset = false;
    bool updateCount_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I64) {
                currentTime_isset = true;
                qint64 v;
                r.readI64(v);
                s.currentTime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I64) {
                fullSyncBefore_isset = true;
                qint64 v;
                r.readI64(v);
                s.fullSyncBefore = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I32) {
                updateCount_isset = true;
                qint32 v;
                r.readI32(v);
                s.updateCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.uploaded = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!currentTime_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "SyncState.currentTime has no value");
    if(!fullSyncBefore_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "SyncState.fullSyncBefore has no value");
    if(!updateCount_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "SyncState.updateCount has no value");
}

void writeSyncChunk(ThriftBinaryBufferWriter& w, const SyncChunk& s) {
    w.writeStructBegin("SyncChunk");
    w.writeFieldBegin("currentTime", ThriftFieldType::T_I64, 1);
    w.writeI64(s.currentTime);
    w.writeFieldEnd();
    if(s.chunkHighUSN.isSet()) {
        w.writeFieldBegin("chunkHighUSN", ThriftFieldType::T_I32, 2);
        w.writeI32(s.chunkHighUSN.ref());
        w.writeFieldEnd();
    }
    w.writeFieldBegin("updateCount", ThriftFieldType::T_I32, 3);
    w.writeI32(s.updateCount);
    w.writeFieldEnd();
    if(s.notes.isSet()) {
        w.writeFieldBegin("notes", ThriftFieldType::T_LIST, 4);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.notes.ref().length());
        Q_FOREACH(const Note& elem, s.notes.ref()) {
            writeNote(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.notebooks.isSet()) {
        w.writeFieldBegin("notebooks", ThriftFieldType::T_LIST, 5);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.notebooks.ref().length());
        Q_FOREACH(const Notebook& elem, s.notebooks.ref()) {
            writeNotebook(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.tags.isSet()) {
        w.writeFieldBegin("tags", ThriftFieldType::T_LIST, 6);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.tags.ref().length());
        Q_FOREACH(const Tag& elem, s.tags.ref()) {
            writeTag(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.searches.isSet()) {
        w.writeFieldBegin("searches", ThriftFieldType::T_LIST, 7);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.searches.ref().length());
        Q_FOREACH(const SavedSearch& elem, s.searches.ref()) {
            writeSavedSearch(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.resources.isSet()) {
        w.writeFieldBegin("resources", ThriftFieldType::T_LIST, 8);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.resources.ref().length());
        Q_FOREACH(const Resource& elem, s.resources.ref()) {
            writeResource(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.expungedNotes.isSet()) {
        w.writeFieldBegin("expungedNotes", ThriftFieldType::T_LIST, 9);
        w.writeListBegin(ThriftFieldType::T_STRING, s.expungedNotes.ref().length());
        Q_FOREACH(const Guid& elem, s.expungedNotes.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.expungedNotebooks.isSet()) {
        w.writeFieldBegin("expungedNotebooks", ThriftFieldType::T_LIST, 10);
        w.writeListBegin(ThriftFieldType::T_STRING, s.expungedNotebooks.ref().length());
        Q_FOREACH(const Guid& elem, s.expungedNotebooks.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.expungedTags.isSet()) {
        w.writeFieldBegin("expungedTags", ThriftFieldType::T_LIST, 11);
        w.writeListBegin(ThriftFieldType::T_STRING, s.expungedTags.ref().length());
        Q_FOREACH(const Guid& elem, s.expungedTags.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.expungedSearches.isSet()) {
        w.writeFieldBegin("expungedSearches", ThriftFieldType::T_LIST, 12);
        w.writeListBegin(ThriftFieldType::T_STRING, s.expungedSearches.ref().length());
        Q_FOREACH(const Guid& elem, s.expungedSearches.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.linkedNotebooks.isSet()) {
        w.writeFieldBegin("linkedNotebooks", ThriftFieldType::T_LIST, 13);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.linkedNotebooks.ref().length());
        Q_FOREACH(const LinkedNotebook& elem, s.linkedNotebooks.ref()) {
            writeLinkedNotebook(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.expungedLinkedNotebooks.isSet()) {
        w.writeFieldBegin("expungedLinkedNotebooks", ThriftFieldType::T_LIST, 14);
        w.writeListBegin(ThriftFieldType::T_STRING, s.expungedLinkedNotebooks.ref().length());
        Q_FOREACH(const Guid& elem, s.expungedLinkedNotebooks.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readSyncChunk(ThriftBinaryBufferReader& r, SyncChunk& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool currentTime_isset = false;
    bool updateCount_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I64) {
                currentTime_isset = true;
                qint64 v;
                r.readI64(v);
                s.currentTime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.chunkHighUSN = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I32) {
                updateCount_isset = true;
                qint32 v;
                r.readI32(v);
                s.updateCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Note > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.notes)");
                for(quint32 i = 0; i < size; i++) {
                    Note elem;
                    readNote(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.notes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Notebook > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.notebooks)");
                for(quint32 i = 0; i < size; i++) {
                    Notebook elem;
                    readNotebook(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.notebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Tag > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.tags)");
                for(quint32 i = 0; i < size; i++) {
                    Tag elem;
                    readTag(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.tags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< SavedSearch > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.searches)");
                for(quint32 i = 0; i < size; i++) {
                    SavedSearch elem;
                    readSavedSearch(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.searches = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Resource > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.resources)");
                for(quint32 i = 0; i < size; i++) {
                    Resource elem;
                    readResource(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.resources = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.expungedNotes)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.expungedNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.expungedNotebooks)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.expungedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.expungedTags)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.expungedTags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.expungedSearches)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.expungedSearches = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< LinkedNotebook > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.linkedNotebooks)");
                for(quint32 i = 0; i < size; i++) {
                    LinkedNotebook elem;
                    readLinkedNotebook(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.linkedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (SyncChunk.expungedLinkedNotebooks)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.expungedLinkedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!currentTime_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "SyncChunk.currentTime has no value");
    if(!updateCount_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "SyncChunk.updateCount has no value");
}

void writeSyncChunkFilter(ThriftBinaryBufferWriter& w, const SyncChunkFilter& s) {
    w.writeStructBegin("SyncChunkFilter");
    if(s.includeNotes.isSet()) {
        w.writeFieldBegin("includeNotes", ThriftFieldType::T_BOOL, 1);
        w.writeBool(s.includeNotes.ref());
        w.writeFieldEnd();
    }
    if(s.includeNoteResources.isSet()) {
        w.writeFieldBegin("includeNoteResources", ThriftFieldType::T_BOOL, 2);
        w.writeBool(s.includeNoteResources.ref());
        w.writeFieldEnd();
    }
    if(s.includeNoteAttributes.isSet()) {
        w.writeFieldBegin("includeNoteAttributes", ThriftFieldType::T_BOOL, 3);
        w.writeBool(s.includeNoteAttributes.ref());
        w.writeFieldEnd();
    }
    if(s.includeNotebooks.isSet()) {
        w.writeFieldBegin("includeNotebooks", ThriftFieldType::T_BOOL, 4);
        w.writeBool(s.includeNotebooks.ref());
        w.writeFieldEnd();
    }
    if(s.includeTags.isSet()) {
        w.writeFieldBegin("includeTags", ThriftFieldType::T_BOOL, 5);
        w.writeBool(s.includeTags.ref());
        w.writeFieldEnd();
    }
    if(s.includeSearches.isSet()) {
        w.writeFieldBegin("includeSearches", ThriftFieldType::T_BOOL, 6);
        w.writeBool(s.includeSearches.ref());
        w.writeFieldEnd();
    }
    if(s.includeResources.isSet()) {
        w.writeFieldBegin("includeResources", ThriftFieldType::T_BOOL, 7);
        w.writeBool(s.includeResources.ref());
        w.writeFieldEnd();
    }
    if(s.includeLinkedNotebooks.isSet()) {
        w.writeFieldBegin("includeLinkedNotebooks", ThriftFieldType::T_BOOL, 8);
        w.writeBool(s.includeLinkedNotebooks.ref());
        w.writeFieldEnd();
    }
    if(s.includeExpunged.isSet()) {
        w.writeFieldBegin("includeExpunged", ThriftFieldType::T_BOOL, 9);
        w.writeBool(s.includeExpunged.ref());
        w.writeFieldEnd();
    }
    if(s.includeNoteApplicationDataFullMap.isSet()) {
        w.writeFieldBegin("includeNoteApplicationDataFullMap", ThriftFieldType::T_BOOL, 10);
        w.writeBool(s.includeNoteApplicationDataFullMap.ref());
        w.writeFieldEnd();
    }
    if(s.includeResourceApplicationDataFullMap.isSet()) {
        w.writeFieldBegin("includeResourceApplicationDataFullMap", ThriftFieldType::T_BOOL, 12);
        w.writeBool(s.includeResourceApplicationDataFullMap.ref());
        w.writeFieldEnd();
    }
    if(s.includeNoteResourceApplicationDataFullMap.isSet()) {
        w.writeFieldBegin("includeNoteResourceApplicationDataFullMap", ThriftFieldType::T_BOOL, 13);
        w.writeBool(s.includeNoteResourceApplicationDataFullMap.ref());
        w.writeFieldEnd();
    }
    if(s.requireNoteContentClass.isSet()) {
        w.writeFieldBegin("requireNoteContentClass", ThriftFieldType::T_STRING, 11);
        w.writeString(s.requireNoteContentClass.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readSyncChunkFilter(ThriftBinaryBufferReader& r, SyncChunkFilter& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeNoteResources = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeNoteAttributes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeTags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeSearches = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeResources = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeLinkedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeExpunged = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeNoteApplicationDataFullMap = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeResourceApplicationDataFullMap = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeNoteResourceApplicationDataFullMap = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.requireNoteContentClass = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNoteFilter(ThriftBinaryBufferWriter& w, const NoteFilter& s) {
    w.writeStructBegin("NoteFilter");
    if(s.order.isSet()) {
        w.writeFieldBegin("order", ThriftFieldType::T_I32, 1);
        w.writeI32(s.order.ref());
        w.writeFieldEnd();
    }
    if(s.ascending.isSet()) {
        w.writeFieldBegin("ascending", ThriftFieldType::T_BOOL, 2);
        w.writeBool(s.ascending.ref());
        w.writeFieldEnd();
    }
    if(s.words.isSet()) {
        w.writeFieldBegin("words", ThriftFieldType::T_STRING, 3);
        w.writeString(s.words.ref());
        w.writeFieldEnd();
    }
    if(s.notebookGuid.isSet()) {
        w.writeFieldBegin("notebookGuid", ThriftFieldType::T_STRING, 4);
        w.writeString(s.notebookGuid.ref());
        w.writeFieldEnd();
    }
    if(s.tagGuids.isSet()) {
        w.writeFieldBegin("tagGuids", ThriftFieldType::T_LIST, 5);
        w.writeListBegin(ThriftFieldType::T_STRING, s.tagGuids.ref().length());
        Q_FOREACH(const Guid& elem, s.tagGuids.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.timeZone.isSet()) {
        w.writeFieldBegin("timeZone", ThriftFieldType::T_STRING, 6);
        w.writeString(s.timeZone.ref());
        w.writeFieldEnd();
    }
    if(s.inactive.isSet()) {
        w.writeFieldBegin("inactive", ThriftFieldType::T_BOOL, 7);
        w.writeBool(s.inactive.ref());
        w.writeFieldEnd();
    }
    if(s.emphasized.isSet()) {
        w.writeFieldBegin("emphasized", ThriftFieldType::T_STRING, 8);
        w.writeString(s.emphasized.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNoteFilter(ThriftBinaryBufferReader& r, NoteFilter& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.order = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.ascending = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.words = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.notebookGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NoteFilter.tagGuids)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.tagGuids = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.timeZone = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.inactive = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.emphasized = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNoteList(ThriftBinaryBufferWriter& w, const NoteList& s) {
    w.writeStructBegin("NoteList");
    w.writeFieldBegin("startIndex", ThriftFieldType::T_I32, 1);
    w.writeI32(s.startIndex);
    w.writeFieldEnd();
    w.writeFieldBegin("totalNotes", ThriftFieldType::T_I32, 2);
    w.writeI32(s.totalNotes);
    w.writeFieldEnd();
    w.writeFieldBegin("notes", ThriftFieldType::T_LIST, 3);
    w.writeListBegin(ThriftFieldType::T_STRUCT, s.notes.length());
    Q_FOREACH(const Note& elem, s.notes) {
        writeNote(w, elem);
    }
    w.writeListEnd();
    w.writeFieldEnd();
    if(s.stoppedWords.isSet()) {
        w.writeFieldBegin("stoppedWords", ThriftFieldType::T_LIST, 4);
        w.writeListBegin(ThriftFieldType::T_STRING, s.stoppedWords.ref().length());
        Q_FOREACH(const QString& elem, s.stoppedWords.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.searchedWords.isSet()) {
        w.writeFieldBegin("searchedWords", ThriftFieldType::T_LIST, 5);
        w.writeListBegin(ThriftFieldType::T_STRING, s.searchedWords.ref().length());
        Q_FOREACH(const QString& elem, s.searchedWords.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.updateCount.isSet()) {
        w.writeFieldBegin("updateCount", ThriftFieldType::T_I32, 6);
        w.writeI32(s.updateCount.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNoteList(ThriftBinaryBufferReader& r, NoteList& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool startIndex_isset = false;
    bool totalNotes_isset = false;
    bool notes_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                startIndex_isset = true;
                qint32 v;
                r.readI32(v);
                s.startIndex = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                totalNotes_isset = true;
                qint32 v;
                r.readI32(v);
                s.totalNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_LIST) {
                notes_isset = true;
                QList< Note > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NoteList.notes)");
                for(quint32 i = 0; i < size; i++) {
                    Note elem;
                    readNote(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.notes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NoteList.stoppedWords)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.stoppedWords = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NoteList.searchedWords)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.searchedWords = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!startIndex_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteList.startIndex has no value");
    if(!totalNotes_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteList.totalNotes has no value");
    if(!notes_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteList.notes has no value");
}

void writeNoteMetadata(ThriftBinaryBufferWriter& w, const NoteMetadata& s) {
    w.writeStructBegin("NoteMetadata");
    w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
    w.writeString(s.guid);
    w.writeFieldEnd();
    if(s.title.isSet()) {
        w.writeFieldBegin("title", ThriftFieldType::T_STRING, 2);
        w.writeString(s.title.ref());
        w.writeFieldEnd();
    }
    if(s.contentLength.isSet()) {
        w.writeFieldBegin("contentLength", ThriftFieldType::T_I32, 5);
        w.writeI32(s.contentLength.ref());
        w.writeFieldEnd();
    }
    if(s.created.isSet()) {
        w.writeFieldBegin("created", ThriftFieldType::T_I64, 6);
        w.writeI64(s.created.ref());
        w.writeFieldEnd();
    }
    if(s.updated.isSet()) {
        w.writeFieldBegin("updated", ThriftFieldType::T_I64, 7);
        w.writeI64(s.updated.ref());
        w.writeFieldEnd();
    }
    if(s.deleted.isSet()) {
        w.writeFieldBegin("deleted", ThriftFieldType::T_I64, 8);
        w.writeI64(s.deleted.ref());
        w.writeFieldEnd();
    }
    if(s.updateSequenceNum.isSet()) {
        w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 10);
        w.writeI32(s.updateSequenceNum.ref());
        w.writeFieldEnd();
    }
    if(s.notebookGuid.isSet()) {
        w.writeFieldBegin("notebookGuid", ThriftFieldType::T_STRING, 11);
        w.writeString(s.notebookGuid.ref());
        w.writeFieldEnd();
    }
    if(s.tagGuids.isSet()) {
        w.writeFieldBegin("tagGuids", ThriftFieldType::T_LIST, 12);
        w.writeListBegin(ThriftFieldType::T_STRING, s.tagGuids.ref().length());
        Q_FOREACH(const Guid& elem, s.tagGuids.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.attributes.isSet()) {
        w.writeFieldBegin("attributes", ThriftFieldType::T_STRUCT, 14);
        writeNoteAttributes(w, s.attributes.ref());
        w.writeFieldEnd();
    }
    if(s.largestResourceMime.isSet()) {
        w.writeFieldBegin("largestResourceMime", ThriftFieldType::T_STRING, 20);
        w.writeString(s.largestResourceMime.ref());
        w.writeFieldEnd();
    }
    if(s.largestResourceSize.isSet()) {
        w.writeFieldBegin("largestResourceSize", ThriftFieldType::T_I32, 21);
        w.writeI32(s.largestResourceSize.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNoteMetadata(ThriftBinaryBufferReader& r, NoteMetadata& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool guid_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                guid_isset = true;
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.title = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.contentLength = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.created = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.updated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.deleted = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.notebookGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NoteMetadata.tagGuids)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.tagGuids = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                NoteAttributes v;
                readNoteAttributes(r, v);
                s.attributes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 20) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.largestResourceMime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 21) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.largestResourceSize = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!guid_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteMetadata.guid has no value");
}

void writeNotesMetadataList(ThriftBinaryBufferWriter& w, const NotesMetadataList& s) {
    w.writeStructBegin("NotesMetadataList");
    w.writeFieldBegin("startIndex", ThriftFieldType::T_I32, 1);
    w.writeI32(s.startIndex);
    w.writeFieldEnd();
    w.writeFieldBegin("totalNotes", ThriftFieldType::T_I32, 2);
    w.writeI32(s.totalNotes);
    w.writeFieldEnd();
    w.writeFieldBegin("notes", ThriftFieldType::T_LIST, 3);
    w.writeListBegin(ThriftFieldType::T_STRUCT, s.notes.length());
    Q_FOREACH(const NoteMetadata& elem, s.notes) {
        writeNoteMetadata(w, elem);
    }
    w.writeListEnd();
    w.writeFieldEnd();
    if(s.stoppedWords.isSet()) {
        w.writeFieldBegin("stoppedWords", ThriftFieldType::T_LIST, 4);
        w.writeListBegin(ThriftFieldType::T_STRING, s.stoppedWords.ref().length());
        Q_FOREACH(const QString& elem, s.stoppedWords.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.searchedWords.isSet()) {
        w.writeFieldBegin("searchedWords", ThriftFieldType::T_LIST, 5);
        w.writeListBegin(ThriftFieldType::T_STRING, s.searchedWords.ref().length());
        Q_FOREACH(const QString& elem, s.searchedWords.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.updateCount.isSet()) {
        w.writeFieldBegin("updateCount", ThriftFieldType::T_I32, 6);
        w.writeI32(s.updateCount.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNotesMetadataList(ThriftBinaryBufferReader& r, NotesMetadataList& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool startIndex_isset = false;
    bool totalNotes_isset = false;
    bool notes_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                startIndex_isset = true;
                qint32 v;
                r.readI32(v);
                s.startIndex = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                totalNotes_isset = true;
                qint32 v;
                r.readI32(v);
                s.totalNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_LIST) {
                notes_isset = true;
                QList< NoteMetadata > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NotesMetadataList.notes)");
                for(quint32 i = 0; i < size; i++) {
                    NoteMetadata elem;
                    readNoteMetadata(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.notes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NotesMetadataList.stoppedWords)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.stoppedWords = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NotesMetadataList.searchedWords)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.searchedWords = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!startIndex_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NotesMetadataList.startIndex has no value");
    if(!totalNotes_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NotesMetadataList.totalNotes has no value");
    if(!notes_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NotesMetadataList.notes has no value");
}

void writeNotesMetadataResultSpec(ThriftBinaryBufferWriter& w, const NotesMetadataResultSpec& s) {
    w.writeStructBegin("NotesMetadataResultSpec");
    if(s.includeTitle.isSet()) {
        w.writeFieldBegin("includeTitle", ThriftFieldType::T_BOOL, 2);
        w.writeBool(s.includeTitle.ref());
        w.writeFieldEnd();
    }
    if(s.includeContentLength.isSet()) {
        w.writeFieldBegin("includeContentLength", ThriftFieldType::T_BOOL, 5);
        w.writeBool(s.includeContentLength.ref());
        w.writeFieldEnd();
    }
    if(s.includeCreated.isSet()) {
        w.writeFieldBegin("includeCreated", ThriftFieldType::T_BOOL, 6);
        w.writeBool(s.includeCreated.ref());
        w.writeFieldEnd();
    }
    if(s.includeUpdated.isSet()) {
        w.writeFieldBegin("includeUpdated", ThriftFieldType::T_BOOL, 7);
        w.writeBool(s.includeUpdated.ref());
        w.writeFieldEnd();
    }
    if(s.includeDeleted.isSet()) {
        w.writeFieldBegin("includeDeleted", ThriftFieldType::T_BOOL, 8);
        w.writeBool(s.includeDeleted.ref());
        w.writeFieldEnd();
    }
    if(s.includeUpdateSequenceNum.isSet()) {
        w.writeFieldBegin("includeUpdateSequenceNum", ThriftFieldType::T_BOOL, 10);
        w.writeBool(s.includeUpdateSequenceNum.ref());
        w.writeFieldEnd();
    }
    if(s.includeNotebookGuid.isSet()) {
        w.writeFieldBegin("includeNotebookGuid", ThriftFieldType::T_BOOL, 11);
        w.writeBool(s.includeNotebookGuid.ref());
        w.writeFieldEnd();
    }
    if(s.includeTagGuids.isSet()) {
        w.writeFieldBegin("includeTagGuids", ThriftFieldType::T_BOOL, 12);
        w.writeBool(s.includeTagGuids.ref());
        w.writeFieldEnd();
    }
    if(s.includeAttributes.isSet()) {
        w.writeFieldBegin("includeAttributes", ThriftFieldType::T_BOOL, 14);
        w.writeBool(s.includeAttributes.ref());
        w.writeFieldEnd();
    }
    if(s.includeLargestResourceMime.isSet()) {
        w.writeFieldBegin("includeLargestResourceMime", ThriftFieldType::T_BOOL, 20);
        w.writeBool(s.includeLargestResourceMime.ref());
        w.writeFieldEnd();
    }
    if(s.includeLargestResourceSize.isSet()) {
        w.writeFieldBegin("includeLargestResourceSize", ThriftFieldType::T_BOOL, 21);
        w.writeBool(s.includeLargestResourceSize.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNotesMetadataResultSpec(ThriftBinaryBufferReader& r, NotesMetadataResultSpec& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeTitle = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeContentLength = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeCreated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeUpdated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeDeleted = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeUpdateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeNotebookGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeTagGuids = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeAttributes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 20) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeLargestResourceMime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 21) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeLargestResourceSize = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNoteCollectionCounts(ThriftBinaryBufferWriter& w, const NoteCollectionCounts& s) {
    w.writeStructBegin("NoteCollectionCounts");
    if(s.notebookCounts.isSet()) {
        w.writeFieldBegin("notebookCounts", ThriftFieldType::T_MAP, 1);
        w.writeMapBegin(ThriftFieldType::T_STRING, ThriftFieldType::T_I32, s.notebookCounts.ref().keys().length());
        Q_FOREACH(const Guid& elem, s.notebookCounts.ref().keys()) {
            w.writeString(elem);
            w.writeI32(s.notebookCounts.ref().value(elem));
        }
        w.writeMapEnd();
        w.writeFieldEnd();
    }
    if(s.tagCounts.isSet()) {
        w.writeFieldBegin("tagCounts", ThriftFieldType::T_MAP, 2);
        w.writeMapBegin(ThriftFieldType::T_STRING, ThriftFieldType::T_I32, s.tagCounts.ref().keys().length());
        Q_FOREACH(const Guid& elem, s.tagCounts.ref().keys()) {
            w.writeString(elem);
            w.writeI32(s.tagCounts.ref().value(elem));
        }
        w.writeMapEnd();
        w.writeFieldEnd();
    }
    if(s.trashCount.isSet()) {
        w.writeFieldBegin("trashCount", ThriftFieldType::T_I32, 3);
        w.writeI32(s.trashCount.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNoteCollectionCounts(ThriftBinaryBufferReader& r, NoteCollectionCounts& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_MAP) {
                QMap< Guid, qint32 > v;
                quint32 size;
                ThriftFieldType::type keyType;
                ThriftFieldType::type elemType;
                r.readMapBegin(keyType, elemType, size);
                if(keyType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map key type (NoteCollectionCounts.notebookCounts)");
                if(elemType != ThriftFieldType::T_I32) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map value type (NoteCollectionCounts.notebookCounts)");
                for(quint32 i = 0; i < size; i++) {
                    Guid key;
                    r.readString(key);
                    qint32 value;
                    r.readI32(value);
                    v[key] = value;
                }
                r.readMapEnd();
                s.notebookCounts = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_MAP) {
                QMap< Guid, qint32 > v;
                quint32 size;
                ThriftFieldType::type keyType;
                ThriftFieldType::type elemType;
                r.readMapBegin(keyType, elemType, size);
                if(keyType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map key type (NoteCollectionCounts.tagCounts)");
                if(elemType != ThriftFieldType::T_I32) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map value type (NoteCollectionCounts.tagCounts)");
                for(quint32 i = 0; i < size; i++) {
                    Guid key;
                    r.readString(key);
                    qint32 value;
                    r.readI32(value);
                    v[key] = value;
                }
                r.readMapEnd();
                s.tagCounts = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.trashCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNoteEmailParameters(ThriftBinaryBufferWriter& w, const NoteEmailParameters& s) {
    w.writeStructBegin("NoteEmailParameters");
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.note.isSet()) {
        w.writeFieldBegin("note", ThriftFieldType::T_STRUCT, 2);
        writeNote(w, s.note.ref());
        w.writeFieldEnd();
    }
    if(s.toAddresses.isSet()) {
        w.writeFieldBegin("toAddresses", ThriftFieldType::T_LIST, 3);
        w.writeListBegin(ThriftFieldType::T_STRING, s.toAddresses.ref().length());
        Q_FOREACH(const QString& elem, s.toAddresses.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.ccAddresses.isSet()) {
        w.writeFieldBegin("ccAddresses", ThriftFieldType::T_LIST, 4);
        w.writeListBegin(ThriftFieldType::T_STRING, s.ccAddresses.ref().length());
        Q_FOREACH(const QString& elem, s.ccAddresses.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.subject.isSet()) {
        w.writeFieldBegin("subject", ThriftFieldType::T_STRING, 5);
        w.writeString(s.subject.ref());
        w.writeFieldEnd();
    }
    if(s.message.isSet()) {
        w.writeFieldBegin("message", ThriftFieldType::T_STRING, 6);
        w.writeString(s.message.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNoteEmailParameters(ThriftBinaryBufferReader& r, NoteEmailParameters& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                Note v;
                readNote(r, v);
                s.note = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NoteEmailParameters.toAddresses)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.toAddresses = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (NoteEmailParameters.ccAddresses)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.ccAddresses = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.subject = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.message = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNoteVersionId(ThriftBinaryBufferWriter& w, const NoteVersionId& s) {
    w.writeStructBegin("NoteVersionId");
    w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 1);
    w.writeI32(s.updateSequenceNum);
    w.writeFieldEnd();
    w.writeFieldBegin("updated", ThriftFieldType::T_I64, 2);
    w.writeI64(s.updated);
    w.writeFieldEnd();
    w.writeFieldBegin("saved", ThriftFieldType::T_I64, 3);
    w.writeI64(s.saved);
    w.writeFieldEnd();
    w.writeFieldBegin("title", ThriftFieldType::T_STRING, 4);
    w.writeString(s.title);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNoteVersionId(ThriftBinaryBufferReader& r, NoteVersionId& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool updateSequenceNum_isset = false;
    bool updated_isset = false;
    bool saved_isset = false;
    bool title_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                updateSequenceNum_isset = true;
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I64) {
                updated_isset = true;
                qint64 v;
                r.readI64(v);
                s.updated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I64) {
                saved_isset = true;
                qint64 v;
                r.readI64(v);
                s.saved = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                title_isset = true;
                QString v;
                r.readString(v);
                s.title = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!updateSequenceNum_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteVersionId.updateSequenceNum has no value");
    if(!updated_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteVersionId.updated has no value");
    if(!saved_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteVersionId.saved has no value");
    if(!title_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "NoteVersionId.title has no value");
}

void writeClientUsageMetrics(ThriftBinaryBufferWriter& w, const ClientUsageMetrics& s) {
    w.writeStructBegin("ClientUsageMetrics");
    if(s.sessions.isSet()) {
        w.writeFieldBegin("sessions", ThriftFieldType::T_I32, 1);
        w.writeI32(s.sessions.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readClientUsageMetrics(ThriftBinaryBufferReader& r, ClientUsageMetrics& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.sessions = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeRelatedQuery(ThriftBinaryBufferWriter& w, const RelatedQuery& s) {
    w.writeStructBegin("RelatedQuery");
    if(s.noteGuid.isSet()) {
        w.writeFieldBegin("noteGuid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.noteGuid.ref());
        w.writeFieldEnd();
    }
    if(s.plainText.isSet()) {
        w.writeFieldBegin("plainText", ThriftFieldType::T_STRING, 2);
        w.writeString(s.plainText.ref());
        w.writeFieldEnd();
    }
    if(s.filter.isSet()) {
        w.writeFieldBegin("filter", ThriftFieldType::T_STRUCT, 3);
        writeNoteFilter(w, s.filter.ref());
        w.writeFieldEnd();
    }
    if(s.referenceUri.isSet()) {
        w.writeFieldBegin("referenceUri", ThriftFieldType::T_STRING, 4);
        w.writeString(s.referenceUri.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readRelatedQuery(ThriftBinaryBufferReader& r, RelatedQuery& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.noteGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.plainText = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                NoteFilter v;
                readNoteFilter(r, v);
                s.filter = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.referenceUri = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeRelatedResult(ThriftBinaryBufferWriter& w, const RelatedResult& s) {
    w.writeStructBegin("RelatedResult");
    if(s.notes.isSet()) {
        w.writeFieldBegin("notes", ThriftFieldType::T_LIST, 1);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.notes.ref().length());
        Q_FOREACH(const Note& elem, s.notes.ref()) {
            writeNote(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.notebooks.isSet()) {
        w.writeFieldBegin("notebooks", ThriftFieldType::T_LIST, 2);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.notebooks.ref().length());
        Q_FOREACH(const Notebook& elem, s.notebooks.ref()) {
            writeNotebook(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.tags.isSet()) {
        w.writeFieldBegin("tags", ThriftFieldType::T_LIST, 3);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.tags.ref().length());
        Q_FOREACH(const Tag& elem, s.tags.ref()) {
            writeTag(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.containingNotebooks.isSet()) {
        w.writeFieldBegin("containingNotebooks", ThriftFieldType::T_LIST, 4);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.containingNotebooks.ref().length());
        Q_FOREACH(const NotebookDescriptor& elem, s.containingNotebooks.ref()) {
            writeNotebookDescriptor(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readRelatedResult(ThriftBinaryBufferReader& r, RelatedResult& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Note > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (RelatedResult.notes)");
                for(quint32 i = 0; i < size; i++) {
                    Note elem;
                    readNote(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.notes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Notebook > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (RelatedResult.notebooks)");
                for(quint32 i = 0; i < size; i++) {
                    Notebook elem;
                    readNotebook(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.notebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Tag > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (RelatedResult.tags)");
                for(quint32 i = 0; i < size; i++) {
                    Tag elem;
                    readTag(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.tags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< NotebookDescriptor > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (RelatedResult.containingNotebooks)");
                for(quint32 i = 0; i < size; i++) {
                    NotebookDescriptor elem;
                    readNotebookDescriptor(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.containingNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeRelatedResultSpec(ThriftBinaryBufferWriter& w, const RelatedResultSpec& s) {
    w.writeStructBegin("RelatedResultSpec");
    if(s.maxNotes.isSet()) {
        w.writeFieldBegin("maxNotes", ThriftFieldType::T_I32, 1);
        w.writeI32(s.maxNotes.ref());
        w.writeFieldEnd();
    }
    if(s.maxNotebooks.isSet()) {
        w.writeFieldBegin("maxNotebooks", ThriftFieldType::T_I32, 2);
        w.writeI32(s.maxNotebooks.ref());
        w.writeFieldEnd();
    }
    if(s.maxTags.isSet()) {
        w.writeFieldBegin("maxTags", ThriftFieldType::T_I32, 3);
        w.writeI32(s.maxTags.ref());
        w.writeFieldEnd();
    }
    if(s.writableNotebooksOnly.isSet()) {
        w.writeFieldBegin("writableNotebooksOnly", ThriftFieldType::T_BOOL, 4);
        w.writeBool(s.writableNotebooksOnly.ref());
        w.writeFieldEnd();
    }
    if(s.includeContainingNotebooks.isSet()) {
        w.writeFieldBegin("includeContainingNotebooks", ThriftFieldType::T_BOOL, 5);
        w.writeBool(s.includeContainingNotebooks.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readRelatedResultSpec(ThriftBinaryBufferReader& r, RelatedResultSpec& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.maxNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.maxNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.maxTags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.writableNotebooksOnly = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeContainingNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeData(ThriftBinaryBufferWriter& w, const Data& s) {
    w.writeStructBegin("Data");
    if(s.bodyHash.isSet()) {
        w.writeFieldBegin("bodyHash", ThriftFieldType::T_STRING, 1);
        w.writeBinary(s.bodyHash.ref());
        w.writeFieldEnd();
    }
    if(s.size.isSet()) {
        w.writeFieldBegin("size", ThriftFieldType::T_I32, 2);
        w.writeI32(s.size.ref());
        w.writeFieldEnd();
    }
    if(s.body.isSet()) {
        w.writeFieldBegin("body", ThriftFieldType::T_STRING, 3);
        w.writeBinary(s.body.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readData(ThriftBinaryBufferReader& r, Data& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QByteArray v;
                r.readBinary(v);
                s.bodyHash = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.size = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QByteArray v;
                r.readBinary(v);
                s.body = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeUserAttributes(ThriftBinaryBufferWriter& w, const UserAttributes& s) {
    w.writeStructBegin("UserAttributes");
    if(s.defaultLocationName.isSet()) {
        w.writeFieldBegin("defaultLocationName", ThriftFieldType::T_STRING, 1);
        w.writeString(s.defaultLocationName.ref());
        w.writeFieldEnd();
    }
    if(s.defaultLatitude.isSet()) {
        w.writeFieldBegin("defaultLatitude", ThriftFieldType::T_DOUBLE, 2);
        w.writeDouble(s.defaultLatitude.ref());
        w.writeFieldEnd();
    }
    if(s.defaultLongitude.isSet()) {
        w.writeFieldBegin("defaultLongitude", ThriftFieldType::T_DOUBLE, 3);
        w.writeDouble(s.defaultLongitude.ref());
        w.writeFieldEnd();
    }
    if(s.preactivation.isSet()) {
        w.writeFieldBegin("preactivation", ThriftFieldType::T_BOOL, 4);
        w.writeBool(s.preactivation.ref());
        w.writeFieldEnd();
    }
    if(s.viewedPromotions.isSet()) {
        w.writeFieldBegin("viewedPromotions", ThriftFieldType::T_LIST, 5);
        w.writeListBegin(ThriftFieldType::T_STRING, s.viewedPromotions.ref().length());
        Q_FOREACH(const QString& elem, s.viewedPromotions.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.incomingEmailAddress.isSet()) {
        w.writeFieldBegin("incomingEmailAddress", ThriftFieldType::T_STRING, 6);
        w.writeString(s.incomingEmailAddress.ref());
        w.writeFieldEnd();
    }
    if(s.recentMailedAddresses.isSet()) {
        w.writeFieldBegin("recentMailedAddresses", ThriftFieldType::T_LIST, 7);
        w.writeListBegin(ThriftFieldType::T_STRING, s.recentMailedAddresses.ref().length());
        Q_FOREACH(const QString& elem, s.recentMailedAddresses.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.comments.isSet()) {
        w.writeFieldBegin("comments", ThriftFieldType::T_STRING, 9);
        w.writeString(s.comments.ref());
        w.writeFieldEnd();
    }
    if(s.dateAgreedToTermsOfService.isSet()) {
        w.writeFieldBegin("dateAgreedToTermsOfService", ThriftFieldType::T_I64, 11);
        w.writeI64(s.dateAgreedToTermsOfService.ref());
        w.writeFieldEnd();
    }
    if(s.maxReferrals.isSet()) {
        w.writeFieldBegin("maxReferrals", ThriftFieldType::T_I32, 12);
        w.writeI32(s.maxReferrals.ref());
        w.writeFieldEnd();
    }
    if(s.referralCount.isSet()) {
        w.writeFieldBegin("referralCount", ThriftFieldType::T_I32, 13);
        w.writeI32(s.referralCount.ref());
        w.writeFieldEnd();
    }
    if(s.refererCode.isSet()) {
        w.writeFieldBegin("refererCode", ThriftFieldType::T_STRING, 14);
        w.writeString(s.refererCode.ref());
        w.writeFieldEnd();
    }
    if(s.sentEmailDate.isSet()) {
        w.writeFieldBegin("sentEmailDate", ThriftFieldType::T_I64, 15);
        w.writeI64(s.sentEmailDate.ref());
        w.writeFieldEnd();
    }
    if(s.sentEmailCount.isSet()) {
        w.writeFieldBegin("sentEmailCount", ThriftFieldType::T_I32, 16);
        w.writeI32(s.sentEmailCount.ref());
        w.writeFieldEnd();
    }
    if(s.dailyEmailLimit.isSet()) {
        w.writeFieldBegin("dailyEmailLimit", ThriftFieldType::T_I32, 17);
        w.writeI32(s.dailyEmailLimit.ref());
        w.writeFieldEnd();
    }
    if(s.emailOptOutDate.isSet()) {
        w.writeFieldBegin("emailOptOutDate", ThriftFieldType::T_I64, 18);
        w.writeI64(s.emailOptOutDate.ref());
        w.writeFieldEnd();
    }
    if(s.partnerEmailOptInDate.isSet()) {
        w.writeFieldBegin("partnerEmailOptInDate", ThriftFieldType::T_I64, 19);
        w.writeI64(s.partnerEmailOptInDate.ref());
        w.writeFieldEnd();
    }
    if(s.preferredLanguage.isSet()) {
        w.writeFieldBegin("preferredLanguage", ThriftFieldType::T_STRING, 20);
        w.writeString(s.preferredLanguage.ref());
        w.writeFieldEnd();
    }
    if(s.preferredCountry.isSet()) {
        w.writeFieldBegin("preferredCountry", ThriftFieldType::T_STRING, 21);
        w.writeString(s.preferredCountry.ref());
        w.writeFieldEnd();
    }
    if(s.clipFullPage.isSet()) {
        w.writeFieldBegin("clipFullPage", ThriftFieldType::T_BOOL, 22);
        w.writeBool(s.clipFullPage.ref());
        w.writeFieldEnd();
    }
    if(s.twitterUserName.isSet()) {
        w.writeFieldBegin("twitterUserName", ThriftFieldType::T_STRING, 23);
        w.writeString(s.twitterUserName.ref());
        w.writeFieldEnd();
    }
    if(s.twitterId.isSet()) {
        w.writeFieldBegin("twitterId", ThriftFieldType::T_STRING, 24);
        w.writeString(s.twitterId.ref());
        w.writeFieldEnd();
    }
    if(s.groupName.isSet()) {
        w.writeFieldBegin("groupName", ThriftFieldType::T_STRING, 25);
        w.writeString(s.groupName.ref());
        w.writeFieldEnd();
    }
    if(s.recognitionLanguage.isSet()) {
        w.writeFieldBegin("recognitionLanguage", ThriftFieldType::T_STRING, 26);
        w.writeString(s.recognitionLanguage.ref());
        w.writeFieldEnd();
    }
    if(s.referralProof.isSet()) {
        w.writeFieldBegin("referralProof", ThriftFieldType::T_STRING, 28);
        w.writeString(s.referralProof.ref());
        w.writeFieldEnd();
    }
    if(s.educationalDiscount.isSet()) {
        w.writeFieldBegin("educationalDiscount", ThriftFieldType::T_BOOL, 29);
        w.writeBool(s.educationalDiscount.ref());
        w.writeFieldEnd();
    }
    if(s.businessAddress.isSet()) {
        w.writeFieldBegin("businessAddress", ThriftFieldType::T_STRING, 30);
        w.writeString(s.businessAddress.ref());
        w.writeFieldEnd();
    }
    if(s.hideSponsorBilling.isSet()) {
        w.writeFieldBegin("hideSponsorBilling", ThriftFieldType::T_BOOL, 31);
        w.writeBool(s.hideSponsorBilling.ref());
        w.writeFieldEnd();
    }
    if(s.taxExempt.isSet()) {
        w.writeFieldBegin("taxExempt", ThriftFieldType::T_BOOL, 32);
        w.writeBool(s.taxExempt.ref());
        w.writeFieldEnd();
    }
    if(s.useEmailAutoFiling.isSet()) {
        w.writeFieldBegin("useEmailAutoFiling", ThriftFieldType::T_BOOL, 33);
        w.writeBool(s.useEmailAutoFiling.ref());
        w.writeFieldEnd();
    }
    if(s.reminderEmailConfig.isSet()) {
        w.writeFieldBegin("reminderEmailConfig", ThriftFieldType::T_I32, 34);
        w.writeI32(static_cast<qint32>(s.reminderEmailConfig.ref()));
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readUserAttributes(ThriftBinaryBufferReader& r, UserAttributes& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.defaultLocationName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.defaultLatitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.defaultLongitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.preactivation = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (UserAttributes.viewedPromotions)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.viewedPromotions = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.incomingEmailAddress = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (UserAttributes.recentMailedAddresses)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.recentMailedAddresses = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.comments = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.dateAgreedToTermsOfService = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.maxReferrals = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.referralCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.refererCode = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 15) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.sentEmailDate = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 16) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.sentEmailCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 17) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.dailyEmailLimit = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 18) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.emailOptOutDate = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 19) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.partnerEmailOptInDate = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 20) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.preferredLanguage = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 21) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.preferredCountry = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 22) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.clipFullPage = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 23) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.twitterUserName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 24) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.twitterId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 25) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.groupName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 26) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.recognitionLanguage = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 28) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.referralProof = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 29) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.educationalDiscount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 30) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.businessAddress = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 31) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.hideSponsorBilling = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 32) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.taxExempt = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 33) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.useEmailAutoFiling = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 34) {
            if(fieldType == ThriftFieldType::T_I32) {
                ReminderEmailConfig::type v;
                readEnumReminderEmailConfig(r, v);
                s.reminderEmailConfig = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeAccounting(ThriftBinaryBufferWriter& w, const Accounting& s) {
    w.writeStructBegin("Accounting");
    if(s.uploadLimit.isSet()) {
        w.writeFieldBegin("uploadLimit", ThriftFieldType::T_I64, 1);
        w.writeI64(s.uploadLimit.ref());
        w.writeFieldEnd();
    }
    if(s.uploadLimitEnd.isSet()) {
        w.writeFieldBegin("uploadLimitEnd", ThriftFieldType::T_I64, 2);
        w.writeI64(s.uploadLimitEnd.ref());
        w.writeFieldEnd();
    }
    if(s.uploadLimitNextMonth.isSet()) {
        w.writeFieldBegin("uploadLimitNextMonth", ThriftFieldType::T_I64, 3);
        w.writeI64(s.uploadLimitNextMonth.ref());
        w.writeFieldEnd();
    }
    if(s.premiumServiceStatus.isSet()) {
        w.writeFieldBegin("premiumServiceStatus", ThriftFieldType::T_I32, 4);
        w.writeI32(static_cast<qint32>(s.premiumServiceStatus.ref()));
        w.writeFieldEnd();
    }
    if(s.premiumOrderNumber.isSet()) {
        w.writeFieldBegin("premiumOrderNumber", ThriftFieldType::T_STRING, 5);
        w.writeString(s.premiumOrderNumber.ref());
        w.writeFieldEnd();
    }
    if(s.premiumCommerceService.isSet()) {
        w.writeFieldBegin("premiumCommerceService", ThriftFieldType::T_STRING, 6);
        w.writeString(s.premiumCommerceService.ref());
        w.writeFieldEnd();
    }
    if(s.premiumServiceStart.isSet()) {
        w.writeFieldBegin("premiumServiceStart", ThriftFieldType::T_I64, 7);
        w.writeI64(s.premiumServiceStart.ref());
        w.writeFieldEnd();
    }
    if(s.premiumServiceSKU.isSet()) {
        w.writeFieldBegin("premiumServiceSKU", ThriftFieldType::T_STRING, 8);
        w.writeString(s.premiumServiceSKU.ref());
        w.writeFieldEnd();
    }
    if(s.lastSuccessfulCharge.isSet()) {
        w.writeFieldBegin("lastSuccessfulCharge", ThriftFieldType::T_I64, 9);
        w.writeI64(s.lastSuccessfulCharge.ref());
        w.writeFieldEnd();
    }
    if(s.lastFailedCharge.isSet()) {
        w.writeFieldBegin("lastFailedCharge", ThriftFieldType::T_I64, 10);
        w.writeI64(s.lastFailedCharge.ref());
        w.writeFieldEnd();
    }
    if(s.lastFailedChargeReason.isSet()) {
        w.writeFieldBegin("lastFailedChargeReason", ThriftFieldType::T_STRING, 11);
        w.writeString(s.lastFailedChargeReason.ref());
        w.writeFieldEnd();
    }
    if(s.nextPaymentDue.isSet()) {
        w.writeFieldBegin("nextPaymentDue", ThriftFieldType::T_I64, 12);
        w.writeI64(s.nextPaymentDue.ref());
        w.writeFieldEnd();
    }
    if(s.premiumLockUntil.isSet()) {
        w.writeFieldBegin("premiumLockUntil", ThriftFieldType::T_I64, 13);
        w.writeI64(s.premiumLockUntil.ref());
        w.writeFieldEnd();
    }
    if(s.updated.isSet()) {
        w.writeFieldBegin("updated", ThriftFieldType::T_I64, 14);
        w.writeI64(s.updated.ref());
        w.writeFieldEnd();
    }
    if(s.premiumSubscriptionNumber.isSet()) {
        w.writeFieldBegin("premiumSubscriptionNumber", ThriftFieldType::T_STRING, 16);
        w.writeString(s.premiumSubscriptionNumber.ref());
        w.writeFieldEnd();
    }
    if(s.lastRequestedCharge.isSet()) {
        w.writeFieldBegin("lastRequestedCharge", ThriftFieldType::T_I64, 17);
        w.writeI64(s.lastRequestedCharge.ref());
        w.writeFieldEnd();
    }
    if(s.currency.isSet()) {
        w.writeFieldBegin("currency", ThriftFieldType::T_STRING, 18);
        w.writeString(s.currency.ref());
        w.writeFieldEnd();
    }
    if(s.unitPrice.isSet()) {
        w.writeFieldBegin("unitPrice", ThriftFieldType::T_I32, 19);
        w.writeI32(s.unitPrice.ref());
        w.writeFieldEnd();
    }
    if(s.businessId.isSet()) {
        w.writeFieldBegin("businessId", ThriftFieldType::T_I32, 20);
        w.writeI32(s.businessId.ref());
        w.writeFieldEnd();
    }
    if(s.businessName.isSet()) {
        w.writeFieldBegin("businessName", ThriftFieldType::T_STRING, 21);
        w.writeString(s.businessName.ref());
        w.writeFieldEnd();
    }
    if(s.businessRole.isSet()) {
        w.writeFieldBegin("businessRole", ThriftFieldType::T_I32, 22);
        w.writeI32(static_cast<qint32>(s.businessRole.ref()));
        w.writeFieldEnd();
    }
    if(s.unitDiscount.isSet()) {
        w.writeFieldBegin("unitDiscount", ThriftFieldType::T_I32, 23);
        w.writeI32(s.unitDiscount.ref());
        w.writeFieldEnd();
    }
    if(s.nextChargeDate.isSet()) {
        w.writeFieldBegin("nextChargeDate", ThriftFieldType::T_I64, 24);
        w.writeI64(s.nextChargeDate.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readAccounting(ThriftBinaryBufferReader& r, Accounting& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.uploadLimit = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.uploadLimitEnd = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.uploadLimitNextMonth = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_I32) {
                PremiumOrderStatus::type v;
                readEnumPremiumOrderStatus(r, v);
                s.premiumServiceStatus = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.premiumOrderNumber = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.premiumCommerceService = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.premiumServiceStart = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.premiumServiceSKU = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.lastSuccessfulCharge = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.lastFailedCharge = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.lastFailedChargeReason = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.nextPaymentDue = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.premiumLockUntil = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.updated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 16) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.premiumSubscriptionNumber = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 17) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.lastRequestedCharge = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 18) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.currency = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 19) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.unitPrice = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 20) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.businessId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 21) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.businessName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 22) {
            if(fieldType == ThriftFieldType::T_I32) {
                BusinessUserRole::type v;
                readEnumBusinessUserRole(r, v);
                s.businessRole = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 23) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.unitDiscount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 24) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.nextChargeDate = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeBusinessUserInfo(ThriftBinaryBufferWriter& w, const BusinessUserInfo& s) {
    w.writeStructBegin("BusinessUserInfo");
    if(s.businessId.isSet()) {
        w.writeFieldBegin("businessId", ThriftFieldType::T_I32, 1);
        w.writeI32(s.businessId.ref());
        w.writeFieldEnd();
    }
    if(s.businessName.isSet()) {
        w.writeFieldBegin("businessName", ThriftFieldType::T_STRING, 2);
        w.writeString(s.businessName.ref());
        w.writeFieldEnd();
    }
    if(s.role.isSet()) {
        w.writeFieldBegin("role", ThriftFieldType::T_I32, 3);
        w.writeI32(static_cast<qint32>(s.role.ref()));
        w.writeFieldEnd();
    }
    if(s.email.isSet()) {
        w.writeFieldBegin("email", ThriftFieldType::T_STRING, 4);
        w.writeString(s.email.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readBusinessUserInfo(ThriftBinaryBufferReader& r, BusinessUserInfo& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.businessId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.businessName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I32) {
                BusinessUserRole::type v;
                readEnumBusinessUserRole(r, v);
                s.role = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.email = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writePremiumInfo(ThriftBinaryBufferWriter& w, const PremiumInfo& s) {
    w.writeStructBegin("PremiumInfo");
    w.writeFieldBegin("currentTime", ThriftFieldType::T_I64, 1);
    w.writeI64(s.currentTime);
    w.writeFieldEnd();
    w.writeFieldBegin("premium", ThriftFieldType::T_BOOL, 2);
    w.writeBool(s.premium);
    w.writeFieldEnd();
    w.writeFieldBegin("premiumRecurring", ThriftFieldType::T_BOOL, 3);
    w.writeBool(s.premiumRecurring);
    w.writeFieldEnd();
    if(s.premiumExpirationDate.isSet()) {
        w.writeFieldBegin("premiumExpirationDate", ThriftFieldType::T_I64, 4);
        w.writeI64(s.premiumExpirationDate.ref());
        w.writeFieldEnd();
    }
    w.writeFieldBegin("premiumExtendable", ThriftFieldType::T_BOOL, 5);
    w.writeBool(s.premiumExtendable);
    w.writeFieldEnd();
    w.writeFieldBegin("premiumPending", ThriftFieldType::T_BOOL, 6);
    w.writeBool(s.premiumPending);
    w.writeFieldEnd();
    w.writeFieldBegin("premiumCancellationPending", ThriftFieldType::T_BOOL, 7);
    w.writeBool(s.premiumCancellationPending);
    w.writeFieldEnd();
    w.writeFieldBegin("canPurchaseUploadAllowance", ThriftFieldType::T_BOOL, 8);
    w.writeBool(s.canPurchaseUploadAllowance);
    w.writeFieldEnd();
    if(s.sponsoredGroupName.isSet()) {
        w.writeFieldBegin("sponsoredGroupName", ThriftFieldType::T_STRING, 9);
        w.writeString(s.sponsoredGroupName.ref());
        w.writeFieldEnd();
    }
    if(s.sponsoredGroupRole.isSet()) {
        w.writeFieldBegin("sponsoredGroupRole", ThriftFieldType::T_I32, 10);
        w.writeI32(static_cast<qint32>(s.sponsoredGroupRole.ref()));
        w.writeFieldEnd();
    }
    if(s.premiumUpgradable.isSet()) {
        w.writeFieldBegin("premiumUpgradable", ThriftFieldType::T_BOOL, 11);
        w.writeBool(s.premiumUpgradable.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readPremiumInfo(ThriftBinaryBufferReader& r, PremiumInfo& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool currentTime_isset = false;
    bool premium_isset = false;
    bool premiumRecurring_isset = false;
    bool premiumExtendable_isset = false;
    bool premiumPending_isset = false;
    bool premiumCancellationPending_isset = false;
    bool canPurchaseUploadAllowance_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I64) {
                currentTime_isset = true;
                qint64 v;
                r.readI64(v);
                s.currentTime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                premium_isset = true;
                bool v;
                r.readBool(v);
                s.premium = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                premiumRecurring_isset = true;
                bool v;
                r.readBool(v);
                s.premiumRecurring = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.premiumExpirationDate = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                premiumExtendable_isset = true;
                bool v;
                r.readBool(v);
                s.premiumExtendable = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                premiumPending_isset = true;
                bool v;
                r.readBool(v);
                s.premiumPending = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                premiumCancellationPending_isset = true;
                bool v;
                r.readBool(v);
                s.premiumCancellationPending = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                canPurchaseUploadAllowance_isset = true;
                bool v;
                r.readBool(v);
                s.canPurchaseUploadAllowance = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.sponsoredGroupName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_I32) {
                SponsoredGroupRole::type v;
                readEnumSponsoredGroupRole(r, v);
                s.sponsoredGroupRole = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.premiumUpgradable = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!currentTime_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PremiumInfo.currentTime has no value");
    if(!premium_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PremiumInfo.premium has no value");
    if(!premiumRecurring_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PremiumInfo.premiumRecurring has no value");
    if(!premiumExtendable_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PremiumInfo.premiumExtendable has no value");
    if(!premiumPending_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PremiumInfo.premiumPending has no value");
    if(!premiumCancellationPending_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PremiumInfo.premiumCancellationPending has no value");
    if(!canPurchaseUploadAllowance_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PremiumInfo.canPurchaseUploadAllowance has no value");
}

void writeUser(ThriftBinaryBufferWriter& w, const User& s) {
    w.writeStructBegin("User");
    if(s.id.isSet()) {
        w.writeFieldBegin("id", ThriftFieldType::T_I32, 1);
        w.writeI32(s.id.ref());
        w.writeFieldEnd();
    }
    if(s.username.isSet()) {
        w.writeFieldBegin("username", ThriftFieldType::T_STRING, 2);
        w.writeString(s.username.ref());
        w.writeFieldEnd();
    }
    if(s.email.isSet()) {
        w.writeFieldBegin("email", ThriftFieldType::T_STRING, 3);
        w.writeString(s.email.ref());
        w.writeFieldEnd();
    }
    if(s.name.isSet()) {
        w.writeFieldBegin("name", ThriftFieldType::T_STRING, 4);
        w.writeString(s.name.ref());
        w.writeFieldEnd();
    }
    if(s.timezone.isSet()) {
        w.writeFieldBegin("timezone", ThriftFieldType::T_STRING, 6);
        w.writeString(s.timezone.ref());
        w.writeFieldEnd();
    }
    if(s.privilege.isSet()) {
        w.writeFieldBegin("privilege", ThriftFieldType::T_I32, 7);
        w.writeI32(static_cast<qint32>(s.privilege.ref()));
        w.writeFieldEnd();
    }
    if(s.created.isSet()) {
        w.writeFieldBegin("created", ThriftFieldType::T_I64, 9);
        w.writeI64(s.created.ref());
        w.writeFieldEnd();
    }
    if(s.updated.isSet()) {
        w.writeFieldBegin("updated", ThriftFieldType::T_I64, 10);
        w.writeI64(s.updated.ref());
        w.writeFieldEnd();
    }
    if(s.deleted.isSet()) {
        w.writeFieldBegin("deleted", ThriftFieldType::T_I64, 11);
        w.writeI64(s.deleted.ref());
        w.writeFieldEnd();
    }
    if(s.active.isSet()) {
        w.writeFieldBegin("active", ThriftFieldType::T_BOOL, 13);
        w.writeBool(s.active.ref());
        w.writeFieldEnd();
    }
    if(s.shardId.isSet()) {
        w.writeFieldBegin("shardId", ThriftFieldType::T_STRING, 14);
        w.writeString(s.shardId.ref());
        w.writeFieldEnd();
    }
    if(s.attributes.isSet()) {
        w.writeFieldBegin("attributes", ThriftFieldType::T_STRUCT, 15);
        writeUserAttributes(w, s.attributes.ref());
        w.writeFieldEnd();
    }
    if(s.accounting.isSet()) {
        w.writeFieldBegin("accounting", ThriftFieldType::T_STRUCT, 16);
        writeAccounting(w, s.accounting.ref());
        w.writeFieldEnd();
    }
    if(s.premiumInfo.isSet()) {
        w.writeFieldBegin("premiumInfo", ThriftFieldType::T_STRUCT, 17);
        writePremiumInfo(w, s.premiumInfo.ref());
        w.writeFieldEnd();
    }
    if(s.businessUserInfo.isSet()) {
        w.writeFieldBegin("businessUserInfo", ThriftFieldType::T_STRUCT, 18);
        writeBusinessUserInfo(w, s.businessUserInfo.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readUser(ThriftBinaryBufferReader& r, User& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                UserID v;
                r.readI32(v);
                s.id = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.username = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.email = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.name = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.timezone = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_I32) {
                PrivilegeLevel::type v;
                readEnumPrivilegeLevel(r, v);
                s.privilege = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.created = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.updated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.deleted = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.active = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.shardId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 15) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                UserAttributes v;
                readUserAttributes(r, v);
                s.attributes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 16) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                Accounting v;
                readAccounting(r, v);
                s.accounting = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 17) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                PremiumInfo v;
                readPremiumInfo(r, v);
                s.premiumInfo = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 18) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                BusinessUserInfo v;
                readBusinessUserInfo(r, v);
                s.businessUserInfo = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeTag(ThriftBinaryBufferWriter& w, const Tag& s) {
    w.writeStructBegin("Tag");
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.name.isSet()) {
        w.writeFieldBegin("name", ThriftFieldType::T_STRING, 2);
        w.writeString(s.name.ref());
        w.writeFieldEnd();
    }
    if(s.parentGuid.isSet()) {
        w.writeFieldBegin("parentGuid", ThriftFieldType::T_STRING, 3);
        w.writeString(s.parentGuid.ref());
        w.writeFieldEnd();
    }
    if(s.updateSequenceNum.isSet()) {
        w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 4);
        w.writeI32(s.updateSequenceNum.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readTag(ThriftBinaryBufferReader& r, Tag& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.name = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.parentGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeLazyMap(ThriftBinaryBufferWriter& w, const LazyMap& s) {
    w.writeStructBegin("LazyMap");
    if(s.keysOnly.isSet()) {
        w.writeFieldBegin("keysOnly", ThriftFieldType::T_SET, 1);
        w.writeSetBegin(ThriftFieldType::T_STRING, s.keysOnly.ref().count());
        Q_FOREACH(const QString& elem,  s.keysOnly.ref()) {
            w.writeString(elem);
        }
        w.writeSetEnd();
        w.writeFieldEnd();
    }
    if(s.fullMap.isSet()) {
        w.writeFieldBegin("fullMap", ThriftFieldType::T_MAP, 2);
        w.writeMapBegin(ThriftFieldType::T_STRING, ThriftFieldType::T_STRING, s.fullMap.ref().keys().length());
        Q_FOREACH(const QString& elem, s.fullMap.ref().keys()) {
            w.writeString(elem);
            w.writeString(s.fullMap.ref().value(elem));
        }
        w.writeMapEnd();
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readLazyMap(ThriftBinaryBufferReader& r, LazyMap& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_SET) {
                QSet< QString > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readSetBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect set type (LazyMap.keysOnly)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.insert(elem);
                }
                r.readSetEnd();
                s.keysOnly = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_MAP) {
                QMap< QString, QString > v;
                quint32 size;
                ThriftFieldType::type keyType;
                ThriftFieldType::type elemType;
                r.readMapBegin(keyType, elemType, size);
                if(keyType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map key type (LazyMap.fullMap)");
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map value type (LazyMap.fullMap)");
                for(quint32 i = 0; i < size; i++) {
                    QString key;
                    r.readString(key);
                    QString value;
                    r.readString(value);
                    v[key] = value;
                }
                r.readMapEnd();
                s.fullMap = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeResourceAttributes(ThriftBinaryBufferWriter& w, const ResourceAttributes& s) {
    w.writeStructBegin("ResourceAttributes");
    if(s.sourceURL.isSet()) {
        w.writeFieldBegin("sourceURL", ThriftFieldType::T_STRING, 1);
        w.writeString(s.sourceURL.ref());
        w.writeFieldEnd();
    }
    if(s.timestamp.isSet()) {
        w.writeFieldBegin("timestamp", ThriftFieldType::T_I64, 2);
        w.writeI64(s.timestamp.ref());
        w.writeFieldEnd();
    }
    if(s.latitude.isSet()) {
        w.writeFieldBegin("latitude", ThriftFieldType::T_DOUBLE, 3);
        w.writeDouble(s.latitude.ref());
        w.writeFieldEnd();
    }
    if(s.longitude.isSet()) {
        w.writeFieldBegin("longitude", ThriftFieldType::T_DOUBLE, 4);
        w.writeDouble(s.longitude.ref());
        w.writeFieldEnd();
    }
    if(s.altitude.isSet()) {
        w.writeFieldBegin("altitude", ThriftFieldType::T_DOUBLE, 5);
        w.writeDouble(s.altitude.ref());
        w.writeFieldEnd();
    }
    if(s.cameraMake.isSet()) {
        w.writeFieldBegin("cameraMake", ThriftFieldType::T_STRING, 6);
        w.writeString(s.cameraMake.ref());
        w.writeFieldEnd();
    }
    if(s.cameraModel.isSet()) {
        w.writeFieldBegin("cameraModel", ThriftFieldType::T_STRING, 7);
        w.writeString(s.cameraModel.ref());
        w.writeFieldEnd();
    }
    if(s.clientWillIndex.isSet()) {
        w.writeFieldBegin("clientWillIndex", ThriftFieldType::T_BOOL, 8);
        w.writeBool(s.clientWillIndex.ref());
        w.writeFieldEnd();
    }
    if(s.recoType.isSet()) {
        w.writeFieldBegin("recoType", ThriftFieldType::T_STRING, 9);
        w.writeString(s.recoType.ref());
        w.writeFieldEnd();
    }
    if(s.fileName.isSet()) {
        w.writeFieldBegin("fileName", ThriftFieldType::T_STRING, 10);
        w.writeString(s.fileName.ref());
        w.writeFieldEnd();
    }
    if(s.attachment.isSet()) {
        w.writeFieldBegin("attachment", ThriftFieldType::T_BOOL, 11);
        w.writeBool(s.attachment.ref());
        w.writeFieldEnd();
    }
    if(s.applicationData.isSet()) {
        w.writeFieldBegin("applicationData", ThriftFieldType::T_STRUCT, 12);
        writeLazyMap(w, s.applicationData.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readResourceAttributes(ThriftBinaryBufferReader& r, ResourceAttributes& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.sourceURL = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.timestamp = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.latitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.longitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.altitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.cameraMake = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.cameraModel = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.clientWillIndex = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.recoType = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.fileName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.attachment = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                LazyMap v;
                readLazyMap(r, v);
                s.applicationData = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeResource(ThriftBinaryBufferWriter& w, const Resource& s) {
    w.writeStructBegin("Resource");
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.noteGuid.isSet()) {
        w.writeFieldBegin("noteGuid", ThriftFieldType::T_STRING, 2);
        w.writeString(s.noteGuid.ref());
        w.writeFieldEnd();
    }
    if(s.data.isSet()) {
        w.writeFieldBegin("data", ThriftFieldType::T_STRUCT, 3);
        writeData(w, s.data.ref());
        w.writeFieldEnd();
    }
    if(s.mime.isSet()) {
        w.writeFieldBegin("mime", ThriftFieldType::T_STRING, 4);
        w.writeString(s.mime.ref());
        w.writeFieldEnd();
    }
    if(s.width.isSet()) {
        w.writeFieldBegin("width", ThriftFieldType::T_I16, 5);
        w.writeI16(s.width.ref());
        w.writeFieldEnd();
    }
    if(s.height.isSet()) {
        w.writeFieldBegin("height", ThriftFieldType::T_I16, 6);
        w.writeI16(s.height.ref());
        w.writeFieldEnd();
    }
    if(s.duration.isSet()) {
        w.writeFieldBegin("duration", ThriftFieldType::T_I16, 7);
        w.writeI16(s.duration.ref());
        w.writeFieldEnd();
    }
    if(s.active.isSet()) {
        w.writeFieldBegin("active", ThriftFieldType::T_BOOL, 8);
        w.writeBool(s.active.ref());
        w.writeFieldEnd();
    }
    if(s.recognition.isSet()) {
        w.writeFieldBegin("recognition", ThriftFieldType::T_STRUCT, 9);
        writeData(w, s.recognition.ref());
        w.writeFieldEnd();
    }
    if(s.attributes.isSet()) {
        w.writeFieldBegin("attributes", ThriftFieldType::T_STRUCT, 11);
        writeResourceAttributes(w, s.attributes.ref());
        w.writeFieldEnd();
    }
    if(s.updateSequenceNum.isSet()) {
        w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 12);
        w.writeI32(s.updateSequenceNum.ref());
        w.writeFieldEnd();
    }
    if(s.alternateData.isSet()) {
        w.writeFieldBegin("alternateData", ThriftFieldType::T_STRUCT, 13);
        writeData(w, s.alternateData.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readResource(ThriftBinaryBufferReader& r, Resource& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.noteGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                Data v;
                readData(r, v);
                s.data = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.mime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_I16) {
                qint16 v;
                r.readI16(v);
                s.width = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_I16) {
                qint16 v;
                r.readI16(v);
                s.height = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_I16) {
                qint16 v;
                r.readI16(v);
                s.duration = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.active = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                Data v;
                readData(r, v);
                s.recognition = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                ResourceAttributes v;
                readResourceAttributes(r, v);
                s.attributes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                Data v;
                readData(r, v);
                s.alternateData = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNoteAttributes(ThriftBinaryBufferWriter& w, const NoteAttributes& s) {
    w.writeStructBegin("NoteAttributes");
    if(s.subjectDate.isSet()) {
        w.writeFieldBegin("subjectDate", ThriftFieldType::T_I64, 1);
        w.writeI64(s.subjectDate.ref());
        w.writeFieldEnd();
    }
    if(s.latitude.isSet()) {
        w.writeFieldBegin("latitude", ThriftFieldType::T_DOUBLE, 10);
        w.writeDouble(s.latitude.ref());
        w.writeFieldEnd();
    }
    if(s.longitude.isSet()) {
        w.writeFieldBegin("longitude", ThriftFieldType::T_DOUBLE, 11);
        w.writeDouble(s.longitude.ref());
        w.writeFieldEnd();
    }
    if(s.altitude.isSet()) {
        w.writeFieldBegin("altitude", ThriftFieldType::T_DOUBLE, 12);
        w.writeDouble(s.altitude.ref());
        w.writeFieldEnd();
    }
    if(s.author.isSet()) {
        w.writeFieldBegin("author", ThriftFieldType::T_STRING, 13);
        w.writeString(s.author.ref());
        w.writeFieldEnd();
    }
    if(s.source.isSet()) {
        w.writeFieldBegin("source", ThriftFieldType::T_STRING, 14);
        w.writeString(s.source.ref());
        w.writeFieldEnd();
    }
    if(s.sourceURL.isSet()) {
        w.writeFieldBegin("sourceURL", ThriftFieldType::T_STRING, 15);
        w.writeString(s.sourceURL.ref());
        w.writeFieldEnd();
    }
    if(s.sourceApplication.isSet()) {
        w.writeFieldBegin("sourceApplication", ThriftFieldType::T_STRING, 16);
        w.writeString(s.sourceApplication.ref());
        w.writeFieldEnd();
    }
    if(s.shareDate.isSet()) {
        w.writeFieldBegin("shareDate", ThriftFieldType::T_I64, 17);
        w.writeI64(s.shareDate.ref());
        w.writeFieldEnd();
    }
    if(s.reminderOrder.isSet()) {
        w.writeFieldBegin("reminderOrder", ThriftFieldType::T_I64, 18);
        w.writeI64(s.reminderOrder.ref());
        w.writeFieldEnd();
    }
    if(s.reminderDoneTime.isSet()) {
        w.writeFieldBegin("reminderDoneTime", ThriftFieldType::T_I64, 19);
        w.writeI64(s.reminderDoneTime.ref());
        w.writeFieldEnd();
    }
    if(s.reminderTime.isSet()) {
        w.writeFieldBegin("reminderTime", ThriftFieldType::T_I64, 20);
        w.writeI64(s.reminderTime.ref());
        w.writeFieldEnd();
    }
    if(s.placeName.isSet()) {
        w.writeFieldBegin("placeName", ThriftFieldType::T_STRING, 21);
        w.writeString(s.placeName.ref());
        w.writeFieldEnd();
    }
    if(s.contentClass.isSet()) {
        w.writeFieldBegin("contentClass", ThriftFieldType::T_STRING, 22);
        w.writeString(s.contentClass.ref());
        w.writeFieldEnd();
    }
    if(s.applicationData.isSet()) {
        w.writeFieldBegin("applicationData", ThriftFieldType::T_STRUCT, 23);
        writeLazyMap(w, s.applicationData.ref());
        w.writeFieldEnd();
    }
    if(s.lastEditedBy.isSet()) {
        w.writeFieldBegin("lastEditedBy", ThriftFieldType::T_STRING, 24);
        w.writeString(s.lastEditedBy.ref());
        w.writeFieldEnd();
    }
    if(s.classifications.isSet()) {
        w.writeFieldBegin("classifications", ThriftFieldType::T_MAP, 26);
        w.writeMapBegin(ThriftFieldType::T_STRING, ThriftFieldType::T_STRING, s.classifications.ref().keys().length());
        Q_FOREACH(const QString& elem, s.classifications.ref().keys()) {
            w.writeString(elem);
            w.writeString(s.classifications.ref().value(elem));
        }
        w.writeMapEnd();
        w.writeFieldEnd();
    }
    if(s.creatorId.isSet()) {
        w.writeFieldBegin("creatorId", ThriftFieldType::T_I32, 27);
        w.writeI32(s.creatorId.ref());
        w.writeFieldEnd();
    }
    if(s.lastEditorId.isSet()) {
        w.writeFieldBegin("lastEditorId", ThriftFieldType::T_I32, 28);
        w.writeI32(s.lastEditorId.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNoteAttributes(ThriftBinaryBufferReader& r, NoteAttributes& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.subjectDate = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.latitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.longitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_DOUBLE) {
                double v;
                r.readDouble(v);
                s.altitude = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.author = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.source = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 15) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.sourceURL = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 16) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.sourceApplication = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 17) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.shareDate = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 18) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.reminderOrder = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 19) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.reminderDoneTime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 20) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.reminderTime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 21) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.placeName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 22) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.contentClass = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 23) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                LazyMap v;
                readLazyMap(r, v);
                s.applicationData = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 24) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.lastEditedBy = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 26) {
            if(fieldType == ThriftFieldType::T_MAP) {
                QMap< QString, QString > v;
                quint32 size;
                ThriftFieldType::type keyType;
                ThriftFieldType::type elemType;
                r.readMapBegin(keyType, elemType, size);
                if(keyType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map key type (NoteAttributes.classifications)");
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect map value type (NoteAttributes.classifications)");
                for(quint32 i = 0; i < size; i++) {
                    QString key;
                    r.readString(key);
                    QString value;
                    r.readString(value);
                    v[key] = value;
                }
                r.readMapEnd();
                s.classifications = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 27) {
            if(fieldType == ThriftFieldType::T_I32) {
                UserID v;
                r.readI32(v);
                s.creatorId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 28) {
            if(fieldType == ThriftFieldType::T_I32) {
                UserID v;
                r.readI32(v);
                s.lastEditorId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNote(ThriftBinaryBufferWriter& w, const Note& s) {
    w.writeStructBegin("Note");
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.title.isSet()) {
        w.writeFieldBegin("title", ThriftFieldType::T_STRING, 2);
        w.writeString(s.title.ref());
        w.writeFieldEnd();
    }
    if(s.content.isSet()) {
        w.writeFieldBegin("content", ThriftFieldType::T_STRING, 3);
        w.writeString(s.content.ref());
        w.writeFieldEnd();
    }
    if(s.contentHash.isSet()) {
        w.writeFieldBegin("contentHash", ThriftFieldType::T_STRING, 4);
        w.writeBinary(s.contentHash.ref());
        w.writeFieldEnd();
    }
    if(s.contentLength.isSet()) {
        w.writeFieldBegin("contentLength", ThriftFieldType::T_I32, 5);
        w.writeI32(s.contentLength.ref());
        w.writeFieldEnd();
    }
    if(s.created.isSet()) {
        w.writeFieldBegin("created", ThriftFieldType::T_I64, 6);
        w.writeI64(s.created.ref());
        w.writeFieldEnd();
    }
    if(s.updated.isSet()) {
        w.writeFieldBegin("updated", ThriftFieldType::T_I64, 7);
        w.writeI64(s.updated.ref());
        w.writeFieldEnd();
    }
    if(s.deleted.isSet()) {
        w.writeFieldBegin("deleted", ThriftFieldType::T_I64, 8);
        w.writeI64(s.deleted.ref());
        w.writeFieldEnd();
    }
    if(s.active.isSet()) {
        w.writeFieldBegin("active", ThriftFieldType::T_BOOL, 9);
        w.writeBool(s.active.ref());
        w.writeFieldEnd();
    }
    if(s.updateSequenceNum.isSet()) {
        w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 10);
        w.writeI32(s.updateSequenceNum.ref());
        w.writeFieldEnd();
    }
    if(s.notebookGuid.isSet()) {
        w.writeFieldBegin("notebookGuid", ThriftFieldType::T_STRING, 11);
        w.writeString(s.notebookGuid.ref());
        w.writeFieldEnd();
    }
    if(s.tagGuids.isSet()) {
        w.writeFieldBegin("tagGuids", ThriftFieldType::T_LIST, 12);
        w.writeListBegin(ThriftFieldType::T_STRING, s.tagGuids.ref().length());
        Q_FOREACH(const Guid& elem, s.tagGuids.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.resources.isSet()) {
        w.writeFieldBegin("resources", ThriftFieldType::T_LIST, 13);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.resources.ref().length());
        Q_FOREACH(const Resource& elem, s.resources.ref()) {
            writeResource(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.attributes.isSet()) {
        w.writeFieldBegin("attributes", ThriftFieldType::T_STRUCT, 14);
        writeNoteAttributes(w, s.attributes.ref());
        w.writeFieldEnd();
    }
    if(s.tagNames.isSet()) {
        w.writeFieldBegin("tagNames", ThriftFieldType::T_LIST, 15);
        w.writeListBegin(ThriftFieldType::T_STRING, s.tagNames.ref().length());
        Q_FOREACH(const QString& elem, s.tagNames.ref()) {
            w.writeString(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNote(ThriftBinaryBufferReader& r, Note& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.title = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.content = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QByteArray v;
                r.readBinary(v);
                s.contentHash = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.contentLength = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.created = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.updated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.deleted = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.active = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.notebookGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Guid > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (Note.tagGuids)");
                for(quint32 i = 0; i < size; i++) {
                    Guid elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.tagGuids = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< Resource > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (Note.resources)");
                for(quint32 i = 0; i < size; i++) {
                    Resource elem;
                    readResource(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.resources = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                NoteAttributes v;
                readNoteAttributes(r, v);
                s.attributes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 15) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QStringList v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRING) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (Note.tagNames)");
                for(quint32 i = 0; i < size; i++) {
                    QString elem;
                    r.readString(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.tagNames = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writePublishing(ThriftBinaryBufferWriter& w, const Publishing& s) {
    w.writeStructBegin("Publishing");
    if(s.uri.isSet()) {
        w.writeFieldBegin("uri", ThriftFieldType::T_STRING, 1);
        w.writeString(s.uri.ref());
        w.writeFieldEnd();
    }
    if(s.order.isSet()) {
        w.writeFieldBegin("order", ThriftFieldType::T_I32, 2);
        w.writeI32(static_cast<qint32>(s.order.ref()));
        w.writeFieldEnd();
    }
    if(s.ascending.isSet()) {
        w.writeFieldBegin("ascending", ThriftFieldType::T_BOOL, 3);
        w.writeBool(s.ascending.ref());
        w.writeFieldEnd();
    }
    if(s.publicDescription.isSet()) {
        w.writeFieldBegin("publicDescription", ThriftFieldType::T_STRING, 4);
        w.writeString(s.publicDescription.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readPublishing(ThriftBinaryBufferReader& r, Publishing& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.uri = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                NoteSortOrder::type v;
                readEnumNoteSortOrder(r, v);
                s.order = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.ascending = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.publicDescription = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeBusinessNotebook(ThriftBinaryBufferWriter& w, const BusinessNotebook& s) {
    w.writeStructBegin("BusinessNotebook");
    if(s.notebookDescription.isSet()) {
        w.writeFieldBegin("notebookDescription", ThriftFieldType::T_STRING, 1);
        w.writeString(s.notebookDescription.ref());
        w.writeFieldEnd();
    }
    if(s.privilege.isSet()) {
        w.writeFieldBegin("privilege", ThriftFieldType::T_I32, 2);
        w.writeI32(static_cast<qint32>(s.privilege.ref()));
        w.writeFieldEnd();
    }
    if(s.recommended.isSet()) {
        w.writeFieldBegin("recommended", ThriftFieldType::T_BOOL, 3);
        w.writeBool(s.recommended.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readBusinessNotebook(ThriftBinaryBufferReader& r, BusinessNotebook& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.notebookDescription = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                SharedNotebookPrivilegeLevel::type v;
                readEnumSharedNotebookPrivilegeLevel(r, v);
                s.privilege = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.recommended = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeSavedSearchScope(ThriftBinaryBufferWriter& w, const SavedSearchScope& s) {
    w.writeStructBegin("SavedSearchScope");
    if(s.includeAccount.isSet()) {
        w.writeFieldBegin("includeAccount", ThriftFieldType::T_BOOL, 1);
        w.writeBool(s.includeAccount.ref());
        w.writeFieldEnd();
    }
    if(s.includePersonalLinkedNotebooks.isSet()) {
        w.writeFieldBegin("includePersonalLinkedNotebooks", ThriftFieldType::T_BOOL, 2);
        w.writeBool(s.includePersonalLinkedNotebooks.ref());
        w.writeFieldEnd();
    }
    if(s.includeBusinessLinkedNotebooks.isSet()) {
        w.writeFieldBegin("includeBusinessLinkedNotebooks", ThriftFieldType::T_BOOL, 3);
        w.writeBool(s.includeBusinessLinkedNotebooks.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readSavedSearchScope(ThriftBinaryBufferReader& r, SavedSearchScope& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeAccount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includePersonalLinkedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.includeBusinessLinkedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeSavedSearch(ThriftBinaryBufferWriter& w, const SavedSearch& s) {
    w.writeStructBegin("SavedSearch");
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.name.isSet()) {
        w.writeFieldBegin("name", ThriftFieldType::T_STRING, 2);
        w.writeString(s.name.ref());
        w.writeFieldEnd();
    }
    if(s.query.isSet()) {
        w.writeFieldBegin("query", ThriftFieldType::T_STRING, 3);
        w.writeString(s.query.ref());
        w.writeFieldEnd();
    }
    if(s.format.isSet()) {
        w.writeFieldBegin("format", ThriftFieldType::T_I32, 4);
        w.writeI32(static_cast<qint32>(s.format.ref()));
        w.writeFieldEnd();
    }
    if(s.updateSequenceNum.isSet()) {
        w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 5);
        w.writeI32(s.updateSequenceNum.ref());
        w.writeFieldEnd();
    }
    if(s.scope.isSet()) {
        w.writeFieldBegin("scope", ThriftFieldType::T_STRUCT, 6);
        writeSavedSearchScope(w, s.scope.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readSavedSearch(ThriftBinaryBufferReader& r, SavedSearch& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.name = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.query = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_I32) {
                QueryFormat::type v;
                readEnumQueryFormat(r, v);
                s.format = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                SavedSearchScope v;
                readSavedSearchScope(r, v);
                s.scope = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeSharedNotebookRecipientSettings(ThriftBinaryBufferWriter& w, const SharedNotebookRecipientSettings& s) {
    w.writeStructBegin("SharedNotebookRecipientSettings");
    if(s.reminderNotifyEmail.isSet()) {
        w.writeFieldBegin("reminderNotifyEmail", ThriftFieldType::T_BOOL, 1);
        w.writeBool(s.reminderNotifyEmail.ref());
        w.writeFieldEnd();
    }
    if(s.reminderNotifyInApp.isSet()) {
        w.writeFieldBegin("reminderNotifyInApp", ThriftFieldType::T_BOOL, 2);
        w.writeBool(s.reminderNotifyInApp.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readSharedNotebookRecipientSettings(ThriftBinaryBufferReader& r, SharedNotebookRecipientSettings& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.reminderNotifyEmail = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.reminderNotifyInApp = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeSharedNotebook(ThriftBinaryBufferWriter& w, const SharedNotebook& s) {
    w.writeStructBegin("SharedNotebook");
    if(s.id.isSet()) {
        w.writeFieldBegin("id", ThriftFieldType::T_I64, 1);
        w.writeI64(s.id.ref());
        w.writeFieldEnd();
    }
    if(s.userId.isSet()) {
        w.writeFieldBegin("userId", ThriftFieldType::T_I32, 2);
        w.writeI32(s.userId.ref());
        w.writeFieldEnd();
    }
    if(s.notebookGuid.isSet()) {
        w.writeFieldBegin("notebookGuid", ThriftFieldType::T_STRING, 3);
        w.writeString(s.notebookGuid.ref());
        w.writeFieldEnd();
    }
    if(s.email.isSet()) {
        w.writeFieldBegin("email", ThriftFieldType::T_STRING, 4);
        w.writeString(s.email.ref());
        w.writeFieldEnd();
    }
    if(s.notebookModifiable.isSet()) {
        w.writeFieldBegin("notebookModifiable", ThriftFieldType::T_BOOL, 5);
        w.writeBool(s.notebookModifiable.ref());
        w.writeFieldEnd();
    }
    if(s.requireLogin.isSet()) {
        w.writeFieldBegin("requireLogin", ThriftFieldType::T_BOOL, 6);
        w.writeBool(s.requireLogin.ref());
        w.writeFieldEnd();
    }
    if(s.serviceCreated.isSet()) {
        w.writeFieldBegin("serviceCreated", ThriftFieldType::T_I64, 7);
        w.writeI64(s.serviceCreated.ref());
        w.writeFieldEnd();
    }
    if(s.serviceUpdated.isSet()) {
        w.writeFieldBegin("serviceUpdated", ThriftFieldType::T_I64, 10);
        w.writeI64(s.serviceUpdated.ref());
        w.writeFieldEnd();
    }
    if(s.shareKey.isSet()) {
        w.writeFieldBegin("shareKey", ThriftFieldType::T_STRING, 8);
        w.writeString(s.shareKey.ref());
        w.writeFieldEnd();
    }
    if(s.username.isSet()) {
        w.writeFieldBegin("username", ThriftFieldType::T_STRING, 9);
        w.writeString(s.username.ref());
        w.writeFieldEnd();
    }
    if(s.privilege.isSet()) {
        w.writeFieldBegin("privilege", ThriftFieldType::T_I32, 11);
        w.writeI32(static_cast<qint32>(s.privilege.ref()));
        w.writeFieldEnd();
    }
    if(s.allowPreview.isSet()) {
        w.writeFieldBegin("allowPreview", ThriftFieldType::T_BOOL, 12);
        w.writeBool(s.allowPreview.ref());
        w.writeFieldEnd();
    }
    if(s.recipientSettings.isSet()) {
        w.writeFieldBegin("recipientSettings", ThriftFieldType::T_STRUCT, 13);
        writeSharedNotebookRecipientSettings(w, s.recipientSettings.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readSharedNotebook(ThriftBinaryBufferReader& r, SharedNotebook& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.id = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.userId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.notebookGuid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.email = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.notebookModifiable = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.requireLogin = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.serviceCreated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.serviceUpdated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.shareKey = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.username = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_I32) {
                SharedNotebookPrivilegeLevel::type v;
                readEnumSharedNotebookPrivilegeLevel(r, v);
                s.privilege = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.allowPreview = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                SharedNotebookRecipientSettings v;
                readSharedNotebookRecipientSettings(r, v);
                s.recipientSettings = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNotebookRestrictions(ThriftBinaryBufferWriter& w, const NotebookRestrictions& s) {
    w.writeStructBegin("NotebookRestrictions");
    if(s.noReadNotes.isSet()) {
        w.writeFieldBegin("noReadNotes", ThriftFieldType::T_BOOL, 1);
        w.writeBool(s.noReadNotes.ref());
        w.writeFieldEnd();
    }
    if(s.noCreateNotes.isSet()) {
        w.writeFieldBegin("noCreateNotes", ThriftFieldType::T_BOOL, 2);
        w.writeBool(s.noCreateNotes.ref());
        w.writeFieldEnd();
    }
    if(s.noUpdateNotes.isSet()) {
        w.writeFieldBegin("noUpdateNotes", ThriftFieldType::T_BOOL, 3);
        w.writeBool(s.noUpdateNotes.ref());
        w.writeFieldEnd();
    }
    if(s.noExpungeNotes.isSet()) {
        w.writeFieldBegin("noExpungeNotes", ThriftFieldType::T_BOOL, 4);
        w.writeBool(s.noExpungeNotes.ref());
        w.writeFieldEnd();
    }
    if(s.noShareNotes.isSet()) {
        w.writeFieldBegin("noShareNotes", ThriftFieldType::T_BOOL, 5);
        w.writeBool(s.noShareNotes.ref());
        w.writeFieldEnd();
    }
    if(s.noEmailNotes.isSet()) {
        w.writeFieldBegin("noEmailNotes", ThriftFieldType::T_BOOL, 6);
        w.writeBool(s.noEmailNotes.ref());
        w.writeFieldEnd();
    }
    if(s.noSendMessageToRecipients.isSet()) {
        w.writeFieldBegin("noSendMessageToRecipients", ThriftFieldType::T_BOOL, 7);
        w.writeBool(s.noSendMessageToRecipients.ref());
        w.writeFieldEnd();
    }
    if(s.noUpdateNotebook.isSet()) {
        w.writeFieldBegin("noUpdateNotebook", ThriftFieldType::T_BOOL, 8);
        w.writeBool(s.noUpdateNotebook.ref());
        w.writeFieldEnd();
    }
    if(s.noExpungeNotebook.isSet()) {
        w.writeFieldBegin("noExpungeNotebook", ThriftFieldType::T_BOOL, 9);
        w.writeBool(s.noExpungeNotebook.ref());
        w.writeFieldEnd();
    }
    if(s.noSetDefaultNotebook.isSet()) {
        w.writeFieldBegin("noSetDefaultNotebook", ThriftFieldType::T_BOOL, 10);
        w.writeBool(s.noSetDefaultNotebook.ref());
        w.writeFieldEnd();
    }
    if(s.noSetNotebookStack.isSet()) {
        w.writeFieldBegin("noSetNotebookStack", ThriftFieldType::T_BOOL, 11);
        w.writeBool(s.noSetNotebookStack.ref());
        w.writeFieldEnd();
    }
    if(s.noPublishToPublic.isSet()) {
        w.writeFieldBegin("noPublishToPublic", ThriftFieldType::T_BOOL, 12);
        w.writeBool(s.noPublishToPublic.ref());
        w.writeFieldEnd();
    }
    if(s.noPublishToBusinessLibrary.isSet()) {
        w.writeFieldBegin("noPublishToBusinessLibrary", ThriftFieldType::T_BOOL, 13);
        w.writeBool(s.noPublishToBusinessLibrary.ref());
        w.writeFieldEnd();
    }
    if(s.noCreateTags.isSet()) {
        w.writeFieldBegin("noCreateTags", ThriftFieldType::T_BOOL, 14);
        w.writeBool(s.noCreateTags.ref());
        w.writeFieldEnd();
    }
    if(s.noUpdateTags.isSet()) {
        w.writeFieldBegin("noUpdateTags", ThriftFieldType::T_BOOL, 15);
        w.writeBool(s.noUpdateTags.ref());
        w.writeFieldEnd();
    }
    if(s.noExpungeTags.isSet()) {
        w.writeFieldBegin("noExpungeTags", ThriftFieldType::T_BOOL, 16);
        w.writeBool(s.noExpungeTags.ref());
        w.writeFieldEnd();
    }
    if(s.noSetParentTag.isSet()) {
        w.writeFieldBegin("noSetParentTag", ThriftFieldType::T_BOOL, 17);
        w.writeBool(s.noSetParentTag.ref());
        w.writeFieldEnd();
    }
    if(s.noCreateSharedNotebooks.isSet()) {
        w.writeFieldBegin("noCreateSharedNotebooks", ThriftFieldType::T_BOOL, 18);
        w.writeBool(s.noCreateSharedNotebooks.ref());
        w.writeFieldEnd();
    }
    if(s.updateWhichSharedNotebookRestrictions.isSet()) {
        w.writeFieldBegin("updateWhichSharedNotebookRestrictions", ThriftFieldType::T_I32, 19);
        w.writeI32(static_cast<qint32>(s.updateWhichSharedNotebookRestrictions.ref()));
        w.writeFieldEnd();
    }
    if(s.expungeWhichSharedNotebookRestrictions.isSet()) {
        w.writeFieldBegin("expungeWhichSharedNotebookRestrictions", ThriftFieldType::T_I32, 20);
        w.writeI32(static_cast<qint32>(s.expungeWhichSharedNotebookRestrictions.ref()));
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNotebookRestrictions(ThriftBinaryBufferReader& r, NotebookRestrictions& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noReadNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noCreateNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noUpdateNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noExpungeNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noShareNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noEmailNotes = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noSendMessageToRecipients = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noUpdateNotebook = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noExpungeNotebook = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noSetDefaultNotebook = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noSetNotebookStack = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noPublishToPublic = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noPublishToBusinessLibrary = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noCreateTags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 15) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noUpdateTags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 16) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noExpungeTags = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 17) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noSetParentTag = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 18) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.noCreateSharedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 19) {
            if(fieldType == ThriftFieldType::T_I32) {
                SharedNotebookInstanceRestrictions::type v;
                readEnumSharedNotebookInstanceRestrictions(r, v);
                s.updateWhichSharedNotebookRestrictions = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 20) {
            if(fieldType == ThriftFieldType::T_I32) {
                SharedNotebookInstanceRestrictions::type v;
                readEnumSharedNotebookInstanceRestrictions(r, v);
                s.expungeWhichSharedNotebookRestrictions = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNotebook(ThriftBinaryBufferWriter& w, const Notebook& s) {
    w.writeStructBegin("Notebook");
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.name.isSet()) {
        w.writeFieldBegin("name", ThriftFieldType::T_STRING, 2);
        w.writeString(s.name.ref());
        w.writeFieldEnd();
    }
    if(s.updateSequenceNum.isSet()) {
        w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 5);
        w.writeI32(s.updateSequenceNum.ref());
        w.writeFieldEnd();
    }
    if(s.defaultNotebook.isSet()) {
        w.writeFieldBegin("defaultNotebook", ThriftFieldType::T_BOOL, 6);
        w.writeBool(s.defaultNotebook.ref());
        w.writeFieldEnd();
    }
    if(s.serviceCreated.isSet()) {
        w.writeFieldBegin("serviceCreated", ThriftFieldType::T_I64, 7);
        w.writeI64(s.serviceCreated.ref());
        w.writeFieldEnd();
    }
    if(s.serviceUpdated.isSet()) {
        w.writeFieldBegin("serviceUpdated", ThriftFieldType::T_I64, 8);
        w.writeI64(s.serviceUpdated.ref());
        w.writeFieldEnd();
    }
    if(s.publishing.isSet()) {
        w.writeFieldBegin("publishing", ThriftFieldType::T_STRUCT, 10);
        writePublishing(w, s.publishing.ref());
        w.writeFieldEnd();
    }
    if(s.published.isSet()) {
        w.writeFieldBegin("published", ThriftFieldType::T_BOOL, 11);
        w.writeBool(s.published.ref());
        w.writeFieldEnd();
    }
    if(s.stack.isSet()) {
        w.writeFieldBegin("stack", ThriftFieldType::T_STRING, 12);
        w.writeString(s.stack.ref());
        w.writeFieldEnd();
    }
    if(s.sharedNotebookIds.isSet()) {
        w.writeFieldBegin("sharedNotebookIds", ThriftFieldType::T_LIST, 13);
        w.writeListBegin(ThriftFieldType::T_I64, s.sharedNotebookIds.ref().length());
        Q_FOREACH(const qint64& elem, s.sharedNotebookIds.ref()) {
            w.writeI64(elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.sharedNotebooks.isSet()) {
        w.writeFieldBegin("sharedNotebooks", ThriftFieldType::T_LIST, 14);
        w.writeListBegin(ThriftFieldType::T_STRUCT, s.sharedNotebooks.ref().length());
        Q_FOREACH(const SharedNotebook& elem, s.sharedNotebooks.ref()) {
            writeSharedNotebook(w, elem);
        }
        w.writeListEnd();
        w.writeFieldEnd();
    }
    if(s.businessNotebook.isSet()) {
        w.writeFieldBegin("businessNotebook", ThriftFieldType::T_STRUCT, 15);
        writeBusinessNotebook(w, s.businessNotebook.ref());
        w.writeFieldEnd();
    }
    if(s.contact.isSet()) {
        w.writeFieldBegin("contact", ThriftFieldType::T_STRUCT, 16);
        writeUser(w, s.contact.ref());
        w.writeFieldEnd();
    }
    if(s.restrictions.isSet()) {
        w.writeFieldBegin("restrictions", ThriftFieldType::T_STRUCT, 17);
        writeNotebookRestrictions(w, s.restrictions.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNotebook(ThriftBinaryBufferReader& r, Notebook& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.name = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.defaultNotebook = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.serviceCreated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_I64) {
                qint64 v;
                r.readI64(v);
                s.serviceUpdated = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                Publishing v;
                readPublishing(r, v);
                s.publishing = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.published = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.stack = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< qint64 > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_I64) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (Notebook.sharedNotebookIds)");
                for(quint32 i = 0; i < size; i++) {
                    qint64 elem;
                    r.readI64(elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.sharedNotebookIds = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 14) {
            if(fieldType == ThriftFieldType::T_LIST) {
                QList< SharedNotebook > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (Notebook.sharedNotebooks)");
                for(quint32 i = 0; i < size; i++) {
                    SharedNotebook elem;
                    readSharedNotebook(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.sharedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 15) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                BusinessNotebook v;
                readBusinessNotebook(r, v);
                s.businessNotebook = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 16) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                User v;
                readUser(r, v);
                s.contact = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 17) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                NotebookRestrictions v;
                readNotebookRestrictions(r, v);
                s.restrictions = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeLinkedNotebook(ThriftBinaryBufferWriter& w, const LinkedNotebook& s) {
    w.writeStructBegin("LinkedNotebook");
    if(s.shareName.isSet()) {
        w.writeFieldBegin("shareName", ThriftFieldType::T_STRING, 2);
        w.writeString(s.shareName.ref());
        w.writeFieldEnd();
    }
    if(s.username.isSet()) {
        w.writeFieldBegin("username", ThriftFieldType::T_STRING, 3);
        w.writeString(s.username.ref());
        w.writeFieldEnd();
    }
    if(s.shardId.isSet()) {
        w.writeFieldBegin("shardId", ThriftFieldType::T_STRING, 4);
        w.writeString(s.shardId.ref());
        w.writeFieldEnd();
    }
    if(s.shareKey.isSet()) {
        w.writeFieldBegin("shareKey", ThriftFieldType::T_STRING, 5);
        w.writeString(s.shareKey.ref());
        w.writeFieldEnd();
    }
    if(s.uri.isSet()) {
        w.writeFieldBegin("uri", ThriftFieldType::T_STRING, 6);
        w.writeString(s.uri.ref());
        w.writeFieldEnd();
    }
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 7);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.updateSequenceNum.isSet()) {
        w.writeFieldBegin("updateSequenceNum", ThriftFieldType::T_I32, 8);
        w.writeI32(s.updateSequenceNum.ref());
        w.writeFieldEnd();
    }
    if(s.noteStoreUrl.isSet()) {
        w.writeFieldBegin("noteStoreUrl", ThriftFieldType::T_STRING, 9);
        w.writeString(s.noteStoreUrl.ref());
        w.writeFieldEnd();
    }
    if(s.webApiUrlPrefix.isSet()) {
        w.writeFieldBegin("webApiUrlPrefix", ThriftFieldType::T_STRING, 10);
        w.writeString(s.webApiUrlPrefix.ref());
        w.writeFieldEnd();
    }
    if(s.stack.isSet()) {
        w.writeFieldBegin("stack", ThriftFieldType::T_STRING, 11);
        w.writeString(s.stack.ref());
        w.writeFieldEnd();
    }
    if(s.businessId.isSet()) {
        w.writeFieldBegin("businessId", ThriftFieldType::T_I32, 12);
        w.writeI32(s.businessId.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readLinkedNotebook(ThriftBinaryBufferReader& r, LinkedNotebook& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.shareName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.username = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.shardId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.shareKey = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.uri = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.updateSequenceNum = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.noteStoreUrl = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.webApiUrlPrefix = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.stack = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.businessId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writeNotebookDescriptor(ThriftBinaryBufferWriter& w, const NotebookDescriptor& s) {
    w.writeStructBegin("NotebookDescriptor");
    if(s.guid.isSet()) {
        w.writeFieldBegin("guid", ThriftFieldType::T_STRING, 1);
        w.writeString(s.guid.ref());
        w.writeFieldEnd();
    }
    if(s.notebookDisplayName.isSet()) {
        w.writeFieldBegin("notebookDisplayName", ThriftFieldType::T_STRING, 2);
        w.writeString(s.notebookDisplayName.ref());
        w.writeFieldEnd();
    }
    if(s.contactName.isSet()) {
        w.writeFieldBegin("contactName", ThriftFieldType::T_STRING, 3);
        w.writeString(s.contactName.ref());
        w.writeFieldEnd();
    }
    if(s.hasSharedNotebook.isSet()) {
        w.writeFieldBegin("hasSharedNotebook", ThriftFieldType::T_BOOL, 4);
        w.writeBool(s.hasSharedNotebook.ref());
        w.writeFieldEnd();
    }
    if(s.joinedUserCount.isSet()) {
        w.writeFieldBegin("joinedUserCount", ThriftFieldType::T_I32, 5);
        w.writeI32(s.joinedUserCount.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readNotebookDescriptor(ThriftBinaryBufferReader& r, NotebookDescriptor& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                Guid v;
                r.readString(v);
                s.guid = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.notebookDisplayName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.contactName = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.hasSharedNotebook = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_I32) {
                qint32 v;
                r.readI32(v);
                s.joinedUserCount = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
}

void writePublicUserInfo(ThriftBinaryBufferWriter& w, const PublicUserInfo& s) {
    w.writeStructBegin("PublicUserInfo");
    w.writeFieldBegin("userId", ThriftFieldType::T_I32, 1);
    w.writeI32(s.userId);
    w.writeFieldEnd();
    w.writeFieldBegin("shardId", ThriftFieldType::T_STRING, 2);
    w.writeString(s.shardId);
    w.writeFieldEnd();
    if(s.privilege.isSet()) {
        w.writeFieldBegin("privilege", ThriftFieldType::T_I32, 3);
        w.writeI32(static_cast<qint32>(s.privilege.ref()));
        w.writeFieldEnd();
    }
    if(s.username.isSet()) {
        w.writeFieldBegin("username", ThriftFieldType::T_STRING, 4);
        w.writeString(s.username.ref());
        w.writeFieldEnd();
    }
    if(s.noteStoreUrl.isSet()) {
        w.writeFieldBegin("noteStoreUrl", ThriftFieldType::T_STRING, 5);
        w.writeString(s.noteStoreUrl.ref());
        w.writeFieldEnd();
    }
    if(s.webApiUrlPrefix.isSet()) {
        w.writeFieldBegin("webApiUrlPrefix", ThriftFieldType::T_STRING, 6);
        w.writeString(s.webApiUrlPrefix.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readPublicUserInfo(ThriftBinaryBufferReader& r, PublicUserInfo& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool userId_isset = false;
    bool shardId_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I32) {
                userId_isset = true;
                UserID v;
                r.readI32(v);
                s.userId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                shardId_isset = true;
                QString v;
                r.readString(v);
                s.shardId = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I32) {
                PrivilegeLevel::type v;
                readEnumPrivilegeLevel(r, v);
                s.privilege = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.username = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.noteStoreUrl = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.webApiUrlPrefix = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!userId_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PublicUserInfo.userId has no value");
    if(!shardId_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "PublicUserInfo.shardId has no value");
}

void writeAuthenticationResult(ThriftBinaryBufferWriter& w, const AuthenticationResult& s) {
    w.writeStructBegin("AuthenticationResult");
    w.writeFieldBegin("currentTime", ThriftFieldType::T_I64, 1);
    w.writeI64(s.currentTime);
    w.writeFieldEnd();
    w.writeFieldBegin("authenticationToken", ThriftFieldType::T_STRING, 2);
    w.writeString(s.authenticationToken);
    w.writeFieldEnd();
    w.writeFieldBegin("expiration", ThriftFieldType::T_I64, 3);
    w.writeI64(s.expiration);
    w.writeFieldEnd();
    if(s.user.isSet()) {
        w.writeFieldBegin("user", ThriftFieldType::T_STRUCT, 4);
        writeUser(w, s.user.ref());
        w.writeFieldEnd();
    }
    if(s.publicUserInfo.isSet()) {
        w.writeFieldBegin("publicUserInfo", ThriftFieldType::T_STRUCT, 5);
        writePublicUserInfo(w, s.publicUserInfo.ref());
        w.writeFieldEnd();
    }
    if(s.noteStoreUrl.isSet()) {
        w.writeFieldBegin("noteStoreUrl", ThriftFieldType::T_STRING, 6);
        w.writeString(s.noteStoreUrl.ref());
        w.writeFieldEnd();
    }
    if(s.webApiUrlPrefix.isSet()) {
        w.writeFieldBegin("webApiUrlPrefix", ThriftFieldType::T_STRING, 7);
        w.writeString(s.webApiUrlPrefix.ref());
        w.writeFieldEnd();
    }
    if(s.secondFactorRequired.isSet()) {
        w.writeFieldBegin("secondFactorRequired", ThriftFieldType::T_BOOL, 8);
        w.writeBool(s.secondFactorRequired.ref());
        w.writeFieldEnd();
    }
    if(s.secondFactorDeliveryHint.isSet()) {
        w.writeFieldBegin("secondFactorDeliveryHint", ThriftFieldType::T_STRING, 9);
        w.writeString(s.secondFactorDeliveryHint.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readAuthenticationResult(ThriftBinaryBufferReader& r, AuthenticationResult& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool currentTime_isset = false;
    bool authenticationToken_isset = false;
    bool expiration_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_I64) {
                currentTime_isset = true;
                qint64 v;
                r.readI64(v);
                s.currentTime = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                authenticationToken_isset = true;
                QString v;
                r.readString(v);
                s.authenticationToken = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_I64) {
                expiration_isset = true;
                qint64 v;
                r.readI64(v);
                s.expiration = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                User v;
                readUser(r, v);
                s.user = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                PublicUserInfo v;
                readPublicUserInfo(r, v);
                s.publicUserInfo = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.noteStoreUrl = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.webApiUrlPrefix = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.secondFactorRequired = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_STRING) {
                QString v;
                r.readString(v);
                s.secondFactorDeliveryHint = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!currentTime_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "AuthenticationResult.currentTime has no value");
    if(!authenticationToken_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "AuthenticationResult.authenticationToken has no value");
    if(!expiration_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "AuthenticationResult.expiration has no value");
}

void writeBootstrapSettings(ThriftBinaryBufferWriter& w, const BootstrapSettings& s) {
    w.writeStructBegin("BootstrapSettings");
    w.writeFieldBegin("serviceHost", ThriftFieldType::T_STRING, 1);
    w.writeString(s.serviceHost);
    w.writeFieldEnd();
    w.writeFieldBegin("marketingUrl", ThriftFieldType::T_STRING, 2);
    w.writeString(s.marketingUrl);
    w.writeFieldEnd();
    w.writeFieldBegin("supportUrl", ThriftFieldType::T_STRING, 3);
    w.writeString(s.supportUrl);
    w.writeFieldEnd();
    w.writeFieldBegin("accountEmailDomain", ThriftFieldType::T_STRING, 4);
    w.writeString(s.accountEmailDomain);
    w.writeFieldEnd();
    if(s.enableFacebookSharing.isSet()) {
        w.writeFieldBegin("enableFacebookSharing", ThriftFieldType::T_BOOL, 5);
        w.writeBool(s.enableFacebookSharing.ref());
        w.writeFieldEnd();
    }
    if(s.enableGiftSubscriptions.isSet()) {
        w.writeFieldBegin("enableGiftSubscriptions", ThriftFieldType::T_BOOL, 6);
        w.writeBool(s.enableGiftSubscriptions.ref());
        w.writeFieldEnd();
    }
    if(s.enableSupportTickets.isSet()) {
        w.writeFieldBegin("enableSupportTickets", ThriftFieldType::T_BOOL, 7);
        w.writeBool(s.enableSupportTickets.ref());
        w.writeFieldEnd();
    }
    if(s.enableSharedNotebooks.isSet()) {
        w.writeFieldBegin("enableSharedNotebooks", ThriftFieldType::T_BOOL, 8);
        w.writeBool(s.enableSharedNotebooks.ref());
        w.writeFieldEnd();
    }
    if(s.enableSingleNoteSharing.isSet()) {
        w.writeFieldBegin("enableSingleNoteSharing", ThriftFieldType::T_BOOL, 9);
        w.writeBool(s.enableSingleNoteSharing.ref());
        w.writeFieldEnd();
    }
    if(s.enableSponsoredAccounts.isSet()) {
        w.writeFieldBegin("enableSponsoredAccounts", ThriftFieldType::T_BOOL, 10);
        w.writeBool(s.enableSponsoredAccounts.ref());
        w.writeFieldEnd();
    }
    if(s.enableTwitterSharing.isSet()) {
        w.writeFieldBegin("enableTwitterSharing", ThriftFieldType::T_BOOL, 11);
        w.writeBool(s.enableTwitterSharing.ref());
        w.writeFieldEnd();
    }
    if(s.enableLinkedInSharing.isSet()) {
        w.writeFieldBegin("enableLinkedInSharing", ThriftFieldType::T_BOOL, 12);
        w.writeBool(s.enableLinkedInSharing.ref());
        w.writeFieldEnd();
    }
    if(s.enablePublicNotebooks.isSet()) {
        w.writeFieldBegin("enablePublicNotebooks", ThriftFieldType::T_BOOL, 13);
        w.writeBool(s.enablePublicNotebooks.ref());
        w.writeFieldEnd();
    }
    w.writeFieldStop();
    w.writeStructEnd();
}

void readBootstrapSettings(ThriftBinaryBufferReader& r, BootstrapSettings& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool serviceHost_isset = false;
    bool marketingUrl_isset = false;
    bool supportUrl_isset = false;
    bool accountEmailDomain_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                serviceHost_isset = true;
                QString v;
                r.readString(v);
                s.serviceHost = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRING) {
                marketingUrl_isset = true;
                QString v;
                r.readString(v);
                s.marketingUrl = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 3) {
            if(fieldType == ThriftFieldType::T_STRING) {
                supportUrl_isset = true;
                QString v;
                r.readString(v);
                s.supportUrl = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 4) {
            if(fieldType == ThriftFieldType::T_STRING) {
                accountEmailDomain_isset = true;
                QString v;
                r.readString(v);
                s.accountEmailDomain = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 5) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableFacebookSharing = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 6) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableGiftSubscriptions = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 7) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableSupportTickets = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 8) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableSharedNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 9) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableSingleNoteSharing = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 10) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableSponsoredAccounts = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 11) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableTwitterSharing = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 12) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enableLinkedInSharing = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 13) {
            if(fieldType == ThriftFieldType::T_BOOL) {
                bool v;
                r.readBool(v);
                s.enablePublicNotebooks = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!serviceHost_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "BootstrapSettings.serviceHost has no value");
    if(!marketingUrl_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "BootstrapSettings.marketingUrl has no value");
    if(!supportUrl_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "BootstrapSettings.supportUrl has no value");
    if(!accountEmailDomain_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "BootstrapSettings.accountEmailDomain has no value");
}

void writeBootstrapProfile(ThriftBinaryBufferWriter& w, const BootstrapProfile& s) {
    w.writeStructBegin("BootstrapProfile");
    w.writeFieldBegin("name", ThriftFieldType::T_STRING, 1);
    w.writeString(s.name);
    w.writeFieldEnd();
    w.writeFieldBegin("settings", ThriftFieldType::T_STRUCT, 2);
    writeBootstrapSettings(w, s.settings);
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
}

void readBootstrapProfile(ThriftBinaryBufferReader& r, BootstrapProfile& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool name_isset = false;
    bool settings_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_STRING) {
                name_isset = true;
                QString v;
                r.readString(v);
                s.name = v;
            } else {
                r.skip(fieldType);
            }
        } else
        if(fieldId == 2) {
            if(fieldType == ThriftFieldType::T_STRUCT) {
                settings_isset = true;
                BootstrapSettings v;
                readBootstrapSettings(r, v);
                s.settings = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!name_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "BootstrapProfile.name has no value");
    if(!settings_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "BootstrapProfile.settings has no value");
}

void writeBootstrapInfo(ThriftBinaryBufferWriter& w, const BootstrapInfo& s) {
    w.writeStructBegin("BootstrapInfo");
    w.writeFieldBegin("profiles", ThriftFieldType::T_LIST, 1);
    w.writeListBegin(ThriftFieldType::T_STRUCT, s.profiles.length());
    Q_FOREACH(const BootstrapProfile& elem, s.profiles) {
        writeBootstrapProfile(w, elem);
    }
    w.writeListEnd();
    w.writeFieldEnd();
    w.writeFieldStop();
    w.writeStructEnd();
}

void readBootstrapInfo(ThriftBinaryBufferReader& r, BootstrapInfo& s) {
    QString fname;
    ThriftFieldType::type fieldType;
    qint16 fieldId;
    bool profiles_isset = false;
    r.readStructBegin(fname);
    while(true) {
        r.readFieldBegin(fname, fieldType, fieldId);
        if(fieldType == ThriftFieldType::T_STOP) break;
        if(fieldId == 1) {
            if(fieldType == ThriftFieldType::T_LIST) {
                profiles_isset = true;
                QList< BootstrapProfile > v;
                quint32 size;
                ThriftFieldType::type elemType;
                r.readListBegin(elemType, size);
                v.reserve(size);
                if(elemType != ThriftFieldType::T_STRUCT) throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect list type (BootstrapInfo.profiles)");
                for(quint32 i = 0; i < size; i++) {
                    BootstrapProfile elem;
                    readBootstrapProfile(r, elem);
                    v.append(elem);
                }
                r.readListEnd();
                s.profiles = v;
            } else {
                r.skip(fieldType);
            }
        } else
        {
            r.skip(fieldType);
        }
        r.readFieldEnd();
    }
    r.readStructEnd();
    if(!profiles_isset) throw ThriftException(ThriftException::Type::INVALID_DATA, "BootstrapInfo.profiles has no value");
}


void readEnumEDAMErrorCode(ThriftBinaryBufferReader& r, EDAMErrorCode::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(EDAMErrorCode::UNKNOWN): e = EDAMErrorCode::UNKNOWN; break;
    case static_cast<int>(EDAMErrorCode::BAD_DATA_FORMAT): e = EDAMErrorCode::BAD_DATA_FORMAT; break;
    case static_cast<int>(EDAMErrorCode::PERMISSION_DENIED): e = EDAMErrorCode::PERMISSION_DENIED; break;
    case static_cast<int>(EDAMErrorCode::INTERNAL_ERROR): e = EDAMErrorCode::INTERNAL_ERROR; break;
    case static_cast<int>(EDAMErrorCode::DATA_REQUIRED): e = EDAMErrorCode::DATA_REQUIRED; break;
    case static_cast<int>(EDAMErrorCode::LIMIT_REACHED): e = EDAMErrorCode::LIMIT_REACHED; break;
    case static_cast<int>(EDAMErrorCode::QUOTA_REACHED): e = EDAMErrorCode::QUOTA_REACHED; break;
    case static_cast<int>(EDAMErrorCode::INVALID_AUTH): e = EDAMErrorCode::INVALID_AUTH; break;
    case static_cast<int>(EDAMErrorCode::AUTH_EXPIRED): e = EDAMErrorCode::AUTH_EXPIRED; break;
    case static_cast<int>(EDAMErrorCode::DATA_CONFLICT): e = EDAMErrorCode::DATA_CONFLICT; break;
    case static_cast<int>(EDAMErrorCode::ENML_VALIDATION): e = EDAMErrorCode::ENML_VALIDATION; break;
    case static_cast<int>(EDAMErrorCode::SHARD_UNAVAILABLE): e = EDAMErrorCode::SHARD_UNAVAILABLE; break;
    case static_cast<int>(EDAMErrorCode::LEN_TOO_SHORT): e = EDAMErrorCode::LEN_TOO_SHORT; break;
    case static_cast<int>(EDAMErrorCode::LEN_TOO_LONG): e = EDAMErrorCode::LEN_TOO_LONG; break;
    case static_cast<int>(EDAMErrorCode::TOO_FEW): e = EDAMErrorCode::TOO_FEW; break;
    case static_cast<int>(EDAMErrorCode::TOO_MANY): e = EDAMErrorCode::TOO_MANY; break;
    case static_cast<int>(EDAMErrorCode::UNSUPPORTED_OPERATION): e = EDAMErrorCode::UNSUPPORTED_OPERATION; break;
    case static_cast<int>(EDAMErrorCode::TAKEN_DOWN): e = EDAMErrorCode::TAKEN_DOWN; break;
    case static_cast<int>(EDAMErrorCode::RATE_LIMIT_REACHED): e = EDAMErrorCode::RATE_LIMIT_REACHED; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum EDAMErrorCode");
    }
}

void readEnumPrivilegeLevel(ThriftBinaryBufferReader& r, PrivilegeLevel::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(PrivilegeLevel::NORMAL): e = PrivilegeLevel::NORMAL; break;
    case static_cast<int>(PrivilegeLevel::PREMIUM): e = PrivilegeLevel::PREMIUM; break;
    case static_cast<int>(PrivilegeLevel::VIP): e = PrivilegeLevel::VIP; break;
    case static_cast<int>(PrivilegeLevel::MANAGER): e = PrivilegeLevel::MANAGER; break;
    case static_cast<int>(PrivilegeLevel::SUPPORT): e = PrivilegeLevel::SUPPORT; break;
    case static_cast<int>(PrivilegeLevel::ADMIN): e = PrivilegeLevel::ADMIN; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum PrivilegeLevel");
    }
}

void readEnumQueryFormat(ThriftBinaryBufferReader& r, QueryFormat::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(QueryFormat::USER): e = QueryFormat::USER; break;
    case static_cast<int>(QueryFormat::SEXP): e = QueryFormat::SEXP; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum QueryFormat");
    }
}

void readEnumNoteSortOrder(ThriftBinaryBufferReader& r, NoteSortOrder::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(NoteSortOrder::CREATED): e = NoteSortOrder::CREATED; break;
    case static_cast<int>(NoteSortOrder::UPDATED): e = NoteSortOrder::UPDATED; break;
    case static_cast<int>(NoteSortOrder::RELEVANCE): e = NoteSortOrder::RELEVANCE; break;
    case static_cast<int>(NoteSortOrder::UPDATE_SEQUENCE_NUMBER): e = NoteSortOrder::UPDATE_SEQUENCE_NUMBER; break;
    case static_cast<int>(NoteSortOrder::TITLE): e = NoteSortOrder::TITLE; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum NoteSortOrder");
    }
}

void readEnumPremiumOrderStatus(ThriftBinaryBufferReader& r, PremiumOrderStatus::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(PremiumOrderStatus::NONE): e = PremiumOrderStatus::NONE; break;
    case static_cast<int>(PremiumOrderStatus::PENDING): e = PremiumOrderStatus::PENDING; break;
    case static_cast<int>(PremiumOrderStatus::ACTIVE): e = PremiumOrderStatus::ACTIVE; break;
    case static_cast<int>(PremiumOrderStatus::FAILED): e = PremiumOrderStatus::FAILED; break;
    case static_cast<int>(PremiumOrderStatus::CANCELLATION_PENDING): e = PremiumOrderStatus::CANCELLATION_PENDING; break;
    case static_cast<int>(PremiumOrderStatus::CANCELED): e = PremiumOrderStatus::CANCELED; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum PremiumOrderStatus");
    }
}

void readEnumSharedNotebookPrivilegeLevel(ThriftBinaryBufferReader& r, SharedNotebookPrivilegeLevel::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(SharedNotebookPrivilegeLevel::READ_NOTEBOOK): e = SharedNotebookPrivilegeLevel::READ_NOTEBOOK; break;
    case static_cast<int>(SharedNotebookPrivilegeLevel::MODIFY_NOTEBOOK_PLUS_ACTIVITY): e = SharedNotebookPrivilegeLevel::MODIFY_NOTEBOOK_PLUS_ACTIVITY; break;
    case static_cast<int>(SharedNotebookPrivilegeLevel::READ_NOTEBOOK_PLUS_ACTIVITY): e = SharedNotebookPrivilegeLevel::READ_NOTEBOOK_PLUS_ACTIVITY; break;
    case static_cast<int>(SharedNotebookPrivilegeLevel::GROUP): e = SharedNotebookPrivilegeLevel::GROUP; break;
    case static_cast<int>(SharedNotebookPrivilegeLevel::FULL_ACCESS): e = SharedNotebookPrivilegeLevel::FULL_ACCESS; break;
    case static_cast<int>(SharedNotebookPrivilegeLevel::BUSINESS_FULL_ACCESS): e = SharedNotebookPrivilegeLevel::BUSINESS_FULL_ACCESS; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum SharedNotebookPrivilegeLevel");
    }
}

void readEnumSponsoredGroupRole(ThriftBinaryBufferReader& r, SponsoredGroupRole::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(SponsoredGroupRole::GROUP_MEMBER): e = SponsoredGroupRole::GROUP_MEMBER; break;
    case static_cast<int>(SponsoredGroupRole::GROUP_ADMIN): e = SponsoredGroupRole::GROUP_ADMIN; break;
    case static_cast<int>(SponsoredGroupRole::GROUP_OWNER): e = SponsoredGroupRole::GROUP_OWNER; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum SponsoredGroupRole");
    }
}

void readEnumBusinessUserRole(ThriftBinaryBufferReader& r, BusinessUserRole::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(BusinessUserRole::ADMIN): e = BusinessUserRole::ADMIN; break;
    case static_cast<int>(BusinessUserRole::NORMAL): e = BusinessUserRole::NORMAL; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum BusinessUserRole");
    }
}

void readEnumSharedNotebookInstanceRestrictions(ThriftBinaryBufferReader& r, SharedNotebookInstanceRestrictions::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(SharedNotebookInstanceRestrictions::ONLY_JOINED_OR_PREVIEW): e = SharedNotebookInstanceRestrictions::ONLY_JOINED_OR_PREVIEW; break;
    case static_cast<int>(SharedNotebookInstanceRestrictions::NO_SHARED_NOTEBOOKS): e = SharedNotebookInstanceRestrictions::NO_SHARED_NOTEBOOKS; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum SharedNotebookInstanceRestrictions");
    }
}

void readEnumReminderEmailConfig(ThriftBinaryBufferReader& r, ReminderEmailConfig::type& e) {
    qint32 i;
    r.readI32(i);
    switch(i) {
    case static_cast<int>(ReminderEmailConfig::DO_NOT_SEND): e = ReminderEmailConfig::DO_NOT_SEND; break;
    case static_cast<int>(ReminderEmailConfig::SEND_DAILY_EMAIL): e = ReminderEmailConfig::SEND_DAILY_EMAIL; break;
    default: throw ThriftException(ThriftException::Type::INVALID_DATA, "Incorrect value for enum ReminderEmailConfig");
    }
}

/** @endcond */



}
