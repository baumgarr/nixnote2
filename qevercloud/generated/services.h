// This file is generated from Evernote Thrift API and is a part of the QEverCloud project

#ifndef QEVERCLOUD_GENERATED_SERVICES_H
#define QEVERCLOUD_GENERATED_SERVICES_H

#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QDateTime>
#include "../Optional.h"
#include <QObject>
#include "constants.h"
#include "types.h"
#include "../AsyncResult.h"

namespace qevercloud {

/**
 * Service:  NoteStore
 * <p>
 * The NoteStore service is used by EDAM clients to exchange information
 * about the collection of notes in an account.  This is primarily used for
 * synchronization, but could also be used by a "thin" client without a full
 * local cache.
 * </p><p>
 * All functions take an "authenticationToken" parameter, which is the
 * value returned by the UserStore which permits access to the account.
 * This parameter is mandatory for all functions.
 * </p>
 *
 * Calls which require an authenticationToken may throw an EDAMUserException
 * for the following reasons:
 *  <ul>
 *   <li> AUTH_EXPIRED "authenticationToken" - token has expired
 *   </li>
 *   <li> BAD_DATA_FORMAT "authenticationToken" - token is malformed
 *   </li>
 *   <li> DATA_REQUIRED "authenticationToken" - token is empty
 *   </li>
 *   <li> INVALID_AUTH "authenticationToken" - token signature is invalid
 *   </li>
 * </ul>
 */
class NoteStore: public QObject {
    Q_DISABLE_COPY(NoteStore)
public:
    explicit NoteStore(QString noteStoreUrl = QString(), QString authenticationToken = QString(), QObject *parent = 0);
    explicit NoteStore(QObject *parent);

    void setNoteStoreUrl(QString noteStoreUrl) {url_ = noteStoreUrl;}
    QString noteStoreUrl() {return url_;}

    void setAuthenticationToken(QString authenticationToken) {authenticationToken_ = authenticationToken;}
    QString authenticationToken() {return authenticationToken_ ;}

    /**
       * Asks the NoteStore to provide information about the status of the user
       * account corresponding to the provided authentication token.
       */
    SyncState getSyncState(QString authenticationToken = QString());

    /** Asynchronous version of @link getSyncState @endlink */
    AsyncResult* getSyncStateAsync(QString authenticationToken = QString());

    /**
       * Asks the NoteStore to provide information about the status of the user
       * account corresponding to the provided authentication token.
       * This version of 'getSyncState' allows the client to upload coarse-
       * grained usage metrics to the service.
       *
       * @param clientMetrics  see the documentation of the ClientUsageMetrics
       *   structure for an explanation of the fields that clients can pass to
       *   the service.
       */
    SyncState getSyncStateWithMetrics(const ClientUsageMetrics& clientMetrics, QString authenticationToken = QString());

    /** Asynchronous version of @link getSyncStateWithMetrics @endlink */
    AsyncResult* getSyncStateWithMetricsAsync(const ClientUsageMetrics& clientMetrics, QString authenticationToken = QString());

    /**
       * DEPRECATED - use getFilteredSyncChunk.
       */
    SyncChunk getSyncChunk(qint32 afterUSN, qint32 maxEntries, bool fullSyncOnly, QString authenticationToken = QString());

    /** Asynchronous version of @link getSyncChunk @endlink */
    AsyncResult* getSyncChunkAsync(qint32 afterUSN, qint32 maxEntries, bool fullSyncOnly, QString authenticationToken = QString());

    /**
       * Asks the NoteStore to provide the state of the account in order of
       * last modification.  This request retrieves one block of the server's
       * state so that a client can make several small requests against a large
       * account rather than getting the entire state in one big message.
       * This call gives fine-grained control of the data that will
       * be received by a client by omitting data elements that a client doesn't
       * need. This may reduce network traffic and sync times.
       *
       * @param afterUSN
       *   The client can pass this value to ask only for objects that
       *   have been updated after a certain point.  This allows the client to
       *   receive updates after its last checkpoint rather than doing a full
       *   synchronization on every pass.  The default value of "0" indicates
       *   that the client wants to get objects from the start of the account.
       *
       * @param maxEntries
       *   The maximum number of modified objects that should be
       *   returned in the result SyncChunk.  This can be used to limit the size
       *   of each individual message to be friendly for network transfer.
       *
       * @param filter
       *   The caller must set some of the flags in this structure to specify which
       *   data types should be returned during the synchronization.  See
       *   the SyncChunkFilter structure for information on each flag.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "afterUSN" - if negative
       *   </li>
       *   <li> BAD_DATA_FORMAT "maxEntries" - if less than 1
       *   </li>
       * </ul>
       */
    SyncChunk getFilteredSyncChunk(qint32 afterUSN, qint32 maxEntries, const SyncChunkFilter& filter, QString authenticationToken = QString());

    /** Asynchronous version of @link getFilteredSyncChunk @endlink */
    AsyncResult* getFilteredSyncChunkAsync(qint32 afterUSN, qint32 maxEntries, const SyncChunkFilter& filter, QString authenticationToken = QString());

    /**
       * Asks the NoteStore to provide information about the status of a linked
       * notebook that has been shared with the caller, or that is public to the
       * world.
       * This will return a result that is similar to getSyncState, but may omit
       * SyncState.uploaded if the caller doesn't have permission to write to
       * the linked notebook.
       *
       * This function must be called on the shard that owns the referenced
       * notebook.  (I.e. the shardId in /shard/shardId/edam/note must be the
       * same as LinkedNotebook.shardId.)
       *
       * @param authenticationToken
       *   This should be an authenticationToken for the guest who has received
       *   the invitation to the share.  (I.e. this should not be the result of
       *   NoteStore.authenticateToSharedNotebook)
       *
       * @param linkedNotebook
       *   This structure should contain identifying information and permissions
       *   to access the notebook in question.
       */
    SyncState getLinkedNotebookSyncState(const LinkedNotebook& linkedNotebook, QString authenticationToken = QString());

    /** Asynchronous version of @link getLinkedNotebookSyncState @endlink */
    AsyncResult* getLinkedNotebookSyncStateAsync(const LinkedNotebook& linkedNotebook, QString authenticationToken = QString());

    /**
       * Asks the NoteStore to provide information about the contents of a linked
       * notebook that has been shared with the caller, or that is public to the
       * world.
       * This will return a result that is similar to getSyncChunk, but will only
       * contain entries that are visible to the caller.  I.e. only that particular
       * Notebook will be visible, along with its Notes, and Tags on those Notes.
       *
       * This function must be called on the shard that owns the referenced
       * notebook.  (I.e. the shardId in /shard/shardId/edam/note must be the
       * same as LinkedNotebook.shardId.)
       *
       * @param authenticationToken
       *   This should be an authenticationToken for the guest who has received
       *   the invitation to the share.  (I.e. this should not be the result of
       *   NoteStore.authenticateToSharedNotebook)
       *
       * @param linkedNotebook
       *   This structure should contain identifying information and permissions
       *   to access the notebook in question.  This must contain the valid fields
       *   for either a shared notebook (e.g. shareKey)
       *   or a public notebook (e.g. username, uri)
       *
       * @param afterUSN
       *   The client can pass this value to ask only for objects that
       *   have been updated after a certain point.  This allows the client to
       *   receive updates after its last checkpoint rather than doing a full
       *   synchronization on every pass.  The default value of "0" indicates
       *   that the client wants to get objects from the start of the account.
       *
       * @param maxEntries
       *   The maximum number of modified objects that should be
       *   returned in the result SyncChunk.  This can be used to limit the size
       *   of each individual message to be friendly for network transfer.
       *   Applications should not request more than 256 objects at a time,
       *   and must handle the case where the service returns less than the
       *   requested number of objects in a given request even though more
       *   objects are available on the service.
       *
       * @param fullSyncOnly
       *   If true, then the client only wants initial data for a full sync.
       *   In this case, the service will not return any expunged objects,
       *   and will not return any Resources, since these are also provided
       *   in their corresponding Notes.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "afterUSN" - if negative
       *   </li>
       *   <li> BAD_DATA_FORMAT "maxEntries" - if less than 1
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "LinkedNotebook" - if the provided information doesn't match any
       *     valid notebook
       *   </li>
       *   <li> "LinkedNotebook.uri" - if the provided public URI doesn't match any
       *     valid notebook
       *   </li>
       *   <li> "SharedNotebook.id" - if the provided information indicates a
       *      shared notebook that no longer exists
       *   </li>
       * </ul>
       */
    SyncChunk getLinkedNotebookSyncChunk(const LinkedNotebook& linkedNotebook, qint32 afterUSN, qint32 maxEntries, bool fullSyncOnly, QString authenticationToken = QString());

    /** Asynchronous version of @link getLinkedNotebookSyncChunk @endlink */
    AsyncResult* getLinkedNotebookSyncChunkAsync(const LinkedNotebook& linkedNotebook, qint32 afterUSN, qint32 maxEntries, bool fullSyncOnly, QString authenticationToken = QString());

    /**
       * Returns a list of all of the notebooks in the account.
       */
    QList< Notebook > listNotebooks(QString authenticationToken = QString());

    /** Asynchronous version of @link listNotebooks @endlink */
    AsyncResult* listNotebooksAsync(QString authenticationToken = QString());

    /**
       * Returns the current state of the notebook with the provided GUID.
       * The notebook may be active or deleted (but not expunged).
       *
       * @param guid
       *   The GUID of the notebook to be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Notebook.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Notebook" - private notebook, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - tag not found, by GUID
       *   </li>
       * </ul>
       */
    Notebook getNotebook(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getNotebook @endlink */
    AsyncResult* getNotebookAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Returns the notebook that should be used to store new notes in the
       * user's account when no other notebooks are specified.
       */
    Notebook getDefaultNotebook(QString authenticationToken = QString());

    /** Asynchronous version of @link getDefaultNotebook @endlink */
    AsyncResult* getDefaultNotebookAsync(QString authenticationToken = QString());

    /**
       * Asks the service to make a notebook with the provided name.
       *
       * @param notebook
       *   The desired fields for the notebook must be provided on this
       *   object.  The name of the notebook must be set, and either the 'active'
       *   or 'defaultNotebook' fields may be set by the client at creation.
       *   If a notebook exists in the account with the same name (via
       *   case-insensitive compare), this will throw an EDAMUserException.
       *
       * @return
       *   The newly created Notebook.  The server-side GUID will be
       *   saved in this object's 'guid' field.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Notebook.name" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Notebook.stack" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Publishing.uri" - if publishing set but bad uri
       *   </li>
       *   <li> BAD_DATA_FORMAT "Publishing.publicDescription" - if too long
       *   </li>
       *   <li> DATA_CONFLICT "Notebook.name" - name already in use
       *   </li>
       *   <li> DATA_CONFLICT "Publishing.uri" - if URI already in use
       *   </li>
       *   <li> DATA_REQUIRED "Publishing.uri" - if publishing set but uri missing
       *   </li>
       *   <li> LIMIT_REACHED "Notebook" - at max number of notebooks
       *   </li>
       * </ul>
       */
    Notebook createNotebook(const Notebook& notebook, QString authenticationToken = QString());

    /** Asynchronous version of @link createNotebook @endlink */
    AsyncResult* createNotebookAsync(const Notebook& notebook, QString authenticationToken = QString());

    /**
       * Submits notebook changes to the service.  The provided data must include
       * the notebook's guid field for identification.
       *
       * @param notebook
       *   The notebook object containing the requested changes.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Notebook.name" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Notebook.stack" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Publishing.uri" - if publishing set but bad uri
       *   </li>
       *   <li> BAD_DATA_FORMAT "Publishing.publicDescription" - if too long
       *   </li>
       *   <li> DATA_CONFLICT "Notebook.name" - name already in use
       *   </li>
       *   <li> DATA_CONFLICT "Publishing.uri" - if URI already in use
       *   </li>
       *   <li> DATA_REQUIRED "Publishing.uri" - if publishing set but uri missing
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 updateNotebook(const Notebook& notebook, QString authenticationToken = QString());

    /** Asynchronous version of @link updateNotebook @endlink */
    AsyncResult* updateNotebookAsync(const Notebook& notebook, QString authenticationToken = QString());

    /**
       * Permanently removes the notebook from the user's account.
       * After this action, the notebook is no longer available for undeletion, etc.
       * If the notebook contains any Notes, they will be moved to the current
       * default notebook and moved into the trash (i.e. Note.active=false).
       * <p/>
       * NOTE: This function is generally not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param guid
       *   The GUID of the notebook to delete.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Notebook.guid" - if the parameter is missing
       *   </li>
       *   <li> LIMIT_REACHED "Notebook" - trying to expunge the last Notebook
       *   </li>
       *   <li> PERMISSION_DENIED "Notebook" - private notebook, user doesn't own
       *   </li>
       * </ul>
       */
    qint32 expungeNotebook(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link expungeNotebook @endlink */
    AsyncResult* expungeNotebookAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Returns a list of the tags in the account.  Evernote does not support
       * the undeletion of tags, so this will only include active tags.
       */
    QList< Tag > listTags(QString authenticationToken = QString());

    /** Asynchronous version of @link listTags @endlink */
    AsyncResult* listTagsAsync(QString authenticationToken = QString());

    /**
       * Returns a list of the tags that are applied to at least one note within
       * the provided notebook.  If the notebook is public, the authenticationToken
       * may be ignored.
       *
       * @param notebookGuid
       *    the GUID of the notebook to use to find tags
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - notebook not found by GUID
       *   </li>
       * </ul>
       */
    QList< Tag > listTagsByNotebook(Guid notebookGuid, QString authenticationToken = QString());

    /** Asynchronous version of @link listTagsByNotebook @endlink */
    AsyncResult* listTagsByNotebookAsync(Guid notebookGuid, QString authenticationToken = QString());

    /**
       * Returns the current state of the Tag with the provided GUID.
       *
       * @param guid
       *   The GUID of the tag to be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Tag.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Tag" - private Tag, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Tag.guid" - tag not found, by GUID
       *   </li>
       * </ul>
       */
    Tag getTag(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getTag @endlink */
    AsyncResult* getTagAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Asks the service to make a tag with a set of information.
       *
       * @param tag
       *   The desired list of fields for the tag are specified in this
       *   object.  The caller must specify the tag name, and may provide
       *   the parentGUID.
       *
       * @return
       *   The newly created Tag.  The server-side GUID will be
       *   saved in this object.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Tag.name" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Tag.parentGuid" - malformed GUID
       *   </li>
       *   <li> DATA_CONFLICT "Tag.name" - name already in use
       *   </li>
       *   <li> LIMIT_REACHED "Tag" - at max number of tags
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Tag.parentGuid" - not found, by GUID
       *   </li>
       * </ul>
       */
    Tag createTag(const Tag& tag, QString authenticationToken = QString());

    /** Asynchronous version of @link createTag @endlink */
    AsyncResult* createTagAsync(const Tag& tag, QString authenticationToken = QString());

    /**
       * Submits tag changes to the service.  The provided data must include
       * the tag's guid field for identification.  The service will apply
       * updates to the following tag fields:  name, parentGuid
       *
       * @param tag
       *   The tag object containing the requested changes.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Tag.name" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Tag.parentGuid" - malformed GUID
       *   </li>
       *   <li> DATA_CONFLICT "Tag.name" - name already in use
       *   </li>
       *   <li> DATA_CONFLICT "Tag.parentGuid" - can't set parent: circular
       *   </li>
       *   <li> PERMISSION_DENIED "Tag" - user doesn't own tag
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Tag.guid" - tag not found, by GUID
       *   </li>
       *   <li> "Tag.parentGuid" - parent not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 updateTag(const Tag& tag, QString authenticationToken = QString());

    /** Asynchronous version of @link updateTag @endlink */
    AsyncResult* updateTagAsync(const Tag& tag, QString authenticationToken = QString());

    /**
       * Removes the provided tag from every note that is currently tagged with
       * this tag.  If this operation is successful, the tag will still be in
       * the account, but it will not be tagged on any notes.
       *
       * This function is not indended for use by full synchronizing clients, since
       * it does not provide enough result information to the client to reconcile
       * the local state without performing a follow-up sync from the service.  This
       * is intended for "thin clients" that need to efficiently support this as
       * a UI operation.
       *
       * @param guid
       *   The GUID of the tag to remove from all notes.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Tag.guid" - if the guid parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Tag" - user doesn't own tag
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Tag.guid" - tag not found, by GUID
       *   </li>
       * </ul>
       */
    void untagAll(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link untagAll @endlink */
    AsyncResult* untagAllAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Permanently deletes the tag with the provided GUID, if present.
       * <p/>
       * NOTE: This function is generally not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param guid
       *   The GUID of the tag to delete.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Tag.guid" - if the guid parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Tag" - user doesn't own tag
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Tag.guid" - tag not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 expungeTag(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link expungeTag @endlink */
    AsyncResult* expungeTagAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Returns a list of the searches in the account.  Evernote does not support
       * the undeletion of searches, so this will only include active searches.
       */
    QList< SavedSearch > listSearches(QString authenticationToken = QString());

    /** Asynchronous version of @link listSearches @endlink */
    AsyncResult* listSearchesAsync(QString authenticationToken = QString());

    /**
       * Returns the current state of the search with the provided GUID.
       *
       * @param guid
       *   The GUID of the search to be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "SavedSearch.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "SavedSearch" - private Tag, user doesn't own
       *   </li>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "SavedSearch.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    SavedSearch getSearch(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getSearch @endlink */
    AsyncResult* getSearchAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Asks the service to make a saved search with a set of information.
       *
       * @param search
       *   The desired list of fields for the search are specified in this
       *   object. The caller must specify the name and query for the
       *   search, and may optionally specify a search scope.
       *   The SavedSearch.format field is ignored by the service.
       *
       * @return
       *   The newly created SavedSearch.  The server-side GUID will be
       *   saved in this object.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "SavedSearch.name" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "SavedSearch.query" - invalid length
       *   </li>
       *   <li> DATA_CONFLICT "SavedSearch.name" - name already in use
       *   </li>
       *   <li> LIMIT_REACHED "SavedSearch" - at max number of searches
       *   </li>
       * </ul>
       */
    SavedSearch createSearch(const SavedSearch& search, QString authenticationToken = QString());

    /** Asynchronous version of @link createSearch @endlink */
    AsyncResult* createSearchAsync(const SavedSearch& search, QString authenticationToken = QString());

    /**
       * Submits search changes to the service. The provided data must include
       * the search's guid field for identification. The service will apply
       * updates to the following search fields: name, query, and scope.
       *
       * @param search
       *   The search object containing the requested changes.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "SavedSearch.name" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "SavedSearch.query" - invalid length
       *   </li>
       *   <li> DATA_CONFLICT "SavedSearch.name" - name already in use
       *   </li>
       *   <li> PERMISSION_DENIED "SavedSearch" - user doesn't own tag
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "SavedSearch.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 updateSearch(const SavedSearch& search, QString authenticationToken = QString());

    /** Asynchronous version of @link updateSearch @endlink */
    AsyncResult* updateSearchAsync(const SavedSearch& search, QString authenticationToken = QString());

    /**
       * Permanently deletes the saved search with the provided GUID, if present.
       * <p/>
       * NOTE: This function is generally not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param guid
       *   The GUID of the search to delete.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "SavedSearch.guid" - if the guid parameter is empty
       *   </li>
       *   <li> PERMISSION_DENIED "SavedSearch" - user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "SavedSearch.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 expungeSearch(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link expungeSearch @endlink */
    AsyncResult* expungeSearchAsync(Guid guid, QString authenticationToken = QString());

    /**
       * DEPRECATED. Use findNotesMetadata.
       */
    NoteList findNotes(const NoteFilter& filter, qint32 offset, qint32 maxNotes, QString authenticationToken = QString());

    /** Asynchronous version of @link findNotes @endlink */
    AsyncResult* findNotesAsync(const NoteFilter& filter, qint32 offset, qint32 maxNotes, QString authenticationToken = QString());

    /**
       * Finds the position of a note within a sorted subset of all of the user's
       * notes. This may be useful for thin clients that are displaying a paginated
       * listing of a large account, which need to know where a particular note
       * sits in the list without retrieving all notes first.
       *
       * @param authenticationToken
       *   Must be a valid token for the user's account unless the NoteFilter
       *   'notebookGuid' is the GUID of a public notebook.
       *
       * @param filter
       *   The list of criteria that will constrain the notes to be returned.
       *
       * @param guid
       *   The GUID of the note to be retrieved.
       *
       * @return
       *   If the note with the provided GUID is found within the matching note
       *   list, this will return the offset of that note within that list (where
       *   the first offset is 0).  If the note is not found within the set of
       *   notes, this will return -1.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "offset" - not between 0 and EDAM_USER_NOTES_MAX
       *   </li>
       *   <li> BAD_DATA_FORMAT "maxNotes" - not between 0 and EDAM_USER_NOTES_MAX
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.notebookGuid" - if malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.tagGuids" - if any are malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.words" - if search string too long
       *   </li>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - not found, by GUID
       *   </li>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 findNoteOffset(const NoteFilter& filter, Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link findNoteOffset @endlink */
    AsyncResult* findNoteOffsetAsync(const NoteFilter& filter, Guid guid, QString authenticationToken = QString());

    /**
       * Used to find the high-level information about a set of the notes from a
       * user's account based on various criteria specified via a NoteFilter object.
       * <p/>
       * Web applications that wish to periodically check for new content in a user's
       * Evernote account should consider using webhooks instead of polling this API.
       * See http://dev.evernote.com/documentation/cloud/chapters/polling_notification.php
       * for more information.
       *
       * @param authenticationToken
       *   Must be a valid token for the user's account unless the NoteFilter
       *   'notebookGuid' is the GUID of a public notebook.
       *
       * @param filter
       *   The list of criteria that will constrain the notes to be returned.
       *
       * @param offset
       *   The numeric index of the first note to show within the sorted
       *   results.  The numbering scheme starts with "0".  This can be used for
       *   pagination.
       *
       * @param maxNotes
       *   The mximum notes to return in this query.  The service will return a set
       *   of notes that is no larger than this number, but may return fewer notes
       *   if needed.  The NoteList.totalNotes field in the return value will
       *   indicate whether there are more values available after the returned set.
       *
       * @param resultSpec
       *   This specifies which information should be returned for each matching
       *   Note. The fields on this structure can be used to eliminate data that
       *   the client doesn't need, which will reduce the time and bandwidth
       *   to receive and process the reply.
       *
       * @return
       *   The list of notes that match the criteria.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "offset" - not between 0 and EDAM_USER_NOTES_MAX
       *   </li>
       *   <li> BAD_DATA_FORMAT "maxNotes" - not between 0 and EDAM_USER_NOTES_MAX
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.notebookGuid" - if malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.tagGuids" - if any are malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.words" - if search string too long
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    NotesMetadataList findNotesMetadata(const NoteFilter& filter, qint32 offset, qint32 maxNotes, const NotesMetadataResultSpec& resultSpec, QString authenticationToken = QString());

    /** Asynchronous version of @link findNotesMetadata @endlink */
    AsyncResult* findNotesMetadataAsync(const NoteFilter& filter, qint32 offset, qint32 maxNotes, const NotesMetadataResultSpec& resultSpec, QString authenticationToken = QString());

    /**
       * This function is used to determine how many notes are found for each
       * notebook and tag in the user's account, given a current set of filter
       * parameters that determine the current selection.  This function will
       * return a structure that gives the note count for each notebook and tag
       * that has at least one note under the requested filter.  Any notebook or
       * tag that has zero notes in the filtered set will not be listed in the
       * reply to this function (so they can be assumed to be 0).
       *
       * @param authenticationToken
       *   Must be a valid token for the user's account unless the NoteFilter
       *   'notebookGuid' is the GUID of a public notebook.
       *
       * @param filter
       *   The note selection filter that is currently being applied.  The note
       *   counts are to be calculated with this filter applied to the total set
       *   of notes in the user's account.
       *
       * @param withTrash
       *   If true, then the NoteCollectionCounts.trashCount will be calculated
       *   and supplied in the reply. Otherwise, the trash value will be omitted.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "NoteFilter.notebookGuid" - if malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.notebookGuids" - if any are malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.words" - if search string too long
       *   </li>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    NoteCollectionCounts findNoteCounts(const NoteFilter& filter, bool withTrash, QString authenticationToken = QString());

    /** Asynchronous version of @link findNoteCounts @endlink */
    AsyncResult* findNoteCountsAsync(const NoteFilter& filter, bool withTrash, QString authenticationToken = QString());

    /**
       * Returns the current state of the note in the service with the provided
       * GUID.  The ENML contents of the note will only be provided if the
       * 'withContent' parameter is true.  The service will include the meta-data
       * for each resource in the note, but the binary contents of the resources
       * and their recognition data will be omitted.
       * If the Note is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).  The applicationData
       * fields are returned as keysOnly.
       *
       * @param guid
       *   The GUID of the note to be retrieved.
       *
       * @param withContent
       *   If true, the note will include the ENML contents of its
       *   'content' field.
       *
       * @param withResourcesData
       *   If true, any Resource elements in this Note will include the binary
       *   contents of their 'data' field's body.
       *
       * @param withResourcesRecognition
       *   If true, any Resource elements will include the binary contents of the
       *   'recognition' field's body if recognition data is present.
       *
       * @param withResourcesAlternateData
       *   If true, any Resource elements in this Note will include the binary
       *   contents of their 'alternateData' fields' body, if an alternate form
       *   is present.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    Note getNote(Guid guid, bool withContent, bool withResourcesData, bool withResourcesRecognition, bool withResourcesAlternateData, QString authenticationToken = QString());

    /** Asynchronous version of @link getNote @endlink */
    AsyncResult* getNoteAsync(Guid guid, bool withContent, bool withResourcesData, bool withResourcesRecognition, bool withResourcesAlternateData, QString authenticationToken = QString());

    /**
       * Get all of the application data for the note identified by GUID,
       * with values returned within the LazyMap fullMap field.
       * If there are no applicationData entries, then a LazyMap
       * with an empty fullMap will be returned. If your application
       * only needs to fetch its own applicationData entry, use
       * getNoteApplicationDataEntry instead.
       */
    LazyMap getNoteApplicationData(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getNoteApplicationData @endlink */
    AsyncResult* getNoteApplicationDataAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Get the value of a single entry in the applicationData map
       * for the note identified by GUID.
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - note not found, by GUID</li>
       *   <li> "NoteAttributes.applicationData.key" - note not found, by key</li>
       * </ul>
       */
    QString getNoteApplicationDataEntry(Guid guid, QString key, QString authenticationToken = QString());

    /** Asynchronous version of @link getNoteApplicationDataEntry @endlink */
    AsyncResult* getNoteApplicationDataEntryAsync(Guid guid, QString key, QString authenticationToken = QString());

    /**
       * Update, or create, an entry in the applicationData map for
       * the note identified by guid.
       */
    qint32 setNoteApplicationDataEntry(Guid guid, QString key, QString value, QString authenticationToken = QString());

    /** Asynchronous version of @link setNoteApplicationDataEntry @endlink */
    AsyncResult* setNoteApplicationDataEntryAsync(Guid guid, QString key, QString value, QString authenticationToken = QString());

    /**
       * Remove an entry identified by 'key' from the applicationData map for
       * the note identified by 'guid'. Silently ignores an unset of a
       * non-existing key.
       */
    qint32 unsetNoteApplicationDataEntry(Guid guid, QString key, QString authenticationToken = QString());

    /** Asynchronous version of @link unsetNoteApplicationDataEntry @endlink */
    AsyncResult* unsetNoteApplicationDataEntryAsync(Guid guid, QString key, QString authenticationToken = QString());

    /**
       * Returns XHTML contents of the note with the provided GUID.
       * If the Note is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).
       *
       * @param guid
       *   The GUID of the note to be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    QString getNoteContent(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getNoteContent @endlink */
    AsyncResult* getNoteContentAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Returns a block of the extracted plain text contents of the note with the
       * provided GUID.  This text can be indexed for search purposes by a light
       * client that doesn't have capabilities to extract all of the searchable
       * text content from the note and its resources.
       *
       * If the Note is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).
       *
       * @param guid
       *   The GUID of the note to be retrieved.
       *
       * @param noteOnly
       *   If true, this will only return the text extracted from the ENML contents
       *   of the note itself.  If false, this will also include the extracted text
       *   from any text-bearing resources (PDF, recognized images)
       *
       * @param tokenizeForIndexing
       *   If true, this will break the text into cleanly separated and sanitized
       *   tokens.  If false, this will return the more raw text extraction, with
       *   its original punctuation, capitalization, spacing, etc.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    QString getNoteSearchText(Guid guid, bool noteOnly, bool tokenizeForIndexing, QString authenticationToken = QString());

    /** Asynchronous version of @link getNoteSearchText @endlink */
    AsyncResult* getNoteSearchTextAsync(Guid guid, bool noteOnly, bool tokenizeForIndexing, QString authenticationToken = QString());

    /**
       * Returns a block of the extracted plain text contents of the resource with
       * the provided GUID.  This text can be indexed for search purposes by a light
       * client that doesn't have capability to extract all of the searchable
       * text content from a resource.
       *
       * If the Resource is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).
       *
       * @param guid
       *   The GUID of the resource to be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Resource.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    QString getResourceSearchText(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceSearchText @endlink */
    AsyncResult* getResourceSearchTextAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Returns a list of the names of the tags for the note with the provided
       * guid.  This can be used with authentication to get the tags for a
       * user's own note, or can be used without valid authentication to retrieve
       * the names of the tags for a note in a public notebook.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    QStringList getNoteTagNames(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getNoteTagNames @endlink */
    AsyncResult* getNoteTagNamesAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Asks the service to make a note with the provided set of information.
       *
       * @param note
       *   A Note object containing the desired fields to be populated on
       *   the service.
       *
       * @return
       *   The newly created Note from the service.  The server-side
       *   GUIDs for the Note and any Resources will be saved in this object.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.title" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Note.content" - invalid length for ENML content
       *   </li>
       *   <li> BAD_DATA_FORMAT "Resource.mime" - invalid resource MIME type
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteAttributes.*" - bad resource string
       *   </li>
       *   <li> BAD_DATA_FORMAT "ResourceAttributes.*" - bad resource string
       *   </li>
       *   <li> DATA_CONFLICT "Note.deleted" - deleted time set on active note
       *   </li>
       *   <li> DATA_REQUIRED "Resource.data" - resource data body missing
       *   </li>
       *   <li> ENML_VALIDATION "*" - note content doesn't validate against DTD
       *   </li>
       *   <li> LIMIT_REACHED "Note" - at max number per account
       *   </li>
       *   <li> LIMIT_REACHED "Note.size" - total note size too large
       *   </li>
       *   <li> LIMIT_REACHED "Note.resources" - too many resources on Note
       *   </li>
       *   <li> LIMIT_REACHED "Note.tagGuids" - too many Tags on Note
       *   </li>
       *   <li> LIMIT_REACHED "Resource.data.size" - resource too large
       *   </li>
       *   <li> LIMIT_REACHED "NoteAttribute.*" - attribute string too long
       *   </li>
       *   <li> LIMIT_REACHED "ResourceAttribute.*" - attribute string too long
       *   </li>
       *   <li> PERMISSION_DENIED "Note.notebookGuid" - NB not owned by user
       *   </li>
       *   <li> QUOTA_REACHED "Accounting.uploadLimit" - note exceeds upload quota
       *   </li>
       *   <li> BAD_DATA_FORMAT "Tag.name" - Note.tagNames was provided, and one
       *     of the specified tags had an invalid length or pattern
       *   </li>
       *   <li> LIMIT_REACHED "Tag" - Note.tagNames was provided, and the required
       *     new tags would exceed the maximum number per account
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.notebookGuid" - not found, by GUID
       *   </li>
       * </ul>
       */
    Note createNote(const Note& note, QString authenticationToken = QString());

    /** Asynchronous version of @link createNote @endlink */
    AsyncResult* createNoteAsync(const Note& note, QString authenticationToken = QString());

    /**
       * Submit a set of changes to a note to the service.  The provided data
       * must include the note's guid field for identification. The note's
       * title must also be set.
       *
       * @param note
       *   A Note object containing the desired fields to be populated on
       *   the service. With the exception of the note's title and guid, fields
       *   that are not being changed do not need to be set. If the content is not
       *   being modified, note.content should be left unset. If the list of
       *   resources is not being modified, note.resources should be left unset.
       *
       * @return
       *   The metadata (no contents) for the Note on the server after the update
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.title" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "Note.content" - invalid length for ENML body
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteAttributes.*" - bad resource string
       *   </li>
       *   <li> BAD_DATA_FORMAT "ResourceAttributes.*" - bad resource string
       *   </li>
       *   <li> BAD_DATA_FORMAT "Resource.mime" - invalid resource MIME type
       *   </li>
       *   <li> DATA_CONFLICT "Note.deleted" - deleted time set on active note
       *   </li>
       *   <li> DATA_REQUIRED "Resource.data" - resource data body missing
       *   </li>
       *   <li> ENML_VALIDATION "*" - note content doesn't validate against DTD
       *   </li>
       *   <li> LIMIT_REACHED "Note.tagGuids" - too many Tags on Note
       *   </li>
       *   <li> LIMIT_REACHED "Note.resources" - too many resources on Note
       *   </li>
       *   <li> LIMIT_REACHED "Note.size" - total note size too large
       *   </li>
       *   <li> LIMIT_REACHED "Resource.data.size" - resource too large
       *   </li>
       *   <li> LIMIT_REACHED "NoteAttribute.*" - attribute string too long
       *   </li>
       *   <li> LIMIT_REACHED "ResourceAttribute.*" - attribute string too long
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - user doesn't own
       *   </li>
       *   <li> PERMISSION_DENIED "Note.notebookGuid" - user doesn't own destination
       *   </li>
       *   <li> QUOTA_REACHED "Accounting.uploadLimit" - note exceeds upload quota
       *   </li>
       *   <li> BAD_DATA_FORMAT "Tag.name" - Note.tagNames was provided, and one
       *     of the specified tags had an invalid length or pattern
       *   </li>
       *   <li> LIMIT_REACHED "Tag" - Note.tagNames was provided, and the required
       *     new tags would exceed the maximum number per account
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - note not found, by GUID
       *   </li>
       *   <li> "Note.notebookGuid" - if notebookGuid provided, but not found
       *   </li>
       * </ul>
       */
    Note updateNote(const Note& note, QString authenticationToken = QString());

    /** Asynchronous version of @link updateNote @endlink */
    AsyncResult* updateNoteAsync(const Note& note, QString authenticationToken = QString());

    /**
       * Moves the note into the trash. The note may still be undeleted, unless it
       * is expunged.  This is equivalent to calling updateNote() after setting
       * Note.active = false
       *
       * @param guid
       *   The GUID of the note to delete.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> PERMISSION_DENIED "Note" - user doesn't have permission to
       *          update the note.
       *   </li>
       * </ul>
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_CONFLICT "Note.guid" - the note is already deleted
       *   </li>
       * </ul>
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 deleteNote(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link deleteNote @endlink */
    AsyncResult* deleteNoteAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Permanently removes a Note, and all of its Resources,
       * from the service.
       * <p/>
       * NOTE: This function is not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param guid
       *   The GUID of the note to delete.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> PERMISSION_DENIED "Note" - user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 expungeNote(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link expungeNote @endlink */
    AsyncResult* expungeNoteAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Permanently removes a list of Notes, and all of their Resources, from
       * the service.  This should be invoked with a small number of Note GUIDs
       * (e.g. 100 or less) on each call.  To expunge a larger number of notes,
       * call this method multiple times.  This should also be used to reduce the
       * number of Notes in a notebook before calling expungeNotebook() or
       * in the trash before calling expungeInactiveNotes(), since these calls may
       * be prohibitively slow if there are more than a few hundred notes.
       * If an exception is thrown for any of the GUIDs, then none of the notes
       * will be deleted.  I.e. this call can be treated as an atomic transaction.
       * <p/>
       * NOTE: This function is not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param noteGuids
       *   The list of GUIDs for the Notes to remove.
       *
       * @return
       *   The account's updateCount at the end of this operation
       *
       * @throws EDAMUserException <ul>
       *   <li> PERMISSION_DENIED "Note" - user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 expungeNotes(QList< Guid > noteGuids, QString authenticationToken = QString());

    /** Asynchronous version of @link expungeNotes @endlink */
    AsyncResult* expungeNotesAsync(QList< Guid > noteGuids, QString authenticationToken = QString());

    /**
       * Permanently removes all of the Notes that are currently marked as
       * inactive.  This is equivalent to "emptying the trash", and these Notes
       * will be gone permanently.
       * <p/>
       * This operation may be relatively slow if the account contains a large
       * number of inactive Notes.
       * <p/>
       * NOTE: This function is not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @return
       *    The number of notes that were expunged.
       */
    qint32 expungeInactiveNotes(QString authenticationToken = QString());

    /** Asynchronous version of @link expungeInactiveNotes @endlink */
    AsyncResult* expungeInactiveNotesAsync(QString authenticationToken = QString());

    /**
       * Performs a deep copy of the Note with the provided GUID 'noteGuid' into
       * the Notebook with the provided GUID 'toNotebookGuid'.
       * The caller must be the owner of both the Note and the Notebook.
       * This creates a new Note in the destination Notebook with new content and
       * Resources that match all of the content and Resources from the original
       * Note, but with new GUID identifiers.
       * The original Note is not modified by this operation.
       * The copied note is considered as an "upload" for the purpose of upload
       * transfer limit calculation, so its size is added to the upload count for
       * the owner.
       *
       * @param noteGuid
       *   The GUID of the Note to copy.
       *
       * @param toNotebookGuid
       *   The GUID of the Notebook that should receive the new Note.
       *
       * @return
       *   The metadata for the new Note that was created.  This will include the
       *   new GUID for this Note (and any copied Resources), but will not include
       *   the content body or the binary bodies of any Resources.
       *
       * @throws EDAMUserException <ul>
       *   <li> LIMIT_REACHED "Note" - at max number per account
       *   </li>
       *   <li> PERMISSION_DENIED "Notebook.guid" - destination not owned by user
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - user doesn't own
       *   </li>
       *   <li> QUOTA_REACHED "Accounting.uploadLimit" - note exceeds upload quota
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    Note copyNote(Guid noteGuid, Guid toNotebookGuid, QString authenticationToken = QString());

    /** Asynchronous version of @link copyNote @endlink */
    AsyncResult* copyNoteAsync(Guid noteGuid, Guid toNotebookGuid, QString authenticationToken = QString());

    /**
       * Returns a list of the prior versions of a particular note that are
       * saved within the service.  These prior versions are stored to provide a
       * recovery from unintentional removal of content from a note. The identifiers
       * that are returned by this call can be used with getNoteVersion to retrieve
       * the previous note.
       * The identifiers will be listed from the most recent versions to the oldest.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    QList< NoteVersionId > listNoteVersions(Guid noteGuid, QString authenticationToken = QString());

    /** Asynchronous version of @link listNoteVersions @endlink */
    AsyncResult* listNoteVersionsAsync(Guid noteGuid, QString authenticationToken = QString());

    /**
       * This can be used to retrieve a previous version of a Note after it has been
       * updated within the service.  The caller must identify the note (via its
       * guid) and the version (via the updateSequenceNumber of that version).
       * to find a listing of the stored version USNs for a note, call
       * listNoteVersions.
       * This call is only available for notes in Premium accounts.  (I.e. access
       * to past versions of Notes is a Premium-only feature.)
       *
       * @param noteGuid
       *   The GUID of the note to be retrieved.
       *
       * @param updateSequenceNum
       *   The USN of the version of the note that is being retrieved
       *
       * @param withResourcesData
       *   If true, any Resource elements in this Note will include the binary
       *   contents of their 'data' field's body.
       *
       * @param withResourcesRecognition
       *   If true, any Resource elements will include the binary contents of the
       *   'recognition' field's body if recognition data is present.
       *
       * @param withResourcesAlternateData
       *   If true, any Resource elements in this Note will include the binary
       *   contents of their 'alternateData' fields' body, if an alternate form
       *   is present.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       *   <li> PERMISSION_DENIED "updateSequenceNum" -
       *     The account isn't permitted to access previous versions of notes.
       *     (i.e. this is a Free account.)
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       *   <li> "Note.updateSequenceNumber" - the Note doesn't have a version with
       *      the corresponding USN.
       *   </li>
       * </ul>
       */
    Note getNoteVersion(Guid noteGuid, qint32 updateSequenceNum, bool withResourcesData, bool withResourcesRecognition, bool withResourcesAlternateData, QString authenticationToken = QString());

    /** Asynchronous version of @link getNoteVersion @endlink */
    AsyncResult* getNoteVersionAsync(Guid noteGuid, qint32 updateSequenceNum, bool withResourcesData, bool withResourcesRecognition, bool withResourcesAlternateData, QString authenticationToken = QString());

    /**
       * Returns the current state of the resource in the service with the
       * provided GUID.
       * If the Resource is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).  Only the
       * keys for the applicationData will be returned.
       *
       * @param guid
       *   The GUID of the resource to be retrieved.
       *
       * @param withData
       *   If true, the Resource will include the binary contents of the
       *   'data' field's body.
       *
       * @param withRecognition
       *   If true, the Resource will include the binary contents of the
       *   'recognition' field's body if recognition data is present.
       *
       * @param withAttributes
       *   If true, the Resource will include the attributes
       *
       * @param withAlternateData
       *   If true, the Resource will include the binary contents of the
       *   'alternateData' field's body, if an alternate form is present.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Resource.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    Resource getResource(Guid guid, bool withData, bool withRecognition, bool withAttributes, bool withAlternateData, QString authenticationToken = QString());

    /** Asynchronous version of @link getResource @endlink */
    AsyncResult* getResourceAsync(Guid guid, bool withData, bool withRecognition, bool withAttributes, bool withAlternateData, QString authenticationToken = QString());

    /**
       * Get all of the application data for the Resource identified by GUID,
       * with values returned within the LazyMap fullMap field.
       * If there are no applicationData entries, then a LazyMap
       * with an empty fullMap will be returned. If your application
       * only needs to fetch its own applicationData entry, use
       * getResourceApplicationDataEntry instead.
       */
    LazyMap getResourceApplicationData(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceApplicationData @endlink */
    AsyncResult* getResourceApplicationDataAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Get the value of a single entry in the applicationData map
       * for the Resource identified by GUID.
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - Resource not found, by GUID</li>
       *   <li> "ResourceAttributes.applicationData.key" - Resource not found, by key</li>
       * </ul>
       */
    QString getResourceApplicationDataEntry(Guid guid, QString key, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceApplicationDataEntry @endlink */
    AsyncResult* getResourceApplicationDataEntryAsync(Guid guid, QString key, QString authenticationToken = QString());

    /**
       * Update, or create, an entry in the applicationData map for
       * the Resource identified by guid.
       */
    qint32 setResourceApplicationDataEntry(Guid guid, QString key, QString value, QString authenticationToken = QString());

    /** Asynchronous version of @link setResourceApplicationDataEntry @endlink */
    AsyncResult* setResourceApplicationDataEntryAsync(Guid guid, QString key, QString value, QString authenticationToken = QString());

    /**
       * Remove an entry identified by 'key' from the applicationData map for
       * the Resource identified by 'guid'.
       */
    qint32 unsetResourceApplicationDataEntry(Guid guid, QString key, QString authenticationToken = QString());

    /** Asynchronous version of @link unsetResourceApplicationDataEntry @endlink */
    AsyncResult* unsetResourceApplicationDataEntryAsync(Guid guid, QString key, QString authenticationToken = QString());

    /**
       * Submit a set of changes to a resource to the service.  This can be used
       * to update the meta-data about the resource, but cannot be used to change
       * the binary contents of the resource (including the length and hash).  These
       * cannot be changed directly without creating a new resource and removing the
       * old one via updateNote.
       *
       * @param resource
       *   A Resource object containing the desired fields to be populated on
       *   the service.  The service will attempt to update the resource with the
       *   following fields from the client:
       *   <ul>
       *      <li>guid:  must be provided to identify the resource
       *      </li>
       *      <li>mime
       *      </li>
       *      <li>width
       *      </li>
       *      <li>height
       *      </li>
       *      <li>duration
       *      </li>
       *      <li>attributes:  optional.  if present, the set of attributes will
       *           be replaced.
       *      </li>
       *   </ul>
       *
       * @return
       *   The Update Sequence Number of the resource after the changes have been
       *   applied.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Resource.guid" - if the parameter is missing
       *   </li>
       *   <li> BAD_DATA_FORMAT "Resource.mime" - invalid resource MIME type
       *   </li>
       *   <li> BAD_DATA_FORMAT "ResourceAttributes.*" - bad resource string
       *   </li>
       *   <li> LIMIT_REACHED "ResourceAttribute.*" - attribute string too long
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    qint32 updateResource(const Resource& resource, QString authenticationToken = QString());

    /** Asynchronous version of @link updateResource @endlink */
    AsyncResult* updateResourceAsync(const Resource& resource, QString authenticationToken = QString());

    /**
       * Returns binary data of the resource with the provided GUID.  For
       * example, if this were an image resource, this would contain the
       * raw bits of the image.
       * If the Resource is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).
       *
       * @param guid
       *   The GUID of the resource to be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Resource.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    QByteArray getResourceData(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceData @endlink */
    AsyncResult* getResourceDataAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Returns the current state of a resource, referenced by containing
       * note GUID and resource content hash.
       *
       * @param noteGuid
       *   The GUID of the note that holds the resource to be retrieved.
       *
       * @param contentHash
       *   The MD5 checksum of the resource within that note. Note that
       *   this is the binary checksum, for example from Resource.data.bodyHash,
       *   and not the hex-encoded checksum that is used within an en-media
       *   tag in a note body.
       *
       * @param withData
       *   If true, the Resource will include the binary contents of the
       *   'data' field's body.
       *
       * @param withRecognition
       *   If true, the Resource will include the binary contents of the
       *   'recognition' field's body.
       *
       * @param withAlternateData
       *   If true, the Resource will include the binary contents of the
       *   'alternateData' field's body, if an alternate form is present.
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_REQUIRED "Note.guid" - noteGuid param missing
       *   </li>
       *   <li> DATA_REQUIRED "Note.contentHash" - contentHash param missing
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note" - not found, by guid
       *   </li>
       *   <li> "Resource" - not found, by hash
       *   </li>
       * </ul>
       */
    Resource getResourceByHash(Guid noteGuid, QByteArray contentHash, bool withData, bool withRecognition, bool withAlternateData, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceByHash @endlink */
    AsyncResult* getResourceByHashAsync(Guid noteGuid, QByteArray contentHash, bool withData, bool withRecognition, bool withAlternateData, QString authenticationToken = QString());

    /**
       * Returns the binary contents of the recognition index for the resource
       * with the provided GUID.  If the caller asks about a resource that has
       * no recognition data, this will throw EDAMNotFoundException.
       * If the Resource is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).
       *
       * @param guid
       *   The GUID of the resource whose recognition data should be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Resource.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - not found, by GUID
       *   </li>
       *   <li> "Resource.recognition" - resource has no recognition
       *   </li>
       * </ul>
       */
    QByteArray getResourceRecognition(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceRecognition @endlink */
    AsyncResult* getResourceRecognitionAsync(Guid guid, QString authenticationToken = QString());

    /**
       * If the Resource with the provided GUID has an alternate data representation
       * (indicated via the Resource.alternateData field), then this request can
       * be used to retrieve the binary contents of that alternate data file.
       * If the caller asks about a resource that has no alternate data form, this
       * will throw EDAMNotFoundException.
       *
       * @param guid
       *    The GUID of the resource whose recognition data should be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Resource.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - not found, by GUID
       *   </li>
       *   <li> "Resource.alternateData" - resource has no recognition
       *   </li>
       * </ul>
       */
    QByteArray getResourceAlternateData(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceAlternateData @endlink */
    AsyncResult* getResourceAlternateDataAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Returns the set of attributes for the Resource with the provided GUID.
       * If the Resource is found in a public notebook, the authenticationToken
       * will be ignored (so it could be an empty string).
       *
       * @param guid
       *   The GUID of the resource whose attributes should be retrieved.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Resource.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Resource" - private resource, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Resource.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    ResourceAttributes getResourceAttributes(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link getResourceAttributes @endlink */
    AsyncResult* getResourceAttributesAsync(Guid guid, QString authenticationToken = QString());

    /**
       * <p>
       * Looks for a user account with the provided userId on this NoteStore
       * shard and determines whether that account contains a public notebook
       * with the given URI.  If the account is not found, or no public notebook
       * exists with this URI, this will throw an EDAMNotFoundException,
       * otherwise this will return the information for that Notebook.
       * </p>
       * <p>
       * If a notebook is visible on the web with a full URL like
       * http://www.evernote.com/pub/sethdemo/api
       * Then 'sethdemo' is the username that can be used to look up the userId,
       * and 'api' is the publicUri.
       * </p>
       *
       * @param userId
       *    The numeric identifier for the user who owns the public notebook.
       *    To find this value based on a username string, you can invoke
       *    UserStore.getPublicUserInfo
       *
       * @param publicUri
       *    The uri string for the public notebook, from Notebook.publishing.uri.
       *
       * @throws EDAMNotFoundException <ul>
       *   <li>"Publishing.uri" - not found, by URI</li>
       * </ul>
       *
       * @throws EDAMSystemException <ul>
       *   <li> TAKEN_DOWN "PublicNotebook" - The specified public notebook is
       *     taken down (for all requesters).</li>
       *   <li> TAKEN_DOWN "Country" - The specified public notebook is taken
       *     down for the requester because of an IP-based country lookup.</li>
       * </ul>
       */
    Notebook getPublicNotebook(UserID userId, QString publicUri);

    /** Asynchronous version of @link getPublicNotebook @endlink */
    AsyncResult* getPublicNotebookAsync(UserID userId, QString publicUri);

    /**
       * Used to construct a shared notebook object. The constructed notebook will
       * contain a "share key" which serve as a unique identifer and access token
       * for a user to access the notebook of the shared notebook owner.
       *
       * @param sharedNotebook
       *   A shared notebook object populated with the email address of the share
       *   recipient, the notebook guid and the access permissions. All other
       *   attributes of the shared object are ignored. The SharedNotebook.allowPreview
       *   field must be explicitly set with either a true or false value.
       *
       * @return
       *   The fully populated SharedNotebook object including the server assigned
       *   share id and shareKey which can both be used to uniquely identify the
       *   SharedNotebook.
       *
       * @throws EDAMUserException <ul>
       *   <li>BAD_DATA_FORMAT "SharedNotebook.email" - if the email was not valid</li>
       *   <li>BAD_DATA_FORMAT "requireLogin" - if the SharedNotebook.allowPreview field was
       *       not set, and the SharedNotebook.requireLogin was also not set or was set to 
       *       false.</li>
       *   <li>PERMISSION_DENIED "SharedNotebook.recipientSettings" - if
       *       recipientSettings is set in the sharedNotebook.  Only the recipient
       *       can set these values via the setSharedNotebookRecipientSettings
       *       method.
       *   </li>
       *   </ul>
       * @throws EDAMNotFoundException <ul>
       *   <li>Notebook.guid - if the notebookGuid is not a valid GUID for the user.
       *   </li>
       *   </ul>
       */
    SharedNotebook createSharedNotebook(const SharedNotebook& sharedNotebook, QString authenticationToken = QString());

    /** Asynchronous version of @link createSharedNotebook @endlink */
    AsyncResult* createSharedNotebookAsync(const SharedNotebook& sharedNotebook, QString authenticationToken = QString());

    /**
       * Update a SharedNotebook object.
       *
       * @param authenticationToken
       *   Must be an authentication token from the owner or a shared notebook
       *   authentication token or business authentication token with sufficient
       *   permissions to change invitations for a notebook.
       *
       * @param sharedNotebook
       *  The SharedNotebook object containing the requested changes.
       *  The "id" of the shared notebook must be set to allow the service
       *  to identify the SharedNotebook to be updated. In addition, you MUST set
       *  the email, permission, and allowPreview fields to the desired values.
       *  All other fields will be ignored if set.
       *
       * @return
       *  The Update Serial Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li>UNSUPPORTED_OPERATION "updateSharedNotebook" - if this service instance does not support shared notebooks.</li>
       *   <li>BAD_DATA_FORMAT "SharedNotebook.email" - if the email was not valid.</li>
       *   <li>DATA_REQUIRED "SharedNotebook.id" - if the id field was not set.</li>
       *   <li>DATA_REQUIRED "SharedNotebook.privilege" - if the privilege field was not set.</li>
       *   <li>DATA_REQUIRED "SharedNotebook.allowPreview" - if the allowPreview field was not set.</li>
       *   </ul>
       * @throws EDAMNotFoundException <ul>
       *   <li>SharedNotebook.id - if no shared notebook with the specified ID was found.
       *   </ul>
       */
    qint32 updateSharedNotebook(const SharedNotebook& sharedNotebook, QString authenticationToken = QString());

    /** Asynchronous version of @link updateSharedNotebook @endlink */
    AsyncResult* updateSharedNotebookAsync(const SharedNotebook& sharedNotebook, QString authenticationToken = QString());

    /**
       * Set values for the recipient settings associated with a shared notebook.  Having
       * update rights to the shared notebook record itself has no effect on this call;
       * only the recipient of the shared notebook can can the recipient settings.
       *
       * If you do <i>not</i> wish to, or cannot, change one of the reminderNotifyEmail or
       * reminderNotifyInApp fields, you must leave that field unset in recipientSettings.
       * This method will skip that field for updates and leave the existing state as
       * it is.
       *
       * @return The update sequence number of the account to which the shared notebook
       *   belongs, which is the account from which we are sharing a notebook.
       *
       * @throws EDAMNotFoundException "sharedNotebookId" - Thrown if the service does not
       *   have a shared notebook record for the sharedNotebookId on the given shard.  If you
       *   receive this exception, it is probable that the shared notebook record has
       *   been revoked or expired, or that you accessed the wrong shard.
       *
       * @throws EDAMUserException <ul>
       *   <li>PEMISSION_DENIED "authenticationToken" - If you do not have permission to set
       *       the recipient settings for the shared notebook.  Only the recipient has
       *       permission to do this.
       *   <li>DATA_CONFLICT "recipientSettings.reminderNotifyEmail" - Setting whether
       *       or not you want to receive reminder e-mail notifications is possible on
       *       a business notebook in the business to which the user belongs.  All
       *       others can safely unset the reminderNotifyEmail field from the
       *       recipientSettings parameter.
       * </ul>
       */
    qint32 setSharedNotebookRecipientSettings(qint64 sharedNotebookId, const SharedNotebookRecipientSettings& recipientSettings, QString authenticationToken = QString());

    /** Asynchronous version of @link setSharedNotebookRecipientSettings @endlink */
    AsyncResult* setSharedNotebookRecipientSettingsAsync(qint64 sharedNotebookId, const SharedNotebookRecipientSettings& recipientSettings, QString authenticationToken = QString());

    /**
       * Send a reminder message to some or all of the email addresses that a notebook has been
       * shared with. The message includes the current link to view the notebook.
       * @param authenticationToken
       *   The auth token of the user with permissions to share the notebook
       * @param notebookGuid
       *   The guid of the shared notebook
       * @param messageText
       *  User provided text to include in the email
       * @param recipients
       *  The email addresses of the recipients. If this list is empty then all of the
       *  users that the notebook has been shared with are emailed.
       *  If an email address doesn't correspond to share invite members then that address
       *  is ignored.
       * @return
       *  The number of messages sent
       * @throws EDAMUserException <ul>
       *   <li> LIMIT_REACHED "(recipients)" -
       *     The email can't be sent because this would exceed the user's daily
       *     email limit.
       *   </li>
       *   <li> PERMISSION_DENIED "Notebook.guid" - The user doesn't have permission to
       *     send a message for the specified notebook.
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Notebook.guid" - not found, by GUID
       *   </li>
       * </ul>
        */
    qint32 sendMessageToSharedNotebookMembers(Guid notebookGuid, QString messageText, QStringList recipients, QString authenticationToken = QString());

    /** Asynchronous version of @link sendMessageToSharedNotebookMembers @endlink */
    AsyncResult* sendMessageToSharedNotebookMembersAsync(Guid notebookGuid, QString messageText, QStringList recipients, QString authenticationToken = QString());

    /**
       * Lists the collection of shared notebooks for all notebooks in the
       * users account.
       *
       * @return
       *  The list of all SharedNotebooks for the user
       */
    QList< SharedNotebook > listSharedNotebooks(QString authenticationToken = QString());

    /** Asynchronous version of @link listSharedNotebooks @endlink */
    AsyncResult* listSharedNotebooksAsync(QString authenticationToken = QString());

    /**
       * Expunges the SharedNotebooks in the user's account using the
       * SharedNotebook.id as the identifier.
       * <p/>
       * NOTE: This function is generally not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param
       *   sharedNotebookIds - a list of ShardNotebook.id longs identifying the
       *       objects to delete permanently.
       *
       * @return
       *   The account's update sequence number.
       */
    qint32 expungeSharedNotebooks(QList< qint64 > sharedNotebookIds, QString authenticationToken = QString());

    /** Asynchronous version of @link expungeSharedNotebooks @endlink */
    AsyncResult* expungeSharedNotebooksAsync(QList< qint64 > sharedNotebookIds, QString authenticationToken = QString());

    /**
       * Asks the service to make a linked notebook with the provided name, username
       * of the owner and identifiers provided. A linked notebook can be either a
       * link to a public notebook or to a private shared notebook.
       *
       * @param linkedNotebook
       *   The desired fields for the linked notebook must be provided on this
       *   object.  The name of the linked notebook must be set. Either a username
       *   uri or a shard id and share key must be provided otherwise a
       *   EDAMUserException is thrown.
       *
       * @return
       *   The newly created LinkedNotebook.  The server-side id will be
       *   saved in this object's 'id' field.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "LinkedNotebook.name" - invalid length or pattern
       *   </li>
       *   <li> BAD_DATA_FORMAT "LinkedNotebook.username" - bad username format
       *   </li>
       *   <li> BAD_DATA_FORMAT "LinkedNotebook.uri" -
       *     if public notebook set but bad uri
       *   </li>
       *   <li> BAD_DATA_FORMAT "LinkedNotebook.shareKey" -
       *     if private notebook set but bad shareKey
       *   </li>
       *   <li> DATA_REQUIRED "LinkedNotebook.shardId" -
       *     if private notebook but shard id not provided
       *   </li>
       * </ul>
       */
    LinkedNotebook createLinkedNotebook(const LinkedNotebook& linkedNotebook, QString authenticationToken = QString());

    /** Asynchronous version of @link createLinkedNotebook @endlink */
    AsyncResult* createLinkedNotebookAsync(const LinkedNotebook& linkedNotebook, QString authenticationToken = QString());

    /**
       * @param linkedNotebook
       *   Updates the name of a linked notebook.
       *
       * @return
       *   The Update Sequence Number for this change within the account.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "LinkedNotebook.name" - invalid length or pattern
       *   </li>
       * </ul>
       */
    qint32 updateLinkedNotebook(const LinkedNotebook& linkedNotebook, QString authenticationToken = QString());

    /** Asynchronous version of @link updateLinkedNotebook @endlink */
    AsyncResult* updateLinkedNotebookAsync(const LinkedNotebook& linkedNotebook, QString authenticationToken = QString());

    /**
       * Returns a list of linked notebooks
       */
    QList< LinkedNotebook > listLinkedNotebooks(QString authenticationToken = QString());

    /** Asynchronous version of @link listLinkedNotebooks @endlink */
    AsyncResult* listLinkedNotebooksAsync(QString authenticationToken = QString());

    /**
       * Permanently expunges the linked notebook from the account.
       * <p/>
       * NOTE: This function is generally not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param guid
       *   The LinkedNotebook.guid field of the LinkedNotebook to permanently remove
       *   from the account.
       */
    qint32 expungeLinkedNotebook(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link expungeLinkedNotebook @endlink */
    AsyncResult* expungeLinkedNotebookAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Asks the service to produce an authentication token that can be used to
       * access the contents of a shared notebook from someone else's account.
       * This authenticationToken can be used with the various other NoteStore
       * calls to find and retrieve notes, and if the permissions in the shared
       * notebook are sufficient, to make changes to the contents of the notebook.
       *
       * @param shareKey
       *   The 'shareKey' identifier from the SharedNotebook that was granted to
       *   some recipient.  This string internally encodes the notebook identifier
       *   and a security signature.
       *
       * @param authenticationToken
       *   If a non-empty string is provided, this is the full user-based
       *   authentication token that identifies the user who is currently logged in
       *   and trying to access the shared notebook.  This may be required if the
       *   notebook was created with 'requireLogin'.
       *   If this string is empty, the service will attempt to authenticate to the
       *   shared notebook without any logged in user.
       *
       * @throws EDAMSystemException <ul>
       *   <li> BAD_DATA_FORMAT "shareKey" - invalid shareKey string
       *   </li>
       *   <li> INVALID_AUTH "shareKey" - bad signature on shareKey string
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "SharedNotebook.id" - the shared notebook no longer exists
       *   </li>
       * </ul>
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_REQUIRED "authenticationToken" - the share requires login, and
       *          no valid authentication token was provided.
       *   </li>
       *   <li> PERMISSION_DENIED "SharedNotebook.username" - share requires login,
       *          and another username has already been bound to this notebook.
       *   </li>
       * </ul>
       */
    AuthenticationResult authenticateToSharedNotebook(QString shareKey, QString authenticationToken = QString());

    /** Asynchronous version of @link authenticateToSharedNotebook @endlink */
    AsyncResult* authenticateToSharedNotebookAsync(QString shareKey, QString authenticationToken = QString());

    /**
       * This function is used to retrieve extended information about a shared
       * notebook by a guest who has already authenticated to access that notebook.
       * This requires an 'authenticationToken' parameter which should be the
       * resut of a call to authenticateToSharedNotebook(...).
       * I.e. this is the token that gives access to the particular shared notebook
       * in someone else's account -- it's not the authenticationToken for the
       * owner of the notebook itself.
       *
       * @param authenticationToken
       *   Should be the authentication token retrieved from the reply of
       *   authenticateToSharedNotebook(), proving access to a particular shared
       *   notebook.
       *
       * @throws EDAMUserException <ul>
       *   <li> PERMISSION_DENIED "authenticationToken" -
       *          authentication token doesn't correspond to a valid shared notebook
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "SharedNotebook.id" - the shared notebook no longer exists
       *   </li>
       * </ul>
       */
    SharedNotebook getSharedNotebookByAuth(QString authenticationToken = QString());

    /** Asynchronous version of @link getSharedNotebookByAuth @endlink */
    AsyncResult* getSharedNotebookByAuthAsync(QString authenticationToken = QString());

    /**
       * Attempts to send a single note to one or more email recipients.
       * <p/>
       * NOTE: This function is generally not available to third party applications.
       * Calls will result in an EDAMUserException with the error code
       * PERMISSION_DENIED.
       *
       * @param authenticationToken
       *    The note will be sent as the user logged in via this token, using that
       *    user's registered email address.  If the authenticated user doesn't
       *    have permission to read that note, the emailing will fail.
       *
       * @param parameters
       *    The note must be specified either by GUID (in which case it will be
       *    sent using the existing data in the service), or else the full Note
       *    must be passed to this call.  This also specifies the additional
       *    email fields that will be used in the email.
       *
       * @throws EDAMUserException <ul>
       *   <li> LIMIT_REACHED "NoteEmailParameters.toAddresses" -
       *     The email can't be sent because this would exceed the user's daily
       *     email limit.
       *   </li>
       *   <li> BAD_DATA_FORMAT "(email address)" -
       *     email address malformed
       *   </li>
       *   <li> DATA_REQUIRED "NoteEmailParameters.toAddresses" -
       *     if there are no To: or Cc: addresses provided.
       *   </li>
       *   <li> DATA_REQUIRED "Note.title" -
       *     if the caller provides a Note parameter with no title
       *   </li>
       *   <li> DATA_REQUIRED "Note.content" -
       *     if the caller provides a Note parameter with no content
       *   </li>
       *   <li> ENML_VALIDATION "*" - note content doesn't validate against DTD
       *   </li>
       *   <li> DATA_REQUIRED "NoteEmailParameters.note" -
       *     if no guid or note provided
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    void emailNote(const NoteEmailParameters& parameters, QString authenticationToken = QString());

    /** Asynchronous version of @link emailNote @endlink */
    AsyncResult* emailNoteAsync(const NoteEmailParameters& parameters, QString authenticationToken = QString());

    /**
       * If this note is not already shared (via its own direct URL), then this
       * will start sharing that note.
       * This will return the secret "Note Key" for this note that
       * can currently be used in conjunction with the Note's GUID to gain direct
       * read-only access to the Note.
       * If the note is already shared, then this won't make any changes to the
       * note, and the existing "Note Key" will be returned.  The only way to change
       * the Note Key for an existing note is to stopSharingNote first, and then
       * call this function.
       *
       * @param guid
       *   The GUID of the note to be shared.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    QString shareNote(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link shareNote @endlink */
    AsyncResult* shareNoteAsync(Guid guid, QString authenticationToken = QString());

    /**
       * If this note is not already shared then this will stop sharing that note
       * and invalidate its "Note Key", so any existing URLs to access that Note
       * will stop working.
       * If the Note is not shared, then this function will do nothing.
       *
       * @param guid
       *   The GUID of the note to be un-shared.
       *
       * @throws EDAMUserException <ul>
       *   <li> BAD_DATA_FORMAT "Note.guid" - if the parameter is missing
       *   </li>
       *   <li> PERMISSION_DENIED "Note" - private note, user doesn't own
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "Note.guid" - not found, by GUID
       *   </li>
       * </ul>
       */
    void stopSharingNote(Guid guid, QString authenticationToken = QString());

    /** Asynchronous version of @link stopSharingNote @endlink */
    AsyncResult* stopSharingNoteAsync(Guid guid, QString authenticationToken = QString());

    /**
       * Asks the service to produce an authentication token that can be used to
       * access the contents of a single Note which was individually shared
       * from someone's account.
       * This authenticationToken can be used with the various other NoteStore
       * calls to find and retrieve the Note and its directly-referenced children.
       *
       * @param guid
       *   The GUID identifying this Note on this shard.
       *
       * @param noteKey
       *   The 'noteKey' identifier from the Note that was originally created via
       *   a call to shareNote() and then given to a recipient to access.
       *
       * @param authenticationToken
       *   An optional authenticationToken that identifies the user accessing the
       *   shared note. This parameter may be required to access some shared notes.
       *
       * @throws EDAMUserException <ul>
       *   <li> PERMISSION_DENIED "Note" - the Note with that GUID is either not
       *     shared, or the noteKey doesn't match the current key for this note
       *   </li>
       *   <li> PERMISSION_DENIED "authenticationToken" - an authentication token is
       *     required to access this Note, but either no authentication token or a
       *     "non-owner" authentication token was provided.
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li> "guid" - the note with that GUID is not found
       *   </li>
       * </ul>
       *
       * @throws EDAMSystemException <ul>
       *   <li> TAKEN_DOWN "Note" - The specified shared note is taken down (for
       *     all requesters).
       *   </li>
       *   <li> TAKEN_DOWN "Country" - The specified shared note is taken down
       *     for the requester because of an IP-based country lookup.
       *   </ul>
       * </ul>
       */
    AuthenticationResult authenticateToSharedNote(QString guid, QString noteKey, QString authenticationToken = QString());

    /** Asynchronous version of @link authenticateToSharedNote @endlink */
    AsyncResult* authenticateToSharedNoteAsync(QString guid, QString noteKey, QString authenticationToken = QString());

    /**
       * Identify related entities on the service, such as notes,
       * notebooks, and tags related to notes or content.
       *
       * @param query
       *   The information about which we are finding related entities.
    
       * @param resultSpec
       *   Allows the client to indicate the type and quantity of
       *   information to be returned, allowing a saving of time and
       *   bandwidth.
       *
       * @return
       *   The result of the query, with information considered
       *   to likely be relevantly related to the information
       *   described by the query.
       *
       * @throws EDAMUserException <ul>
       *   <li>BAD_DATA_FORMAT "RelatedQuery.plainText" - If you provided a
       *     a zero-length plain text value.
       *   </li>
       *   <li>BAD_DATA_FORMAT "RelatedQuery.noteGuid" - If you provided an
       *     invalid Note GUID, that is, one that does not match the constraints
       *     defined by EDAM_GUID_LEN_MIN, EDAM_GUID_LEN_MAX, EDAM_GUID_REGEX.
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.notebookGuid" - if malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.tagGuids" - if any are malformed
       *   </li>
       *   <li> BAD_DATA_FORMAT "NoteFilter.words" - if search string too long
       *   </li>
       *   <li>PERMISSION_DENIED "Note" - If the caller does not have access to
       *     the note identified by RelatedQuery.noteGuid.
       *   </li>
       *   <li>DATA_REQUIRED "RelatedResultSpec" - If you did not not set any values
       *     in the result spec.
       *   </li>
       * </ul>
       *
       * @throws EDAMNotFoundException <ul>
       *   <li>"RelatedQuery.noteGuid" - the note with that GUID is not
       *     found, if that field has been set in the query.
       *   </li>
       * </ul>
       */
    RelatedResult findRelated(const RelatedQuery& query, const RelatedResultSpec& resultSpec, QString authenticationToken = QString());

    /** Asynchronous version of @link findRelated @endlink */
    AsyncResult* findRelatedAsync(const RelatedQuery& query, const RelatedResultSpec& resultSpec, QString authenticationToken = QString());

private:
    QString url_;
    QString authenticationToken_;
};

/**
 * Service:  UserStore
 * <p>
 * The UserStore service is primarily used by EDAM clients to establish
 * authentication via username and password over a trusted connection (e.g.
 * SSL).  A client's first call to this interface should be checkVersion() to
 * ensure that the client's software is up to date.
 * </p>
 * All calls which require an authenticationToken may throw an
 * EDAMUserException for the following reasons:
 *  <ul>
 *   <li> AUTH_EXPIRED "authenticationToken" - token has expired
 *   <li> BAD_DATA_FORMAT "authenticationToken" - token is malformed
 *   <li> DATA_REQUIRED "authenticationToken" - token is empty
 *   <li> INVALID_AUTH "authenticationToken" - token signature is invalid
 * </ul>
 */
class UserStore: public QObject {
    Q_DISABLE_COPY(UserStore)
public:
    explicit UserStore(QString host, QString authenticationToken = QString(), QObject *parent = 0);

    void setAuthenticationToken(QString authenticationToken) {authenticationToken_ = authenticationToken;}
    QString authenticationToken() {return authenticationToken_ ;}

    /**
       * This should be the first call made by a client to the EDAM service.  It
       * tells the service what protocol version is used by the client.  The
       * service will then return true if the client is capable of talking to
       * the service, and false if the client's protocol version is incompatible
       * with the service, so the client must upgrade.  If a client receives a
       * false value, it should report the incompatibility to the user and not
       * continue with any more EDAM requests (UserStore or NoteStore).
       *
       * @param clientName
       *   This string provides some information about the client for
       *   tracking/logging on the service.  It should provide information about
       *   the client's software and platform. The structure should be:
       *   application/version; platform/version; [ device/version ]
       *   E.g. "Evernote Windows/3.0.1; Windows/XP SP3".
       *
       * @param edamVersionMajor
       *   This should be the major protocol version that was compiled by the
       *   client.  This should be the current value of the EDAM_VERSION_MAJOR
       *   constant for the client.
       *
       * @param edamVersionMinor
       *   This should be the major protocol version that was compiled by the
       *   client.  This should be the current value of the EDAM_VERSION_MINOR
       *   constant for the client.
       */
    bool checkVersion(QString clientName, qint16 edamVersionMajor = EDAM_VERSION_MAJOR, qint16 edamVersionMinor = EDAM_VERSION_MINOR);

    /** Asynchronous version of @link checkVersion @endlink */
    AsyncResult* checkVersionAsync(QString clientName, qint16 edamVersionMajor = EDAM_VERSION_MAJOR, qint16 edamVersionMinor = EDAM_VERSION_MINOR);

    /**
       * This provides bootstrap information to the client. Various bootstrap
       * profiles and settings may be used by the client to configure itself.
       *
       * @param locale
       *   The client's current locale, expressed in language[_country]
       *   format. E.g., "en_US". See ISO-639 and ISO-3166 for valid
       *   language and country codes.
       *
       * @return
       *   The bootstrap information suitable for this client.
       */
    BootstrapInfo getBootstrapInfo(QString locale);

    /** Asynchronous version of @link getBootstrapInfo @endlink */
    AsyncResult* getBootstrapInfoAsync(QString locale);

    /**
       * This is used to check a username and password in order to create a
       * short-lived authentication session that can be used for further actions.
       *
       * This function is only available to Evernote's internal applications.
       * Third party applications must authenticate using OAuth as
       * described at
       * <a href="http://dev.evernote.com/documentation/cloud/">dev.evernote.com</a>.
       *
       * @param username
       *   The username (not numeric user ID) for the account to
       *   authenticate against.  This function will also accept the user's
       *   registered email address in this parameter.
       *
       * @param password
       *   The plaintext password to check against the account.  Since
       *   this is not protected by the EDAM protocol, this information must be
       *   provided over a protected transport (e.g. SSL).
       *
       * @param consumerKey
       *   The "consumer key" portion of the API key issued to the client application
       *   by Evernote.
       *
       * @param consumerSecret
       *   The "consumer secret" portion of the API key issued to the client application
       *   by Evernote.
       *
       * @param supportsTwoFactor
       *   Whether the calling application supports two-factor authentication. If this
       *   parameter is false, this method will fail with the error code INVALID_AUTH and the
       *   parameter "password" when called for a user who has enabled two-factor
       *   authentication.
       *
       * @return
       *   <p>The result of the authentication.  If the authentication was successful,
       *   the AuthenticationResult.user field will be set with the full information
       *   about the User.</p>
       *   <p>If the user has two-factor authentication enabled,
       *   AuthenticationResult.secondFactorRequired will be set and
       *   AuthenticationResult.authenticationToken will contain a short-lived token
       *   that may only be used to complete the two-factor authentication process by calling
       *   UserStore.completeTwoFactorAuthentication.</p>
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_REQUIRED "username" - username is empty
       *   <li> DATA_REQUIRED "password" - password is empty
       *   <li> DATA_REQUIRED "consumerKey" - consumerKey is empty
       *   <li> INVALID_AUTH "username" - username not found
       *   <li> INVALID_AUTH "password" - password did not match
       *   <li> INVALID_AUTH "consumerKey" - consumerKey is not authorized
       *   <li> INVALID_AUTH "consumerSecret" - consumerSecret is incorrect
       *   <li> PERMISSION_DENIED "User.active" - user account is closed
       *   <li> PERMISSION_DENIED "User.tooManyFailuresTryAgainLater" - user has
       *     failed authentication too often
       * </ul>
       */
    AuthenticationResult authenticate(QString username, QString password, QString consumerKey, QString consumerSecret, bool supportsTwoFactor);

    /** Asynchronous version of @link authenticate @endlink */
    AsyncResult* authenticateAsync(QString username, QString password, QString consumerKey, QString consumerSecret, bool supportsTwoFactor);

    /**
       * This is used to check a username and password in order to create a
       * long-lived authentication token that can be used for further actions.
       *
       * This function is not available to most third party applications,
       * which typically authenticate using OAuth as
       * described at
       * <a href="http://dev.evernote.com/documentation/cloud/">dev.evernote.com</a>.
       * If you believe that your application requires permission to authenticate
       * using username and password instead of OAuth, please contact Evernote
       * developer support by visiting 
       * <a href="http://dev.evernote.com">dev.evernote.com</a>.
       *
       * @param username
       *   The username or registered email address of the account to
       *   authenticate against.
       *
       * @param password
       *   The plaintext password to check against the account.  Since
       *   this is not protected by the EDAM protocol, this information must be
       *   provided over a protected transport (i.e. SSL).
       *
       * @param consumerKey
       *   The "consumer key" portion of the API key issued to the client application
       *   by Evernote.
       *
       * @param consumerSecret
       *   The "consumer secret" portion of the API key issued to the client application
       *   by Evernote.
       *
       * @param deviceIdentifier
       *   An optional string, no more than 32 characters in length, that uniquely identifies 
       *   the device from which the authentication is being performed. This string allows 
       *   the service to return the same authentication token when a given application 
       *   requests authentication repeatedly from the same device. This may happen when the 
       *   user logs out of an application and then logs back in, or when the application is 
       *   uninstalled and later reinstalled. If no reliable device identifier can be created, 
       *   this value should be omitted. If set, the device identifier must be between
       *   1 and EDAM_DEVICE_ID_LEN_MAX characters long and must match the regular expression
       *   EDAM_DEVICE_ID_REGEX.
       *
       * @param deviceDescription
       *   A description of the device from which the authentication is being performed.
       *   This field is displayed to the user in a list of authorized applications to
       *   allow them to distinguish between multiple tokens issued to the same client
       *   application on different devices. For example, the Evernote iOS client on
       *   a user's iPhone and iPad might pass the iOS device names "Bob's iPhone" and
       *   "Bob's iPad". The device description must be between 1 and 
       *   EDAM_DEVICE_DESCRIPTION_LEN_MAX characters long and must match the regular 
       *   expression EDAM_DEVICE_DESCRIPTION_REGEX.
       *
       * @param supportsTwoFactor
       *   Whether the calling application supports two-factor authentication. If this
       *   parameter is false, this method will fail with the error code INVALID_AUTH and the
       *   parameter "password" when called for a user who has enabled two-factor
       *   authentication.
       *
       * @return
       *   <p>The result of the authentication. The level of detail provided in the returned
       *   AuthenticationResult.User structure depends on the access level granted by 
       *   calling application's API key.</p>
       *   <p>If the user has two-factor authentication enabled,
       *   AuthenticationResult.secondFactorRequired will be set and
       *   AuthenticationResult.authenticationToken will contain a short-lived token
       *   that may only be used to complete the two-factor authentication process by calling
       *   UserStore.completeTwoFactorAuthentication.</p>
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_REQUIRED "username" - username is empty
       *   <li> DATA_REQUIRED "password" - password is empty
       *   <li> DATA_REQUIRED "consumerKey" - consumerKey is empty
       *   <li> DATA_REQUIRED "consumerSecret" - consumerSecret is empty
       *   <li> DATA_REQUIRED "deviceDescription" - deviceDescription is empty
       *   <li> BAD_DATA_FORMAT "deviceDescription" - deviceDescription is not valid.
       *   <li> BAD_DATA_FORMAT "deviceIdentifier" - deviceIdentifier is not valid.
       *   <li> INVALID_AUTH "username" - username not found
       *   <li> INVALID_AUTH "password" - password did not match
       *   <li> INVALID_AUTH "consumerKey" - consumerKey is not authorized
       *   <li> INVALID_AUTH "consumerSecret" - consumerSecret is incorrect
       *   <li> PERMISSION_DENIED "User.active" - user account is closed
       *   <li> PERMISSION_DENIED "User.tooManyFailuresTryAgainLater" - user has
       *     failed authentication too often
       * </ul>
       */
    AuthenticationResult authenticateLongSession(QString username, QString password, QString consumerKey, QString consumerSecret, QString deviceIdentifier, QString deviceDescription, bool supportsTwoFactor);

    /** Asynchronous version of @link authenticateLongSession @endlink */
    AsyncResult* authenticateLongSessionAsync(QString username, QString password, QString consumerKey, QString consumerSecret, QString deviceIdentifier, QString deviceDescription, bool supportsTwoFactor);

    /**
       * Complete the authentication process when a second factor is required. This
       * call is made after a successful call to authenticate or authenticateLongSession
       * when the authenticating user has enabled two-factor authentication.
       *
       * @param authenticationToken An authentication token returned by a previous
       *   call to UserStore.authenticate or UserStore.authenticateLongSession that
       *   could not be completed in a single call because a second factor was required.
       *
       * @param oneTimeCode The one time code entered by the user. This value is delivered
       *   out-of-band, typically via SMS or an authenticator application.
       *
       * @param deviceIdentifier See the corresponding parameter in authenticateLongSession.
       *
       * @param deviceDescription See the corresponding parameter in authenticateLongSession.
       *
       * @return
       *   The result of the authentication. The level of detail provided in the returned
       *   AuthenticationResult.User structure depends on the access level granted by the
       *   calling application's API key. If the initial authentication call was made to
       *   authenticateLongSession, the AuthenticationResult will contain a long-lived
       *   authentication token.
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_REQUIRED "authenticationToken" - authenticationToken is empty
       *   <li> DATA_REQUIRED "oneTimeCode" - oneTimeCode is empty
       *   <li> BAD_DATA_FORMAT "authenticationToken" - authenticationToken is not well formed
       *   <li> INVALID_AUTH "oneTimeCode" - oneTimeCode did not match
       *   <li> AUTH_EXPIRED "authenticationToken" - authenticationToken has expired
       *   <li> PERMISSION_DENIED "authenticationToken" - authenticationToken is not valid
       *   <li> PERMISSION_DENIED "User.active" - user account is closed
       *   <li> PERMISSION_DENIED "User.tooManyFailuresTryAgainLater" - user has
       *     failed authentication too often
       *   <li> DATA_CONFLICT "User.twoFactorAuthentication" - The user has not enabled
       *      two-factor authentication.</li>
       * </ul>
       */
    AuthenticationResult completeTwoFactorAuthentication(QString oneTimeCode, QString deviceIdentifier, QString deviceDescription, QString authenticationToken = QString());

    /** Asynchronous version of @link completeTwoFactorAuthentication @endlink */
    AsyncResult* completeTwoFactorAuthenticationAsync(QString oneTimeCode, QString deviceIdentifier, QString deviceDescription, QString authenticationToken = QString());

    /**
       * Revoke an existing long lived authentication token. This can be used to
       * revoke OAuth tokens or tokens created by calling authenticateLongSession,
       * and allows a user to effectively log out of Evernote from the perspective
       * of the application that holds the token. The authentication token that is
       * passed is immediately revoked and may not be used to call any authenticated
       * EDAM function. 
       *
       * @param authenticationToken the authentication token to revoke.
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_REQUIRED "authenticationToken" - no authentication token provided
       *   <li> BAD_DATA_FORMAT "authenticationToken" - the authentication token is not well formed
       *   <li> INVALID_AUTH "authenticationToken" - the authentication token is invalid
       *   <li> AUTH_EXPIRED "authenticationToken" - the authentication token is expired or 
       *     is already revoked.
       * </ul>
       */
    void revokeLongSession(QString authenticationToken = QString());

    /** Asynchronous version of @link revokeLongSession @endlink */
    AsyncResult* revokeLongSessionAsync(QString authenticationToken = QString());

    /**
       * This is used to take an existing authentication token that grants access
       * to an individual user account (returned from 'authenticate', 
       * 'authenticateLongSession' or an OAuth authorization) and obtain an additional 
       * authentication token that may be used to access business notebooks if the user
       * is a member of an Evernote Business account.
       *
       * The resulting authentication token may be used to make NoteStore API calls
       * against the business using the NoteStore URL returned in the result.
       *
       * @param authenticationToken 
       *   The authentication token for the user. This may not be a shared authentication
       *   token (returned by NoteStore.authenticateToSharedNotebook or 
       *   NoteStore.authenticateToSharedNote) or a business authentication token.
       * 
       * @return
       *   The result of the authentication, with the token granting access to the
       *   business in the result's 'authenticationToken' field. The URL that must
       *   be used to access the business account NoteStore will be returned in the
       *   result's 'noteStoreUrl' field.  The 'User' field will
       *   not be set in the result.
       *
       * @throws EDAMUserException <ul>
       *   <li> PERMISSION_DENIED "authenticationToken" - the provided authentication token
       *        is a shared or business authentication token. </li>
       *   <li> PERMISSION_DENIED "Business" - the user identified by the provided 
       *        authentication token is not currently a member of a business. </li>
       *   <li> PERMISSION_DENIED "Business.status" - the business that the user is a 
       *        member of is not currently in an active status. </li>
       * </ul>
       */
    AuthenticationResult authenticateToBusiness(QString authenticationToken = QString());

    /** Asynchronous version of @link authenticateToBusiness @endlink */
    AsyncResult* authenticateToBusinessAsync(QString authenticationToken = QString());

    /**
       * This is used to take an existing authentication token (returned from
       * 'authenticate') and exchange it for a newer token which will not expire
       * as soon.  This must be invoked before the previous token expires.
       *
       * This function is only availabe to Evernote's internal applications.
       *
       * @param authenticationToken
       *   The previous authentication token from the authenticate() result.
       *
       * @return
       *   The result of the authentication, with the new token in
       *   the result's 'authenticationToken' field.  The 'User' field will
       *   not be set in the result.
       */
    AuthenticationResult refreshAuthentication(QString authenticationToken = QString());

    /** Asynchronous version of @link refreshAuthentication @endlink */
    AsyncResult* refreshAuthenticationAsync(QString authenticationToken = QString());

    /**
       * Returns the User corresponding to the provided authentication token,
       * or throws an exception if this token is not valid.
       * The level of detail provided in the returned User structure depends on
       * the access level granted by the token, so a web service client may receive
       * fewer fields than an integrated desktop client.
       */
    User getUser(QString authenticationToken = QString());

    /** Asynchronous version of @link getUser @endlink */
    AsyncResult* getUserAsync(QString authenticationToken = QString());

    /**
       * Asks the UserStore about the publicly available location information for
       * a particular username.
       *
       * @throws EDAMUserException <ul>
       *   <li> DATA_REQUIRED "username" - username is empty
       * </ul>
       */
    PublicUserInfo getPublicUserInfo(QString username);

    /** Asynchronous version of @link getPublicUserInfo @endlink */
    AsyncResult* getPublicUserInfoAsync(QString username);

    /**
       * Returns information regarding a user's Premium account corresponding to the
       * provided authentication token, or throws an exception if this token is not
       * valid.
       */
    PremiumInfo getPremiumInfo(QString authenticationToken = QString());

    /** Asynchronous version of @link getPremiumInfo @endlink */
    AsyncResult* getPremiumInfoAsync(QString authenticationToken = QString());

    /**
       * Returns the URL that should be used to talk to the NoteStore for the
       * account represented by the provided authenticationToken.
       * This method isn't needed by most clients, who can retrieve the correct
       * NoteStore URL from the AuthenticationResult returned from the authenticate
       * or refreshAuthentication calls. This method is typically only needed
       * to look up the correct URL for a long-lived session token (e.g. for an
       * OAuth web service).
       */
    QString getNoteStoreUrl(QString authenticationToken = QString());

    /** Asynchronous version of @link getNoteStoreUrl @endlink */
    AsyncResult* getNoteStoreUrlAsync(QString authenticationToken = QString());

private:
    QString url_;
    QString authenticationToken_;
};
}

Q_DECLARE_METATYPE(QList< qevercloud::Notebook >)
Q_DECLARE_METATYPE(QList< qevercloud::Tag >)
Q_DECLARE_METATYPE(QList< qevercloud::SavedSearch >)
Q_DECLARE_METATYPE(QList< qevercloud::NoteVersionId >)
Q_DECLARE_METATYPE(QList< qevercloud::SharedNotebook >)
Q_DECLARE_METATYPE(QList< qevercloud::LinkedNotebook >)

#endif // QEVERCLOUD_GENERATED_SERVICES_H
