// This file is generated from Evernote Thrift API and is a part of the QEverCloud project

#ifndef QEVERCLOUD_GENERATED_TYPES_IMPL_H
#define QEVERCLOUD_GENERATED_TYPES_IMPL_H

#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include "../globals.h"
#include "../impl.h"
#include "types.h"

namespace qevercloud {

/** @cond HIDDEN_SYMBOLS  */

void writeSyncState(ThriftBinaryBufferWriter& w, const SyncState& s);
void readSyncState(ThriftBinaryBufferReader& r, SyncState& s);
void writeSyncChunk(ThriftBinaryBufferWriter& w, const SyncChunk& s);
void readSyncChunk(ThriftBinaryBufferReader& r, SyncChunk& s);
void writeSyncChunkFilter(ThriftBinaryBufferWriter& w, const SyncChunkFilter& s);
void readSyncChunkFilter(ThriftBinaryBufferReader& r, SyncChunkFilter& s);
void writeNoteFilter(ThriftBinaryBufferWriter& w, const NoteFilter& s);
void readNoteFilter(ThriftBinaryBufferReader& r, NoteFilter& s);
void writeNoteList(ThriftBinaryBufferWriter& w, const NoteList& s);
void readNoteList(ThriftBinaryBufferReader& r, NoteList& s);
void writeNoteMetadata(ThriftBinaryBufferWriter& w, const NoteMetadata& s);
void readNoteMetadata(ThriftBinaryBufferReader& r, NoteMetadata& s);
void writeNotesMetadataList(ThriftBinaryBufferWriter& w, const NotesMetadataList& s);
void readNotesMetadataList(ThriftBinaryBufferReader& r, NotesMetadataList& s);
void writeNotesMetadataResultSpec(ThriftBinaryBufferWriter& w, const NotesMetadataResultSpec& s);
void readNotesMetadataResultSpec(ThriftBinaryBufferReader& r, NotesMetadataResultSpec& s);
void writeNoteCollectionCounts(ThriftBinaryBufferWriter& w, const NoteCollectionCounts& s);
void readNoteCollectionCounts(ThriftBinaryBufferReader& r, NoteCollectionCounts& s);
void writeNoteEmailParameters(ThriftBinaryBufferWriter& w, const NoteEmailParameters& s);
void readNoteEmailParameters(ThriftBinaryBufferReader& r, NoteEmailParameters& s);
void writeNoteVersionId(ThriftBinaryBufferWriter& w, const NoteVersionId& s);
void readNoteVersionId(ThriftBinaryBufferReader& r, NoteVersionId& s);
void writeClientUsageMetrics(ThriftBinaryBufferWriter& w, const ClientUsageMetrics& s);
void readClientUsageMetrics(ThriftBinaryBufferReader& r, ClientUsageMetrics& s);
void writeRelatedQuery(ThriftBinaryBufferWriter& w, const RelatedQuery& s);
void readRelatedQuery(ThriftBinaryBufferReader& r, RelatedQuery& s);
void writeRelatedResult(ThriftBinaryBufferWriter& w, const RelatedResult& s);
void readRelatedResult(ThriftBinaryBufferReader& r, RelatedResult& s);
void writeRelatedResultSpec(ThriftBinaryBufferWriter& w, const RelatedResultSpec& s);
void readRelatedResultSpec(ThriftBinaryBufferReader& r, RelatedResultSpec& s);
void writeData(ThriftBinaryBufferWriter& w, const Data& s);
void readData(ThriftBinaryBufferReader& r, Data& s);
void writeUserAttributes(ThriftBinaryBufferWriter& w, const UserAttributes& s);
void readUserAttributes(ThriftBinaryBufferReader& r, UserAttributes& s);
void writeAccounting(ThriftBinaryBufferWriter& w, const Accounting& s);
void readAccounting(ThriftBinaryBufferReader& r, Accounting& s);
void writeBusinessUserInfo(ThriftBinaryBufferWriter& w, const BusinessUserInfo& s);
void readBusinessUserInfo(ThriftBinaryBufferReader& r, BusinessUserInfo& s);
void writePremiumInfo(ThriftBinaryBufferWriter& w, const PremiumInfo& s);
void readPremiumInfo(ThriftBinaryBufferReader& r, PremiumInfo& s);
void writeUser(ThriftBinaryBufferWriter& w, const User& s);
void readUser(ThriftBinaryBufferReader& r, User& s);
void writeTag(ThriftBinaryBufferWriter& w, const Tag& s);
void readTag(ThriftBinaryBufferReader& r, Tag& s);
void writeLazyMap(ThriftBinaryBufferWriter& w, const LazyMap& s);
void readLazyMap(ThriftBinaryBufferReader& r, LazyMap& s);
void writeResourceAttributes(ThriftBinaryBufferWriter& w, const ResourceAttributes& s);
void readResourceAttributes(ThriftBinaryBufferReader& r, ResourceAttributes& s);
void writeResource(ThriftBinaryBufferWriter& w, const Resource& s);
void readResource(ThriftBinaryBufferReader& r, Resource& s);
void writeNoteAttributes(ThriftBinaryBufferWriter& w, const NoteAttributes& s);
void readNoteAttributes(ThriftBinaryBufferReader& r, NoteAttributes& s);
void writeNote(ThriftBinaryBufferWriter& w, const Note& s);
void readNote(ThriftBinaryBufferReader& r, Note& s);
void writePublishing(ThriftBinaryBufferWriter& w, const Publishing& s);
void readPublishing(ThriftBinaryBufferReader& r, Publishing& s);
void writeBusinessNotebook(ThriftBinaryBufferWriter& w, const BusinessNotebook& s);
void readBusinessNotebook(ThriftBinaryBufferReader& r, BusinessNotebook& s);
void writeSavedSearchScope(ThriftBinaryBufferWriter& w, const SavedSearchScope& s);
void readSavedSearchScope(ThriftBinaryBufferReader& r, SavedSearchScope& s);
void writeSavedSearch(ThriftBinaryBufferWriter& w, const SavedSearch& s);
void readSavedSearch(ThriftBinaryBufferReader& r, SavedSearch& s);
void writeSharedNotebookRecipientSettings(ThriftBinaryBufferWriter& w, const SharedNotebookRecipientSettings& s);
void readSharedNotebookRecipientSettings(ThriftBinaryBufferReader& r, SharedNotebookRecipientSettings& s);
void writeSharedNotebook(ThriftBinaryBufferWriter& w, const SharedNotebook& s);
void readSharedNotebook(ThriftBinaryBufferReader& r, SharedNotebook& s);
void writeNotebookRestrictions(ThriftBinaryBufferWriter& w, const NotebookRestrictions& s);
void readNotebookRestrictions(ThriftBinaryBufferReader& r, NotebookRestrictions& s);
void writeNotebook(ThriftBinaryBufferWriter& w, const Notebook& s);
void readNotebook(ThriftBinaryBufferReader& r, Notebook& s);
void writeLinkedNotebook(ThriftBinaryBufferWriter& w, const LinkedNotebook& s);
void readLinkedNotebook(ThriftBinaryBufferReader& r, LinkedNotebook& s);
void writeNotebookDescriptor(ThriftBinaryBufferWriter& w, const NotebookDescriptor& s);
void readNotebookDescriptor(ThriftBinaryBufferReader& r, NotebookDescriptor& s);
void writePublicUserInfo(ThriftBinaryBufferWriter& w, const PublicUserInfo& s);
void readPublicUserInfo(ThriftBinaryBufferReader& r, PublicUserInfo& s);
void writeAuthenticationResult(ThriftBinaryBufferWriter& w, const AuthenticationResult& s);
void readAuthenticationResult(ThriftBinaryBufferReader& r, AuthenticationResult& s);
void writeBootstrapSettings(ThriftBinaryBufferWriter& w, const BootstrapSettings& s);
void readBootstrapSettings(ThriftBinaryBufferReader& r, BootstrapSettings& s);
void writeBootstrapProfile(ThriftBinaryBufferWriter& w, const BootstrapProfile& s);
void readBootstrapProfile(ThriftBinaryBufferReader& r, BootstrapProfile& s);
void writeBootstrapInfo(ThriftBinaryBufferWriter& w, const BootstrapInfo& s);
void readBootstrapInfo(ThriftBinaryBufferReader& r, BootstrapInfo& s);

void readEDAMUserException(ThriftBinaryBufferReader& r, EDAMUserException& e);
void readEDAMSystemException(ThriftBinaryBufferReader& r, EDAMSystemException& e);
void readEDAMNotFoundException(ThriftBinaryBufferReader& r, EDAMNotFoundException& e);

void readEnumEDAMErrorCode(ThriftBinaryBufferReader& r, EDAMErrorCode::type& e);
void readEnumPrivilegeLevel(ThriftBinaryBufferReader& r, PrivilegeLevel::type& e);
void readEnumQueryFormat(ThriftBinaryBufferReader& r, QueryFormat::type& e);
void readEnumNoteSortOrder(ThriftBinaryBufferReader& r, NoteSortOrder::type& e);
void readEnumPremiumOrderStatus(ThriftBinaryBufferReader& r, PremiumOrderStatus::type& e);
void readEnumSharedNotebookPrivilegeLevel(ThriftBinaryBufferReader& r, SharedNotebookPrivilegeLevel::type& e);
void readEnumSponsoredGroupRole(ThriftBinaryBufferReader& r, SponsoredGroupRole::type& e);
void readEnumBusinessUserRole(ThriftBinaryBufferReader& r, BusinessUserRole::type& e);
void readEnumSharedNotebookInstanceRestrictions(ThriftBinaryBufferReader& r, SharedNotebookInstanceRestrictions::type& e);
void readEnumReminderEmailConfig(ThriftBinaryBufferReader& r, ReminderEmailConfig::type& e);

/** @endcond */

}
#endif // QEVERCLOUD_GENERATED_TYPES_IMPL_H
