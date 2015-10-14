// This file is generated from Evernote Thrift API and is a part of the QEverCloud project

#ifndef QEVERCLOUD_GENERATED_TYPES_H
#define QEVERCLOUD_GENERATED_TYPES_H

#include <QMap>
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QDateTime>
#include "../Optional.h"
#include "EDAMErrorCode.h"
#include <QSharedPointer>
#include <QMetaType>

namespace qevercloud {

/**
 * This enumeration defines the possible permission levels for a user.
 * Free accounts will have a level of NORMAL and paid Premium accounts
 * will have a level of PREMIUM.
 */
struct PrivilegeLevel {
    enum type {
        NORMAL = 1,
        PREMIUM = 3,
        VIP = 5,
        MANAGER = 7,
        SUPPORT = 8,
        ADMIN = 9
    };
};

/**
 * Every search query is specified as a sequence of characters.
 * Currently, only the USER query format is supported.
 */
struct QueryFormat {
    enum type {
        USER = 1,
        SEXP = 2
    };
};

/**
 * This enumeration defines the possible sort ordering for notes when
 * they are returned from a search result.
 */
struct NoteSortOrder {
    enum type {
        CREATED = 1,
        UPDATED = 2,
        RELEVANCE = 3,
        UPDATE_SEQUENCE_NUMBER = 4,
        TITLE = 5
    };
};

/**
 * This enumeration defines the possible states of a premium account
 *
 * NONE:    the user has never attempted to become a premium subscriber
 *
 * PENDING: the user has requested a premium account but their charge has not
 *   been confirmed
 *
 * ACTIVE:  the user has been charged and their premium account is in good
 *  standing
 *
 * FAILED:  the system attempted to charge the was denied. Their premium
 *   privileges have been revoked. We will periodically attempt to re-validate
 *   their order.
 *
 * CANCELLATION_PENDING: the user has requested that no further charges be made
 *   but the current account is still active.
 *
 * CANCELED: the premium account was canceled either because of failure to pay
 *   or user cancelation. No more attempts will be made to activate the account.
 */
struct PremiumOrderStatus {
    enum type {
        NONE = 0,
        PENDING = 1,
        ACTIVE = 2,
        FAILED = 3,
        CANCELLATION_PENDING = 4,
        CANCELED = 5
    };
};

/**
 * Privilege levels for accessing shared notebooks.
 *
 * READ_NOTEBOOK: Recipient is able to read the contents of the shared notebook
 *   but does to have access to information about other recipients of the
 *   notebook or the activity stream information.
 *
 * MODIFY_NOTEBOOK_PLUS_ACTIVITY: Recipient has rights to read and modify the contents
 *   of the shared notebook, including the right to move notes to the trash and to create
 *   notes in the notebook.  The recipient can also access information about other
 *   recipients and the activity stream.
 *
 * READ_NOTEBOOK_PLUS_ACTIVITY: Recipient has READ_NOTEBOOK rights and can also
 *   access information about other recipients and the activity stream.
 *
 * GROUP: If the user belongs to a group, such as a Business, that has a defined
 *   privilege level, use the privilege level of the group as the privilege for
 *   the individual.
 *
 * FULL_ACCESS: Recipient has full rights to the shared notebook and recipient lists,
 *   including privilege to revoke and create invitations and to change privilege
 *   levels on invitations for individuals.  This privilege level is primarily intended
 *   for use by individual shares.
 *
 * BUSINESS_FULL_ACCESS: Intended for use with Business Notebooks, a
 * BUSINESS_FULL_ACCESS level is FULL_ACCESS with the additional rights to
 * change how the notebook will appear in the business library, including the
 * rights to publish and unpublish the notebook from the library.
 */
struct SharedNotebookPrivilegeLevel {
    enum type {
        READ_NOTEBOOK = 0,
        MODIFY_NOTEBOOK_PLUS_ACTIVITY = 1,
        READ_NOTEBOOK_PLUS_ACTIVITY = 2,
        GROUP = 3,
        FULL_ACCESS = 4,
        BUSINESS_FULL_ACCESS = 5
    };
};

/**
 * Enumeration of the roles that a User can have within a sponsored group.
 *
 * GROUP_MEMBER: The user is a member of the group with no special privileges.
 *
 * GROUP_ADMIN: The user is an administrator within the group.
 *
 * GROUP_OWNER: The user is the owner of the group.
 */
struct SponsoredGroupRole {
    enum type {
        GROUP_MEMBER = 1,
        GROUP_ADMIN = 2,
        GROUP_OWNER = 3
    };
};

/**
 * Enumeration of the roles that a User can have within an Evernote Business account.
 *
 * ADMIN: The user is an administrator of the Evernote Business account.
 *
 * NORMAL: The user is a regular user within the Evernote Business account.
 */
struct BusinessUserRole {
    enum type {
        ADMIN = 1,
        NORMAL = 2
    };
};

/**
 * An enumeration describing restrictions on the domain of shared notebook
 * instances that are valid for a given operation, as used, for example, in
 * NotebookRestrictions.
 *
 * ONLY_JOINED_OR_PREVIEW: The domain consists of shared notebooks that
 *   "belong" to the recipient or still available for preview by any recipient.
 *   Shared notebooks that the recipient has joined (the username has already been
 *   assigned to our user) are in the domain.  Additionally, shared notebooks
 *   that allow preview and have not yet been joined are in the domain.
 *
 * NO_SHARED_NOTEBOOKS: No shared notebooks are applicable to the operation.
 */
struct SharedNotebookInstanceRestrictions {
    enum type {
        ONLY_JOINED_OR_PREVIEW = 1,
        NO_SHARED_NOTEBOOKS = 2
    };
};

/**
 * An enumeration describing the configuration state related to receiving
 * reminder e-mails from the service.  Reminder e-mails summarize notes
 * based on their Note.attributes.reminderTime values.
 *
 * DO_NOT_SEND: The user has selected to not receive reminder e-mail.
 *
 * SEND_DAILY_EMAIL: The user has selected to receive reminder e-mail for those
 *   days when there is a reminder.
 */
struct ReminderEmailConfig {
    enum type {
        DO_NOT_SEND = 1,
        SEND_DAILY_EMAIL = 2
    };
};


/**
 * Every Evernote account is assigned a unique numeric identifier which
 * will not change for the life of the account.  This is independent of
 * the (string-based) "username" which is known by the user for login
 * purposes.  The user should have no reason to know their UserID.
 */
typedef qint32 UserID;

/**
 * Most data elements within a user's account (e.g. notebooks, notes, tags,
 * resources, etc.) are internally referred to using a globally unique
 * identifier that is written in a standard string format.  For example:
 *
 *    "8743428c-ef91-4d05-9e7c-4a2e856e813a"
 *
 * The internal components of the GUID are not given any particular meaning:
 * only the entire string is relevant as a unique identifier.
 */
typedef QString Guid;

/**
 * An Evernote Timestamp is the date and time of an event in UTC time.
 * This is expressed as a specific number of milliseconds since the
 * standard base "epoch" of:
 *
 *    January 1, 1970, 00:00:00 GMT
 *
 * NOTE:  the time is expressed at the resolution of milliseconds, but
 * the value is only precise to the level of seconds.   This means that
 * the last three (decimal) digits of the timestamp will be '000'.
 *
 * The Thrift IDL specification does not include a native date/time type,
 * so this value is used instead.
 *
 * The service will accept timestamp values (e.g. for Note created and update
 * times) between 1000-01-01 and 9999-12-31
 */
typedef qint64 Timestamp;


/**
 * This structure encapsulates the information about the state of the
 * user's account for the purpose of "state based" synchronization.
 **/
struct SyncState {
    /**
    The server's current date and time.
    */
    Timestamp currentTime;
    /**
    The cutoff date and time for client caches to be
       updated via incremental synchronization.  Any clients that were last
       synched with the server before this date/time must do a full resync of all
       objects.  This cutoff point will change over time as archival data is
       deleted or special circumstances on the service require resynchronization.
    */
    Timestamp fullSyncBefore;
    /**
    Indicates the total number of transactions that have
       been committed within the account.  This reflects (for example) the
       number of discrete additions or modifications that have been made to
       the data in this account (tags, notes, resources, etc.).
       This number is the "high water mark" for Update Sequence Numbers (USN)
       within the account.
    */
    qint32 updateCount;
    /**
    The total number of bytes that have been uploaded to
       this account in the current monthly period.  This can be compared against
       Accounting.uploadLimit (from the UserStore) to determine how close the user
       is to their monthly upload limit.
       This value may not be present if the SyncState has been retrieved by
       a caller that only has read access to the account.
    */
    Optional< qint64 > uploaded;

    bool operator==(const SyncState& other) const
    {
        return (currentTime == other.currentTime)
            && (fullSyncBefore == other.fullSyncBefore)
            && (updateCount == other.updateCount)
            && uploaded.isEqual(other.uploaded)
        ;
    }

    bool operator!=(const SyncState& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is used with the 'getFilteredSyncChunk' call to provide
 * fine-grained control over the data that's returned when a client needs
 * to synchronize with the service. Each flag in this structure specifies
 * whether to include one class of data in the results of that call.
 *
 **/
struct SyncChunkFilter {
    /**
    If true, then the server will include the SyncChunks.notes field
    */
    Optional< bool > includeNotes;
    /**
    If true, then the server will include the 'resources' field on all of
       the Notes that are in SyncChunk.notes.
       If 'includeNotes' is false, then this will have no effect.
    */
    Optional< bool > includeNoteResources;
    /**
    If true, then the server will include the 'attributes' field on all of
       the Notes that are in SyncChunks.notes.
       If 'includeNotes' is false, then this will have no effect.
    */
    Optional< bool > includeNoteAttributes;
    /**
    If true, then the server will include the SyncChunks.notebooks field
    */
    Optional< bool > includeNotebooks;
    /**
    If true, then the server will include the SyncChunks.tags field
    */
    Optional< bool > includeTags;
    /**
    If true, then the server will include the SyncChunks.searches field
    */
    Optional< bool > includeSearches;
    /**
    If true, then the server will include the SyncChunks.resources field.
       Since the Resources are also provided with their Note
       (in the Notes.resources list), this is primarily useful for clients that
       want to watch for changes to individual Resources due to recognition data
       being added.
    */
    Optional< bool > includeResources;
    /**
    If true, then the server will include the SyncChunks.linkedNotebooks field.
    */
    Optional< bool > includeLinkedNotebooks;
    /**
    If true, then the server will include the 'expunged' data for any type
       of included data.  For example, if 'includeTags' and 'includeExpunged'
       are both true, then the SyncChunks.expungedTags field will be set with
       the GUIDs of tags that have been expunged from the server.
    */
    Optional< bool > includeExpunged;
    /**
    If true, then the values for the applicationData map will be filled
       in, assuming notes and note attributes are being returned.  Otherwise,
       only the keysOnly field will be filled in.
    */
    Optional< bool > includeNoteApplicationDataFullMap;
    /**
    If true, then the fullMap values for the applicationData map will be
       filled in, assuming resources and resource attributes are being returned
       (includeResources is true).  Otherwise, only the keysOnly field will be
       filled in.
    */
    Optional< bool > includeResourceApplicationDataFullMap;
    /**
    If true, then the fullMap values for the applicationData map will be
       filled in for resources found inside of notes, assuming resources are
       being returned in notes (includeNoteResources is true).  Otherwise,
       only the keysOnly field will be filled in.
    */
    Optional< bool > includeNoteResourceApplicationDataFullMap;
    /**
    If set, then only send notes whose content class matches this value.
       The value can be a literal match or, if the last character is an
       asterisk, a prefix match.
    */
    Optional< QString > requireNoteContentClass;

    bool operator==(const SyncChunkFilter& other) const
    {
        return includeNotes.isEqual(other.includeNotes)
            && includeNoteResources.isEqual(other.includeNoteResources)
            && includeNoteAttributes.isEqual(other.includeNoteAttributes)
            && includeNotebooks.isEqual(other.includeNotebooks)
            && includeTags.isEqual(other.includeTags)
            && includeSearches.isEqual(other.includeSearches)
            && includeResources.isEqual(other.includeResources)
            && includeLinkedNotebooks.isEqual(other.includeLinkedNotebooks)
            && includeExpunged.isEqual(other.includeExpunged)
            && includeNoteApplicationDataFullMap.isEqual(other.includeNoteApplicationDataFullMap)
            && includeResourceApplicationDataFullMap.isEqual(other.includeResourceApplicationDataFullMap)
            && includeNoteResourceApplicationDataFullMap.isEqual(other.includeNoteResourceApplicationDataFullMap)
            && requireNoteContentClass.isEqual(other.requireNoteContentClass)
        ;
    }

    bool operator!=(const SyncChunkFilter& other) const
    {
        return !(*this == other);
    }

};

/**
 * A list of criteria that are used to indicate which notes are desired from
 * the account.  This is used in queries to the NoteStore to determine
 * which notes should be retrieved.
 *
 **/
struct NoteFilter {
    /**
    The NoteSortOrder value indicating what criterion should be
       used to sort the results of the filter.
    */
    Optional< qint32 > order;
    /**
    If true, the results will be ascending in the requested
       sort order.  If false, the results will be descending.
    */
    Optional< bool > ascending;
    /**
    If present, a search query string that will filter the set of notes to be returned.
       Accepts the full search grammar documented in the Evernote API Overview.
    */
    Optional< QString > words;
    /**
    If present, the Guid of the notebook that must contain
       the notes.
    */
    Optional< Guid > notebookGuid;
    /**
    If present, the list of tags (by GUID) that must be present
       on the notes.
    */
    Optional< QList< Guid > > tagGuids;
    /**
    The zone ID for the user, which will be used to interpret
       any dates or times in the queries that do not include their desired zone
       information.
       For example, if a query requests notes created "yesterday", this
       will be evaluated from the provided time zone, if provided.
       The format must be encoded as a standard zone ID such as
       "America/Los_Angeles".
    */
    Optional< QString > timeZone;
    /**
    If true, then only notes that are not active (i.e. notes in
       the Trash) will be returned. Otherwise, only active notes will be returned.
       There is no way to find both active and inactive notes in a single query.
    */
    Optional< bool > inactive;
    /**
    If present, a search query string that may or may not influence the notes
       to be returned, both in terms of coverage as well as of order. Think of it
       as a wish list, not a requirement.
       Accepts the full search grammar documented in the Evernote API Overview.
    */
    Optional< QString > emphasized;

    bool operator==(const NoteFilter& other) const
    {
        return order.isEqual(other.order)
            && ascending.isEqual(other.ascending)
            && words.isEqual(other.words)
            && notebookGuid.isEqual(other.notebookGuid)
            && tagGuids.isEqual(other.tagGuids)
            && timeZone.isEqual(other.timeZone)
            && inactive.isEqual(other.inactive)
            && emphasized.isEqual(other.emphasized)
        ;
    }

    bool operator!=(const NoteFilter& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is provided to the findNotesMetadata function to specify
 * the subset of fields that should be included in each NoteMetadata element
 * that is returned in the NotesMetadataList.
 * Each field on this structure is a boolean flag that indicates whether the
 * corresponding field should be included in the NoteMetadata structure when
 * it is returned.  For example, if the 'includeTitle' field is set on this
 * structure when calling findNotesMetadata, then each NoteMetadata in the
 * list should have its 'title' field set.
 * If one of the fields in this spec is not set, then it will be treated as
 * 'false' by the server, so the default behavior is to include nothing in
 * replies (but the mandatory GUID)
 */
struct NotesMetadataResultSpec {
    /** NOT DOCUMENTED */
    Optional< bool > includeTitle;
    /** NOT DOCUMENTED */
    Optional< bool > includeContentLength;
    /** NOT DOCUMENTED */
    Optional< bool > includeCreated;
    /** NOT DOCUMENTED */
    Optional< bool > includeUpdated;
    /** NOT DOCUMENTED */
    Optional< bool > includeDeleted;
    /** NOT DOCUMENTED */
    Optional< bool > includeUpdateSequenceNum;
    /** NOT DOCUMENTED */
    Optional< bool > includeNotebookGuid;
    /** NOT DOCUMENTED */
    Optional< bool > includeTagGuids;
    /** NOT DOCUMENTED */
    Optional< bool > includeAttributes;
    /** NOT DOCUMENTED */
    Optional< bool > includeLargestResourceMime;
    /** NOT DOCUMENTED */
    Optional< bool > includeLargestResourceSize;

    bool operator==(const NotesMetadataResultSpec& other) const
    {
        return includeTitle.isEqual(other.includeTitle)
            && includeContentLength.isEqual(other.includeContentLength)
            && includeCreated.isEqual(other.includeCreated)
            && includeUpdated.isEqual(other.includeUpdated)
            && includeDeleted.isEqual(other.includeDeleted)
            && includeUpdateSequenceNum.isEqual(other.includeUpdateSequenceNum)
            && includeNotebookGuid.isEqual(other.includeNotebookGuid)
            && includeTagGuids.isEqual(other.includeTagGuids)
            && includeAttributes.isEqual(other.includeAttributes)
            && includeLargestResourceMime.isEqual(other.includeLargestResourceMime)
            && includeLargestResourceSize.isEqual(other.includeLargestResourceSize)
        ;
    }

    bool operator!=(const NotesMetadataResultSpec& other) const
    {
        return !(*this == other);
    }

};

/**
 * A data structure representing the number of notes for each notebook
 * and tag with a non-zero set of applicable notes.
 *
 **/
struct NoteCollectionCounts {
    /**
    A mapping from the Notebook GUID to the number of
       notes (from some selection) that are in the corresponding notebook.
    */
    Optional< QMap< Guid, qint32 > > notebookCounts;
    /**
    A mapping from the Tag GUID to the number of notes (from some
       selection) that have the corresponding tag.
    */
    Optional< QMap< Guid, qint32 > > tagCounts;
    /**
    If this is set, then this is the number of notes that are in the trash.
       If this is not set, then the number of notes in the trash hasn't been
       reported.  (I.e. if there are no notes in the trash, this will be set
       to 0.)
    */
    Optional< qint32 > trashCount;

    bool operator==(const NoteCollectionCounts& other) const
    {
        return notebookCounts.isEqual(other.notebookCounts)
            && tagCounts.isEqual(other.tagCounts)
            && trashCount.isEqual(other.trashCount)
        ;
    }

    bool operator!=(const NoteCollectionCounts& other) const
    {
        return !(*this == other);
    }

};

/**
 * Identifying information about previous versions of a note that are backed up
 * within Evernote's servers.  Used in the return value of the listNoteVersions
 * call.
 *
 * */
struct NoteVersionId {
    /**
    The update sequence number for the Note when it last had this content.
        This serves to uniquely identify each version of the note, since USN
        values are unique within an account for each update.
    */
    qint32 updateSequenceNum;
    /**
    The 'updated' time that was set on the Note when it had this version
        of the content.  This is the user-modifiable modification time on the
        note, so it's not reliable for guaranteeing the order of various
        versions.  (E.g. if someone modifies the note, then changes this time
        manually into the past and then updates the note again.)
    */
    Timestamp updated;
    /**
    A timestamp that holds the date and time when this version of the note
        was backed up by Evernote's servers.  This
    */
    Timestamp saved;
    /**
    The title of the note when this particular version was saved.  (The
        current title of the note may differ from this value.)
    */
    QString title;

    bool operator==(const NoteVersionId& other) const
    {
        return (updateSequenceNum == other.updateSequenceNum)
            && (updated == other.updated)
            && (saved == other.saved)
            && (title == other.title)
        ;
    }

    bool operator!=(const NoteVersionId& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is passed from clients to the Evernote service when they wish
 * to relay coarse-grained usage metrics to the service to help improve
 * products.
 *
 * */
struct ClientUsageMetrics {
    /**
    This field contains a count of the number of usage "sessions" that have
        occurred with this client which have not previously been reported to
        the service.
        A "session" is defined as one of the 96 fifteen-minute intervals of the
        day when someone used Evernote's interface at least once.
        So if a user interacts with an Evernote client at 12:18, 12:24, and 12:36,
        and then the client synchronizes at 12:39, it would report that there were
        two previously-unreported sessions (one session for the 12:15-12:30 time
        period, and one for the 12:30-12:45 period).
        If the user used Evernote again at 12:41 and synchronized at 12:43, it
        would not report any new sessions, because the 12:30-12:45 session had
        already been reported.
    */
    Optional< qint32 > sessions;

    bool operator==(const ClientUsageMetrics& other) const
    {
        return sessions.isEqual(other.sessions)
        ;
    }

    bool operator!=(const ClientUsageMetrics& other) const
    {
        return !(*this == other);
    }

};

/**
 * A description of the thing for which we are searching for related
 * entities.
 *
 * You must specify either <em>noteGuid</em> or <em>plainText</em>, but
 * not both. <em>filter</em> and <em>referenceUri</em> are optional.
 *
 * */
struct RelatedQuery {
    /**
    The GUID of an existing note in your account for which related
         entities will be found.
    */
    Optional< QString > noteGuid;
    /**
    A string of plain text for which to find related entities.
         You should provide a text block with a number of characters between
         EDAM_RELATED_PLAINTEXT_LEN_MIN and EDAM_RELATED_PLAINTEXT_LEN_MAX.
    */
    Optional< QString > plainText;
    /**
    The list of criteria that will constrain the notes being considered
         related.
         Please note that some of the parameters may be ignored, such as
         <em>order</em> and <em>ascending</em>.
    */
    Optional< NoteFilter > filter;
    /**
    A URI string specifying a reference entity, around which "relatedness"
         should be based. This can be an URL pointing to a web page, for example.
    */
    Optional< QString > referenceUri;

    bool operator==(const RelatedQuery& other) const
    {
        return noteGuid.isEqual(other.noteGuid)
            && plainText.isEqual(other.plainText)
            && filter.isEqual(other.filter)
            && referenceUri.isEqual(other.referenceUri)
        ;
    }

    bool operator!=(const RelatedQuery& other) const
    {
        return !(*this == other);
    }

};

/**
 * A description of the thing for which the service will find related
 * entities, via findRelated(), together with a description of what
 * type of entities and how many you are seeking in the
 * RelatedResult.
 *
 * */
struct RelatedResultSpec {
    /**
    Return notes that are related to the query, but no more than
         this many.  Any value greater than EDAM_RELATED_MAX_NOTES
         will be silently capped.  If you do not set this field, then
         no notes will be returned.
    */
    Optional< qint32 > maxNotes;
    /**
    Return notebooks that are related to the query, but no more than
         this many.  Any value greater than EDAM_RELATED_MAX_NOTEBOOKS
         will be silently capped.  If you do not set this field, then
         no notebooks will be returned.
    */
    Optional< qint32 > maxNotebooks;
    /**
    Return tags that are related to the query, but no more than
         this many.  Any value greater than EDAM_RELATED_MAX_TAGS
         will be silently capped.  If you do not set this field, then
         no tags will be returned.
    */
    Optional< qint32 > maxTags;
    /**
    Require that all returned related notebooks are writable.
         The user will be able to create notes in all returned notebooks.
         However, individual notes returned may still belong to notebooks
         in which the user lacks the ability to create notes.
    */
    Optional< bool > writableNotebooksOnly;
    /**
    If set to <code>true</code>, return the containingNotebooks field
         in the RelatedResult, which will contain the list of notebooks to
         to which the returned related notes belong.
    */
    Optional< bool > includeContainingNotebooks;

    bool operator==(const RelatedResultSpec& other) const
    {
        return maxNotes.isEqual(other.maxNotes)
            && maxNotebooks.isEqual(other.maxNotebooks)
            && maxTags.isEqual(other.maxTags)
            && writableNotebooksOnly.isEqual(other.writableNotebooksOnly)
            && includeContainingNotebooks.isEqual(other.includeContainingNotebooks)
        ;
    }

    bool operator!=(const RelatedResultSpec& other) const
    {
        return !(*this == other);
    }

};

/**
 * In several places, EDAM exchanges blocks of bytes of data for a component
 * which may be relatively large.  For example:  the contents of a clipped
 * HTML note, the bytes of an embedded image, or the recognition XML for
 * a large image.  This structure is used in the protocol to represent
 * any of those large blocks of data when they are transmitted or when
 * they are only referenced their metadata.
 *
 **/
struct Data {
    /**
    This field carries a one-way hash of the contents of the
       data body, in binary form.  The hash function is MD5<br/>
       Length:  EDAM_HASH_LEN (exactly)
    */
    Optional< QByteArray > bodyHash;
    /**
    The length, in bytes, of the data body.
    */
    Optional< qint32 > size;
    /**
    This field is set to contain the binary contents of the data
       whenever the resource is being transferred.  If only metadata is
       being exchanged, this field will be empty.  For example, a client could
       notify the service about the change to an attribute for a resource
       without transmitting the binary resource contents.
    */
    Optional< QByteArray > body;

    bool operator==(const Data& other) const
    {
        return bodyHash.isEqual(other.bodyHash)
            && size.isEqual(other.size)
            && body.isEqual(other.body)
        ;
    }

    bool operator!=(const Data& other) const
    {
        return !(*this == other);
    }

};

/**
 * A structure holding the optional attributes that can be stored
 * on a User.  These are generally less critical than the core User fields.
 *
 **/
struct UserAttributes {
    /**
    the location string that should be associated
       with the user in order to determine where notes are taken if not otherwise
       specified.<br/>
       Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > defaultLocationName;
    /**
    if set, this is the latitude that should be
       assigned to any notes that have no other latitude information.
    */
    Optional< double > defaultLatitude;
    /**
    if set, this is the longitude that should be
       assigned to any notes that have no other longitude information.
    */
    Optional< double > defaultLongitude;
    /**
    if set, the user account is not yet confirmed for
       login.  I.e. the account has been created, but we are still waiting for
       the user to complete the activation step.
    */
    Optional< bool > preactivation;
    /**
    a list of promotions the user has seen.
        This list may occasionally be modified by the system when promotions are
        no longer available.<br/>
        Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QStringList > viewedPromotions;
    /**
    if set, this is the email address that the
        user may send email to in order to add an email note directly into the
        account via the SMTP email gateway.  This is the part of the email
        address before the '@' symbol ... our domain is not included.
        If this is not set, the user may not add notes via the gateway.<br/>
        Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > incomingEmailAddress;
    /**
    if set, this will contain a list of email
        addresses that have recently been used as recipients
        of outbound emails by the user.  This can be used to pre-populate a
        list of possible destinations when a user wishes to send a note via
        email.<br/>
        Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX each<br/>
        Max:  EDAM_USER_RECENT_MAILED_ADDRESSES_MAX entries
    */
    Optional< QStringList > recentMailedAddresses;
    /**
    Free-form text field that may hold general support
        information, etc.<br/>
        Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > comments;
    /**
    The date/time when the user agreed to
        the terms of service.  This can be used as the effective "start date"
        for the account.
    */
    Optional< Timestamp > dateAgreedToTermsOfService;
    /**
    The number of referrals that the user is permitted
        to make.
    */
    Optional< qint32 > maxReferrals;
    /**
    The number of referrals sent from this account.
    */
    Optional< qint32 > referralCount;
    /**
    A code indicating where the user was sent from. AKA
        promotion code
    */
    Optional< QString > refererCode;
    /**
    The most recent date when the user sent outbound
        emails from the service.  Used with sentEmailCount to limit the number
        of emails that can be sent per day.
    */
    Optional< Timestamp > sentEmailDate;
    /**
    The number of emails that were sent from the user
        via the service on sentEmailDate.  Used to enforce a limit on the number
        of emails per user per day to prevent spamming.
    */
    Optional< qint32 > sentEmailCount;
    /**
    If set, this is the maximum number of emails that
        may be sent in a given day from this account.  If unset, the server will
        use the configured default limit.
    */
    Optional< qint32 > dailyEmailLimit;
    /**
    If set, this is the date when the user asked
        to be excluded from offers and promotions sent by Evernote.  If not set,
        then the user currently agrees to receive these messages.
    */
    Optional< Timestamp > emailOptOutDate;
    /**
    If set, this is the date when the user asked
        to be included in offers and promotions sent by Evernote's partners.
        If not sent, then the user currently does not agree to receive these
        emails.
    */
    Optional< Timestamp > partnerEmailOptInDate;
    /**
    a 2 character language codes based on:
           http://ftp.ics.uci.edu/pub/ietf/http/related/iso639.txt used for
          localization purposes to determine what language to use for the web
          interface and for other direct communication (e.g. emails).
    */
    Optional< QString > preferredLanguage;
    /**
    Preferred country code based on ISO 3166-1-alpha-2 indicating the
       users preferred country
    */
    Optional< QString > preferredCountry;
    /**
    Boolean flag set to true if the user wants to clip full pages by
       default when they use the web clipper without a selection.
    */
    Optional< bool > clipFullPage;
    /**
    The username of the account of someone who has chosen to enable
       Twittering into Evernote.  This value is subject to change, since users
       may change their Twitter user name.
    */
    Optional< QString > twitterUserName;
    /**
    The unique identifier of the user's Twitter account if that user
       has chosen to enable Twittering into Evernote.
    */
    Optional< QString > twitterId;
    /**
    A name identifier used to identify a particular set of branding and
        light customization.
    */
    Optional< QString > groupName;
    /**
    a 2 character language codes based on:
           http://ftp.ics.uci.edu/pub/ietf/http/related/iso639.txt
           If set, this is used to determine the language that should be used
           when processing images and PDF files to find text.
           If not set, then the 'preferredLanguage' will be used.
    */
    Optional< QString > recognitionLanguage;
    /** NOT DOCUMENTED */
    Optional< QString > referralProof;
    /** NOT DOCUMENTED */
    Optional< bool > educationalDiscount;
    /**
    A string recording the business address of a Sponsored Account user who has requested invoicing.
    */
    Optional< QString > businessAddress;
    /**
    A flag indicating whether to hide the billing information on a sponsored
           account owner's settings page
    */
    Optional< bool > hideSponsorBilling;
    /**
    A flag indicating the user's sponsored group is exempt from sale tax
    */
    Optional< bool > taxExempt;
    /**
    A flag indicating whether the user chooses to allow Evernote to automatically
           file and tag emailed notes
    */
    Optional< bool > useEmailAutoFiling;
    /**
    Configuration state for whether or not the user wishes to receive
           reminder e-mail.  This setting applies to both the reminder e-mail sent
           for personal reminder notes and for the reminder e-mail sent for reminder
           notes in the user's business notebooks that the user has configured for
           e-mail notifications.
    */
    Optional< ReminderEmailConfig::type > reminderEmailConfig;

    bool operator==(const UserAttributes& other) const
    {
        return defaultLocationName.isEqual(other.defaultLocationName)
            && defaultLatitude.isEqual(other.defaultLatitude)
            && defaultLongitude.isEqual(other.defaultLongitude)
            && preactivation.isEqual(other.preactivation)
            && viewedPromotions.isEqual(other.viewedPromotions)
            && incomingEmailAddress.isEqual(other.incomingEmailAddress)
            && recentMailedAddresses.isEqual(other.recentMailedAddresses)
            && comments.isEqual(other.comments)
            && dateAgreedToTermsOfService.isEqual(other.dateAgreedToTermsOfService)
            && maxReferrals.isEqual(other.maxReferrals)
            && referralCount.isEqual(other.referralCount)
            && refererCode.isEqual(other.refererCode)
            && sentEmailDate.isEqual(other.sentEmailDate)
            && sentEmailCount.isEqual(other.sentEmailCount)
            && dailyEmailLimit.isEqual(other.dailyEmailLimit)
            && emailOptOutDate.isEqual(other.emailOptOutDate)
            && partnerEmailOptInDate.isEqual(other.partnerEmailOptInDate)
            && preferredLanguage.isEqual(other.preferredLanguage)
            && preferredCountry.isEqual(other.preferredCountry)
            && clipFullPage.isEqual(other.clipFullPage)
            && twitterUserName.isEqual(other.twitterUserName)
            && twitterId.isEqual(other.twitterId)
            && groupName.isEqual(other.groupName)
            && recognitionLanguage.isEqual(other.recognitionLanguage)
            && referralProof.isEqual(other.referralProof)
            && educationalDiscount.isEqual(other.educationalDiscount)
            && businessAddress.isEqual(other.businessAddress)
            && hideSponsorBilling.isEqual(other.hideSponsorBilling)
            && taxExempt.isEqual(other.taxExempt)
            && useEmailAutoFiling.isEqual(other.useEmailAutoFiling)
            && reminderEmailConfig.isEqual(other.reminderEmailConfig)
        ;
    }

    bool operator!=(const UserAttributes& other) const
    {
        return !(*this == other);
    }

};

/**
 * This represents the bookkeeping information for the user's subscription.
 *
 **/
struct Accounting {
    /**
    The number of bytes that can be uploaded to the account
       in the current month.  For new notes that are created, this is the length
       of the note content (in Unicode characters) plus the size of each resource
       (in bytes).  For edited notes, this is the the difference between the old
       length and the new length (if this is greater than 0) plus the size of
       each new resource.
    */
    Optional< qint64 > uploadLimit;
    /**
    The date and time when the current upload limit
       expires.  At this time, the monthly upload count reverts to 0 and a new
       limit is imposed.  This date and time is exclusive, so this is effectively
       the start of the new month.
    */
    Optional< Timestamp > uploadLimitEnd;
    /**
    When uploadLimitEnd is reached, the service
       will change uploadLimit to uploadLimitNextMonth. If a premium account is
       canceled, this mechanism will reset the quota appropriately.
    */
    Optional< qint64 > uploadLimitNextMonth;
    /**
    Indicates the phases of a premium account
       during the billing process.
    */
    Optional< PremiumOrderStatus::type > premiumServiceStatus;
    /**
    The order number used by the commerce system to
       process recurring payments
    */
    Optional< QString > premiumOrderNumber;
    /**
    The commerce system used (paypal, Google
       checkout, etc)
    */
    Optional< QString > premiumCommerceService;
    /**
    The start date when this premium promotion
       began (this number will get overwritten if a premium service is canceled
       and then re-activated).
    */
    Optional< Timestamp > premiumServiceStart;
    /**
    The code associated with the purchase eg. monthly
       or annual purchase. Clients should interpret this value and localize it.
    */
    Optional< QString > premiumServiceSKU;
    /**
    Date the last time the user was charged.
       Null if never charged.
    */
    Optional< Timestamp > lastSuccessfulCharge;
    /**
    Date the last time a charge was attempted and
       failed.
    */
    Optional< Timestamp > lastFailedCharge;
    /**
    Reason provided for the charge failure
    */
    Optional< QString > lastFailedChargeReason;
    /**
    The end of the billing cycle. This could be in the
       past if there are failed charges.
    */
    Optional< Timestamp > nextPaymentDue;
    /**
    An internal variable to manage locking operations
       on the commerce variables.
    */
    Optional< Timestamp > premiumLockUntil;
    /**
    The date any modification where made to this record.
    */
    Optional< Timestamp > updated;
    /**
    The number number identifying the
       recurring subscription used to make the recurring charges.
    */
    Optional< QString > premiumSubscriptionNumber;
    /**
    Date charge last attempted
    */
    Optional< Timestamp > lastRequestedCharge;
    /**
    ISO 4217 currency code
    */
    Optional< QString > currency;
    /**
    charge in the smallest unit of the currency (e.g. cents for USD)
    */
    Optional< qint32 > unitPrice;
    /**
    <i>DEPRECATED:</i>See BusinessUserInfo.
    */
    Optional< qint32 > businessId;
    /**
    <i>DEPRECATED:</i>See BusinessUserInfo.
    */
    Optional< QString > businessName;
    /**
    <i>DEPRECATED:</i>See BusinessUserInfo.
    */
    Optional< BusinessUserRole::type > businessRole;
    /**
    discount per seat in negative amount and smallest unit of the currency (e.g. cents for USD)
    */
    Optional< qint32 > unitDiscount;
    /**
    The next time the user will be charged, may or may not be the same as nextPaymentDue
    */
    Optional< Timestamp > nextChargeDate;

    bool operator==(const Accounting& other) const
    {
        return uploadLimit.isEqual(other.uploadLimit)
            && uploadLimitEnd.isEqual(other.uploadLimitEnd)
            && uploadLimitNextMonth.isEqual(other.uploadLimitNextMonth)
            && premiumServiceStatus.isEqual(other.premiumServiceStatus)
            && premiumOrderNumber.isEqual(other.premiumOrderNumber)
            && premiumCommerceService.isEqual(other.premiumCommerceService)
            && premiumServiceStart.isEqual(other.premiumServiceStart)
            && premiumServiceSKU.isEqual(other.premiumServiceSKU)
            && lastSuccessfulCharge.isEqual(other.lastSuccessfulCharge)
            && lastFailedCharge.isEqual(other.lastFailedCharge)
            && lastFailedChargeReason.isEqual(other.lastFailedChargeReason)
            && nextPaymentDue.isEqual(other.nextPaymentDue)
            && premiumLockUntil.isEqual(other.premiumLockUntil)
            && updated.isEqual(other.updated)
            && premiumSubscriptionNumber.isEqual(other.premiumSubscriptionNumber)
            && lastRequestedCharge.isEqual(other.lastRequestedCharge)
            && currency.isEqual(other.currency)
            && unitPrice.isEqual(other.unitPrice)
            && businessId.isEqual(other.businessId)
            && businessName.isEqual(other.businessName)
            && businessRole.isEqual(other.businessRole)
            && unitDiscount.isEqual(other.unitDiscount)
            && nextChargeDate.isEqual(other.nextChargeDate)
        ;
    }

    bool operator!=(const Accounting& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is used to provide information about an Evernote Business
 * membership, for members who are part of a business.
 *
 * */
struct BusinessUserInfo {
    /**
    The ID of the Evernote Business account that the user is a member of.
     <dt>businessName
       The human-readable name of the Evernote Business account that the user
           is a member of.
    */
    Optional< qint32 > businessId;
    /** NOT DOCUMENTED */
    Optional< QString > businessName;
    /**
    The role of the user within the Evernote Business account that
           they are a member of.
    */
    Optional< BusinessUserRole::type > role;
    /**
    An e-mail address that will be used by the service in the context of your
           Evernote Business activities.  For example, this e-mail address will be used
           when you e-mail a business note, when you update notes in the account of
           your business, etc.  The business e-mail cannot be used for identification
           purposes such as for logging into the service.
    */
    Optional< QString > email;

    bool operator==(const BusinessUserInfo& other) const
    {
        return businessId.isEqual(other.businessId)
            && businessName.isEqual(other.businessName)
            && role.isEqual(other.role)
            && email.isEqual(other.email)
        ;
    }

    bool operator!=(const BusinessUserInfo& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is used to provide information about a user's Premium account.
 **/
struct PremiumInfo {
    /**
    The server-side date and time when this data was generated.
    */
    Timestamp currentTime;
    /**
    True if the user's account is Premium.
    */
    bool premium;
    /**
    True if the user's account is Premium and has a recurring payment method.
    */
    bool premiumRecurring;
    /**
    The date when the user's Premium account expires, or the date when the
       user's account is due for payment if it has a recurring payment method.
    */
    Optional< Timestamp > premiumExpirationDate;
    /**
    True if the user is eligible for purchasing Premium account extensions.
    */
    bool premiumExtendable;
    /**
    True if the user's Premium account is pending payment confirmation
    */
    bool premiumPending;
    /**
    True if the user has requested that no further charges to be made; the
       Premium account will remain active until it expires.
    */
    bool premiumCancellationPending;
    /**
    True if the user is eligible for purchasing additional upload allowance.
    */
    bool canPurchaseUploadAllowance;
    /**
    The name of the sponsored group that the user is part of.
    */
    Optional< QString > sponsoredGroupName;
    /**
    DEPRECATED - will be removed in a future update.
    */
    Optional< SponsoredGroupRole::type > sponsoredGroupRole;
    /**
    True if the user is eligible for purchasing Premium account upgrade.
    */
    Optional< bool > premiumUpgradable;

    bool operator==(const PremiumInfo& other) const
    {
        return (currentTime == other.currentTime)
            && (premium == other.premium)
            && (premiumRecurring == other.premiumRecurring)
            && premiumExpirationDate.isEqual(other.premiumExpirationDate)
            && (premiumExtendable == other.premiumExtendable)
            && (premiumPending == other.premiumPending)
            && (premiumCancellationPending == other.premiumCancellationPending)
            && (canPurchaseUploadAllowance == other.canPurchaseUploadAllowance)
            && sponsoredGroupName.isEqual(other.sponsoredGroupName)
            && sponsoredGroupRole.isEqual(other.sponsoredGroupRole)
            && premiumUpgradable.isEqual(other.premiumUpgradable)
        ;
    }

    bool operator!=(const PremiumInfo& other) const
    {
        return !(*this == other);
    }

};

/**
 * This represents the information about a single user account.
 **/
struct User {
    /**
    The unique numeric identifier for the account, which will not
       change for the lifetime of the account.
    */
    Optional< UserID > id;
    /**
    The name that uniquely identifies a single user account. This name
       may be presented by the user, along with their password, to log into
       their account.
       May only contain a-z, 0-9, or '-', and may not start or end with the '-'
       <br/>
       Length:  EDAM_USER_USERNAME_LEN_MIN - EDAM_USER_USERNAME_LEN_MAX
       <br/>
       Regex:  EDAM_USER_USERNAME_REGEX
    */
    Optional< QString > username;
    /**
    The email address registered for the user.  Must comply with
       RFC 2821 and RFC 2822.<br/>
       Third party applications that authenticate using OAuth do not have
       access to this field.
       Length:  EDAM_EMAIL_LEN_MIN - EDAM_EMAIL_LEN_MAX
       <br/>
       Regex:  EDAM_EMAIL_REGEX
    */
    Optional< QString > email;
    /**
    The printable name of the user, which may be a combination
       of given and family names.  This is used instead of separate "first"
       and "last" names due to variations in international name format/order.
       May not start or end with a whitespace character.  May contain any
       character but carriage return or newline (Unicode classes Zl and Zp).
       <br/>
       Length:  EDAM_USER_NAME_LEN_MIN - EDAM_USER_NAME_LEN_MAX
       <br/>
       Regex:  EDAM_USER_NAME_REGEX
    */
    Optional< QString > name;
    /**
    The zone ID for the user's default location.  If present,
       this may be used to localize the display of any timestamp for which no
       other timezone is available.
       The format must be encoded as a standard zone ID such as
       "America/Los_Angeles" or "GMT+08:00"
       <br/>
       Length:  EDAM_TIMEZONE_LEN_MIN - EDAM_TIMEZONE_LEN_MAX
       <br/>
       Regex:  EDAM_TIMEZONE_REGEX
    */
    Optional< QString > timezone;
    /**
    The level of access permitted for the user.
    */
    Optional< PrivilegeLevel::type > privilege;
    /**
    The date and time when this user account was created in the
       service.
    */
    Optional< Timestamp > created;
    /**
    The date and time when this user account was last modified
       in the service.
    */
    Optional< Timestamp > updated;
    /**
    If the account has been deleted from the system (e.g. as
       the result of a legal request by the user), the date and time of the
       deletion will be represented here.  If not, this value will not be set.
    */
    Optional< Timestamp > deleted;
    /**
    If the user account is available for login and
       synchronization, this flag will be set to true.
    */
    Optional< bool > active;
    /**
    DEPRECATED - Client applications should have no need to use this field.
    */
    Optional< QString > shardId;
    /**
    If present, this will contain a list of the attributes
       for this user account.
    */
    Optional< UserAttributes > attributes;
    /**
    Bookkeeping information for the user's subscription.
    */
    Optional< Accounting > accounting;
    /**
    If present, this will contain a set of commerce information
       relating to the user's premium service level.
    */
    Optional< PremiumInfo > premiumInfo;
    /**
    If present, this will contain a set of business information
       relating to the user's business membership.  If not present, the
       user is not currently part of a business.
    */
    Optional< BusinessUserInfo > businessUserInfo;

    bool operator==(const User& other) const
    {
        return id.isEqual(other.id)
            && username.isEqual(other.username)
            && email.isEqual(other.email)
            && name.isEqual(other.name)
            && timezone.isEqual(other.timezone)
            && privilege.isEqual(other.privilege)
            && created.isEqual(other.created)
            && updated.isEqual(other.updated)
            && deleted.isEqual(other.deleted)
            && active.isEqual(other.active)
            && shardId.isEqual(other.shardId)
            && attributes.isEqual(other.attributes)
            && accounting.isEqual(other.accounting)
            && premiumInfo.isEqual(other.premiumInfo)
            && businessUserInfo.isEqual(other.businessUserInfo)
        ;
    }

    bool operator!=(const User& other) const
    {
        return !(*this == other);
    }

};

/**
 * A tag within a user's account is a unique name which may be organized
 * a simple hierarchy.
 **/
struct Tag {
    /**
    The unique identifier of this tag. Will be set by the service,
       so may be omitted by the client when creating the Tag.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > guid;
    /**
    A sequence of characters representing the tag's identifier.
       Case is preserved, but is ignored for comparisons.
       This means that an account may only have one tag with a given name, via
       case-insensitive comparison, so an account may not have both "food" and
       "Food" tags.
       May not contain a comma (','), and may not begin or end with a space.
       <br/>
       Length:  EDAM_TAG_NAME_LEN_MIN - EDAM_TAG_NAME_LEN_MAX
       <br/>
       Regex:  EDAM_TAG_NAME_REGEX
    */
    Optional< QString > name;
    /**
    If this is set, then this is the GUID of the tag that
       holds this tag within the tag organizational hierarchy.  If this is
       not set, then the tag has no parent and it is a "top level" tag.
       Cycles are not allowed (e.g. a->parent->parent == a) and will be
       rejected by the service.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > parentGuid;
    /**
    A number identifying the last transaction to
       modify the state of this object.  The USN values are sequential within an
       account, and can be used to compare the order of modifications within the
       service.
    */
    Optional< qint32 > updateSequenceNum;

    bool operator==(const Tag& other) const
    {
        return guid.isEqual(other.guid)
            && name.isEqual(other.name)
            && parentGuid.isEqual(other.parentGuid)
            && updateSequenceNum.isEqual(other.updateSequenceNum)
        ;
    }

    bool operator!=(const Tag& other) const
    {
        return !(*this == other);
    }

};

/**
 * A structure that wraps a map of name/value pairs whose values are not
 * always present in the structure in order to reduce space when obtaining
 * batches of entities that contain the map.
 *
 * When the server provides the client with a LazyMap, it will fill in either
 * the keysOnly field or the fullMap field, but never both, based on the API
 * and parameters.
 *
 * When a client provides a LazyMap to the server as part of an update to
 * an object, the server will only update the LazyMap if the fullMap field is
 * set. If the fullMap field is not set, the server will not make any changes
 * to the map.
 *
 * Check the API documentation of the individual calls involving the LazyMap
 * for full details including the constraints of the names and values of the
 * map.
 *
 * */
struct LazyMap {
    /**
    The set of keys for the map.  This field is ignored by the
           server when set.
    */
    Optional< QSet< QString > > keysOnly;
    /**
    The complete map, including all keys and values.
    */
    Optional< QMap< QString, QString > > fullMap;

    bool operator==(const LazyMap& other) const
    {
        return keysOnly.isEqual(other.keysOnly)
            && fullMap.isEqual(other.fullMap)
        ;
    }

    bool operator!=(const LazyMap& other) const
    {
        return !(*this == other);
    }

};

/**
 * Structure holding the optional attributes of a Resource
 * */
struct ResourceAttributes {
    /**
    the original location where the resource was hosted
       <br/>
        Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > sourceURL;
    /**
    the date and time that is associated with this resource
       (e.g. the time embedded in an image from a digital camera with a clock)
    */
    Optional< Timestamp > timestamp;
    /**
    the latitude where the resource was captured
    */
    Optional< double > latitude;
    /**
    the longitude where the resource was captured
    */
    Optional< double > longitude;
    /**
    the altitude where the resource was captured
    */
    Optional< double > altitude;
    /**
    information about an image's camera, e.g. as embedded in
       the image's EXIF data
       <br/>
       Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > cameraMake;
    /**
    information about an image's camera, e.g. as embedded
       in the image's EXIF data
       <br/>
       Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > cameraModel;
    /**
    if true, then the original client that submitted
       the resource plans to submit the recognition index for this resource at a
       later time.
    */
    Optional< bool > clientWillIndex;
    /**
    DEPRECATED - this field is no longer set by the service, so should
         be ignored.
    */
    Optional< QString > recoType;
    /**
    if the resource came from a source that provided an
       explicit file name, the original name will be stored here.  Many resources
       come from unnamed sources, so this will not always be set.
    */
    Optional< QString > fileName;
    /**
    this will be true if the resource should be displayed as an attachment,
       or false if the resource should be displayed inline (if possible).
    */
    Optional< bool > attachment;
    /**
    Provides a location for applications to store a relatively small
     (4kb) blob of data associated with a Resource that is not visible to the user
     and that is opaque to the Evernote service. A single application may use at most
     one entry in this map, using its API consumer key as the map key. See the
     documentation for LazyMap for a description of when the actual map values
     are returned by the service.
     <p>To safely add or modify your application's entry in the map, use
     NoteStore.setResourceApplicationDataEntry. To safely remove your application's
     entry from the map, use NoteStore.unsetResourceApplicationDataEntry.</p>
     Minimum length of a name (key): EDAM_APPLICATIONDATA_NAME_LEN_MIN
     <br/>
     Sum max size of key and value: EDAM_APPLICATIONDATA_ENTRY_LEN_MAX
     <br/>
     Syntax regex for name (key): EDAM_APPLICATIONDATA_NAME_REGEX
    */
    Optional< LazyMap > applicationData;

    bool operator==(const ResourceAttributes& other) const
    {
        return sourceURL.isEqual(other.sourceURL)
            && timestamp.isEqual(other.timestamp)
            && latitude.isEqual(other.latitude)
            && longitude.isEqual(other.longitude)
            && altitude.isEqual(other.altitude)
            && cameraMake.isEqual(other.cameraMake)
            && cameraModel.isEqual(other.cameraModel)
            && clientWillIndex.isEqual(other.clientWillIndex)
            && recoType.isEqual(other.recoType)
            && fileName.isEqual(other.fileName)
            && attachment.isEqual(other.attachment)
            && applicationData.isEqual(other.applicationData)
        ;
    }

    bool operator!=(const ResourceAttributes& other) const
    {
        return !(*this == other);
    }

};

/**
 * Every media file that is embedded or attached to a note is represented
 * through a Resource entry.
 * */
struct Resource {
    /**
    The unique identifier of this resource.  Will be set whenever
       a resource is retrieved from the service, but may be null when a client
       is creating a resource.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > guid;
    /**
    The unique identifier of the Note that holds this
       Resource. Will be set whenever the resource is retrieved from the service,
       but may be null when a client is creating a resource.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > noteGuid;
    /**
    The contents of the resource.
       Maximum length:  The data.body is limited to EDAM_RESOURCE_SIZE_MAX_FREE
       for free accounts and EDAM_RESOURCE_SIZE_MAX_PREMIUM for premium accounts.
    */
    Optional< Data > data;
    /**
    The MIME type for the embedded resource.  E.g. "image/gif"
       <br/>
       Length:  EDAM_MIME_LEN_MIN - EDAM_MIME_LEN_MAX
       <br/>
       Regex:  EDAM_MIME_REGEX
    */
    Optional< QString > mime;
    /**
    If set, this contains the display width of this resource, in
       pixels.
    */
    Optional< qint16 > width;
    /**
    If set, this contains the display height of this resource,
       in pixels.
    */
    Optional< qint16 > height;
    /**
    DEPRECATED: ignored.
    */
    Optional< qint16 > duration;
    /**
    DEPRECATED: ignored.
    */
    Optional< bool > active;
    /**
    If set, this will hold the encoded data that provides
       information on search and recognition within this resource.
    */
    Optional< Data > recognition;
    /**
    A list of the attributes for this resource.
    */
    Optional< ResourceAttributes > attributes;
    /**
    A number identifying the last transaction to
       modify the state of this object. The USN values are sequential within an
       account, and can be used to compare the order of modifications within the
       service.
    */
    Optional< qint32 > updateSequenceNum;
    /**
    Some Resources may be assigned an alternate data format by the service
       which may be more appropriate for indexing or rendering than the original
       data provided by the user.  In these cases, the alternate data form will
       be available via this Data element.  If a Resource has no alternate form,
       this field will be unset.
    */
    Optional< Data > alternateData;

    bool operator==(const Resource& other) const
    {
        return guid.isEqual(other.guid)
            && noteGuid.isEqual(other.noteGuid)
            && data.isEqual(other.data)
            && mime.isEqual(other.mime)
            && width.isEqual(other.width)
            && height.isEqual(other.height)
            && duration.isEqual(other.duration)
            && active.isEqual(other.active)
            && recognition.isEqual(other.recognition)
            && attributes.isEqual(other.attributes)
            && updateSequenceNum.isEqual(other.updateSequenceNum)
            && alternateData.isEqual(other.alternateData)
        ;
    }

    bool operator!=(const Resource& other) const
    {
        return !(*this == other);
    }

};

/**
 * The list of optional attributes that can be stored on a note.
 * */
struct NoteAttributes {
    /**
    time that the note refers to
    */
    Optional< Timestamp > subjectDate;
    /**
    the latitude where the note was taken
    */
    Optional< double > latitude;
    /**
    the longitude where the note was taken
    */
    Optional< double > longitude;
    /**
    the altitude where the note was taken
    */
    Optional< double > altitude;
    /**
    the author of the content of the note
       <br/>
       Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > author;
    /**
    the method that the note was added to the account, if the
       note wasn't directly authored in an Evernote desktop client.
       <br/>
       Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > source;
    /**
    the original location where the resource was hosted. For web clips,
       this will be the URL of the page that was clipped.
       <br/>
       Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > sourceURL;
    /**
    an identifying string for the application that
       created this note.  This string does not have a guaranteed syntax or
       structure -- it is intended for human inspection and tracking.
       <br/>
       Length:  EDAM_ATTRIBUTE_LEN_MIN - EDAM_ATTRIBUTE_LEN_MAX
    */
    Optional< QString > sourceApplication;
    /**
    The date and time when this note was directly shared via its own URL.
      This is only set on notes that were individually shared - it is independent
      of any notebook-level sharing of the containing notebook. This field
      is treated as "read-only" for clients; the server will ignore changes
      to this field from an external client.
    */
    Optional< Timestamp > shareDate;
    /**
    The set of notes with this parameter set are considered
     "reminders" and are to be treated specially by clients to give them
     higher UI prominence within a notebook.  The value is used to sort
     the reminder notes within the notebook with higher values
     representing greater prominence.  Outside of the context of a
     notebook, the value of this parameter is undefined.  The value is
     not intended to be compared to the values of reminder notes in
     other notebooks.  In order to allow clients to place a note at a
     higher precedence than other notes, you should never set a value
     greater than the current time (as defined for a Timetstamp). To
     place a note at higher precedence than existing notes, set the
     value to the current time as defined for a timestamp (milliseconds
     since the epoch).  Synchronizing clients must remember the time when
     the update was performed, using the local clock on the client,
     and use that value when they later upload the note to the service.
     Clients must not set the reminderOrder to the reminderTime as the
     reminderTime could be in the future.  Those two fields are never
     intended to be related.  The correct value for reminderOrder field
     for new notes is the "current" time when the user indicated that
     the note is a reminder.  Clients may implement a separate
     "sort by date" feature to show notes ordered by reminderTime.
     Whenever a reminderDoneTime or reminderTime is set but a
     reminderOrder is not set, the server will fill in the current
     server time for the reminderOrder field.
    */
    Optional< qint64 > reminderOrder;
    /**
    The date and time when a user dismissed/"marked done" the reminder
     on the note.  Users typically do not manually set this value directly
     as it is set to the time when the user dismissed/"marked done" the
     reminder.
    */
    Optional< Timestamp > reminderDoneTime;
    /**
    The date and time a user has selected to be reminded of the note.
     A note with this value set is known as a "reminder" and the user can
     be reminded, via e-mail or client-specific notifications, of the note
     when the time is reached or about to be reached.  When a user sets
     a reminder time on a note that has a reminder done time, and that
     reminder time is in the future, then the reminder done time should be
     cleared.  This should happen regardless of any existing reminder time
     that may have previously existed on the note.
    */
    Optional< Timestamp > reminderTime;
    /**
    Allows the user to assign a human-readable location name associated
     with a note. Users may assign values like 'Home' and 'Work'. Place
     names may also be populated with values from geonames database
     (e.g., a restaurant name). Applications are encouraged to normalize values
     so that grouping values by place name provides a useful result. Applications
     MUST NOT automatically add place name values based on geolocation without
     confirmation from the user; that is, the value in this field should be
     more useful than a simple automated lookup based on the note's latitude
     and longitude.
    */
    Optional< QString > placeName;
    /**
    The class (or type) of note. This field is used to indicate to
     clients that special structured information is represented within
     the note such that special rules apply when making
     modifications. If contentClass is set and the client
     application does not specifically support the specified class,
     the client MUST treat the note as read-only. In this case, the
     client MAY modify the note's notebook and tags via the
     Note.notebookGuid and Note.tagGuids fields.  The client MAY also
     modify the reminderOrder field as well as the reminderTime and
     reminderDoneTime fields.
     <p>Applications should set contentClass only when they are creating notes
     that contain structured information that needs to be maintained in order
     for the user to be able to use the note within that application.
     Setting contentClass makes a note read-only in other applications, so
     there is a trade-off when an application chooses to use contentClass.
     Applications that set contentClass when creating notes must use a contentClass
     string of the form <i>CompanyName.ApplicationName</i> to ensure uniqueness.</p>
     Length restrictions: EDAM_NOTE_CONTENT_CLASS_LEN_MIN, EDAM_NOTE_CONTENT_CLASS_LEN_MAX
     <br/>
     Regex: EDAM_NOTE_CONTENT_CLASS_REGEX
    */
    Optional< QString > contentClass;
    /**
    Provides a location for applications to store a relatively small
     (4kb) blob of data that is not meant to be visible to the user and
     that is opaque to the Evernote service. A single application may use at most
     one entry in this map, using its API consumer key as the map key. See the
     documentation for LazyMap for a description of when the actual map values
     are returned by the service.
     <p>To safely add or modify your application's entry in the map, use
     NoteStore.setNoteApplicationDataEntry. To safely remove your application's
     entry from the map, use NoteStore.unsetNoteApplicationDataEntry.</p>
     Minimum length of a name (key): EDAM_APPLICATIONDATA_NAME_LEN_MIN
     <br/>
     Sum max size of key and value: EDAM_APPLICATIONDATA_ENTRY_LEN_MAX
     <br/>
     Syntax regex for name (key): EDAM_APPLICATIONDATA_NAME_REGEX
    */
    Optional< LazyMap > applicationData;
    /**
    An indication of who made the last change to the note.  If you are
     accessing the note via a shared notebook to which you have modification
     rights, or if you are the owner of the notebook to which the note belongs,
     then you have access to the value.  In this case, the value will be
     unset if the owner of the notebook containing the note was the last to
     make the modification, else it will be a string describing the
     guest who made the last edit.  If you do not have access to this value,
     it will be left unset.  This field is read-only by clients.  The server
     will ignore all values set by clients into this field.
    */
    Optional< QString > lastEditedBy;
    /**
    A map of classifications applied to the note by clients or by the
     Evernote service. The key is the string name of the classification type,
     and the value is a constant that begins with CLASSIFICATION_.
    */
    Optional< QMap< QString, QString > > classifications;
    /**
    The numeric user ID of the user who originally created the note.
    */
    Optional< UserID > creatorId;
    /**
    The numeric user ID of the user described in lastEditedBy.
    */
    Optional< UserID > lastEditorId;

    bool operator==(const NoteAttributes& other) const
    {
        return subjectDate.isEqual(other.subjectDate)
            && latitude.isEqual(other.latitude)
            && longitude.isEqual(other.longitude)
            && altitude.isEqual(other.altitude)
            && author.isEqual(other.author)
            && source.isEqual(other.source)
            && sourceURL.isEqual(other.sourceURL)
            && sourceApplication.isEqual(other.sourceApplication)
            && shareDate.isEqual(other.shareDate)
            && reminderOrder.isEqual(other.reminderOrder)
            && reminderDoneTime.isEqual(other.reminderDoneTime)
            && reminderTime.isEqual(other.reminderTime)
            && placeName.isEqual(other.placeName)
            && contentClass.isEqual(other.contentClass)
            && applicationData.isEqual(other.applicationData)
            && lastEditedBy.isEqual(other.lastEditedBy)
            && classifications.isEqual(other.classifications)
            && creatorId.isEqual(other.creatorId)
            && lastEditorId.isEqual(other.lastEditorId)
        ;
    }

    bool operator!=(const NoteAttributes& other) const
    {
        return !(*this == other);
    }

};

/**
 * Represents a single note in the user's account.
 *
 * */
struct Note {
    /**
    The unique identifier of this note.  Will be set by the
       server, but will be omitted by clients calling NoteStore.createNote()
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > guid;
    /**
    The subject of the note.  Can't begin or end with a space.
       <br/>
       Length:  EDAM_NOTE_TITLE_LEN_MIN - EDAM_NOTE_TITLE_LEN_MAX
       <br/>
       Regex:  EDAM_NOTE_TITLE_REGEX
    */
    Optional< QString > title;
    /**
    The XHTML block that makes up the note.  This is
       the canonical form of the note's contents, so will include abstract
       Evernote tags for internal resource references.  A client may create
       a separate transformed version of this content for internal presentation,
       but the same canonical bytes should be used for transmission and
       comparison unless the user chooses to modify their content.
       <br/>
       Length:  EDAM_NOTE_CONTENT_LEN_MIN - EDAM_NOTE_CONTENT_LEN_MAX
    */
    Optional< QString > content;
    /**
    The binary MD5 checksum of the UTF-8 encoded content
       body. This will always be set by the server, but clients may choose to omit
       this when they submit a note with content.
       <br/>
       Length:  EDAM_HASH_LEN (exactly)
    */
    Optional< QByteArray > contentHash;
    /**
    The number of Unicode characters in the content of
       the note.  This will always be set by the service, but clients may choose
       to omit this value when they submit a Note.
    */
    Optional< qint32 > contentLength;
    /**
    The date and time when the note was created in one of the
       clients.  In most cases, this will match the user's sense of when
       the note was created, and ordering between notes will be based on
       ordering of this field.  However, this is not a "reliable" timestamp
       if a client has an incorrect clock, so it cannot provide a true absolute
       ordering between notes.  Notes created directly through the service
       (e.g. via the web GUI) will have an absolutely ordered "created" value.
    */
    Optional< Timestamp > created;
    /**
    The date and time when the note was last modified in one of
       the clients.  In most cases, this will match the user's sense of when
       the note was modified, but this field may not be absolutely reliable
       due to the possibility of client clock errors.
    */
    Optional< Timestamp > updated;
    /**
    If present, the note is considered "deleted", and this
       stores the date and time when the note was deleted by one of the clients.
       In most cases, this will match the user's sense of when the note was
       deleted, but this field may be unreliable due to the possibility of
       client clock errors.
    */
    Optional< Timestamp > deleted;
    /**
    If the note is available for normal actions and viewing,
       this flag will be set to true.
    */
    Optional< bool > active;
    /**
    A number identifying the last transaction to
       modify the state of this note (including changes to the note's attributes
       or resources).  The USN values are sequential within an account,
       and can be used to compare the order of modifications within the service.
    */
    Optional< qint32 > updateSequenceNum;
    /**
    The unique identifier of the notebook that contains
       this note.  If no notebookGuid is provided on a call to createNote(), the
       default notebook will be used instead.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< QString > notebookGuid;
    /**
    A list of the GUID identifiers for tags that are applied to this note.
       This may be provided in a call to createNote() to unambiguously declare
       the tags that should be assigned to the new note.  Alternately, clients
       may pass the names of desired tags via the 'tagNames' field during
       note creation.
       If the list of tags are omitted on a call to createNote(), then
       the server will assume that no changes have been made to the resources.
       Maximum:  EDAM_NOTE_TAGS_MAX tags per note
    */
    Optional< QList< Guid > > tagGuids;
    /**
    The list of resources that are embedded within this note.
       If the list of resources are omitted on a call to updateNote(), then
       the server will assume that no changes have been made to the resources.
       The binary contents of the resources must be provided when the resource
       is first sent to the service, but it will be omitted by the service when
       the Note is returned in the future.
       Maximum:  EDAM_NOTE_RESOURCES_MAX resources per note
    */
    Optional< QList< Resource > > resources;
    /**
    A list of the attributes for this note.
       If the list of attributes are omitted on a call to updateNote(), then
       the server will assume that no changes have been made to the resources.
    */
    Optional< NoteAttributes > attributes;
    /**
    May be provided by clients during calls to createNote() as an
       alternative to providing the tagGuids of existing tags.  If any tagNames
       are provided during createNote(), these will be found, or created if they
       don't already exist.  Created tags will have no parent (they will be at
       the top level of the tag panel).
    */
    Optional< QStringList > tagNames;

    bool operator==(const Note& other) const
    {
        return guid.isEqual(other.guid)
            && title.isEqual(other.title)
            && content.isEqual(other.content)
            && contentHash.isEqual(other.contentHash)
            && contentLength.isEqual(other.contentLength)
            && created.isEqual(other.created)
            && updated.isEqual(other.updated)
            && deleted.isEqual(other.deleted)
            && active.isEqual(other.active)
            && updateSequenceNum.isEqual(other.updateSequenceNum)
            && notebookGuid.isEqual(other.notebookGuid)
            && tagGuids.isEqual(other.tagGuids)
            && resources.isEqual(other.resources)
            && attributes.isEqual(other.attributes)
            && tagNames.isEqual(other.tagNames)
        ;
    }

    bool operator!=(const Note& other) const
    {
        return !(*this == other);
    }

};

/**
 * If a Notebook has been opened to the public, the Notebook will have a
 * reference to one of these structures, which gives the location and optional
 * description of the externally-visible public Notebook.
 * */
struct Publishing {
    /**
    If this field is present, then the notebook is published for
       mass consumption on the Internet under the provided URI, which is
       relative to a defined base publishing URI defined by the service.
       This field can only be modified via the web service GUI ... publishing
       cannot be modified via an offline client.
       <br/>
       Length:  EDAM_PUBLISHING_URI_LEN_MIN - EDAM_PUBLISHING_URI_LEN_MAX
       <br/>
       Regex:  EDAM_PUBLISHING_URI_REGEX
    */
    Optional< QString > uri;
    /**
    When the notes are publicly displayed, they will be sorted
       based on the requested criteria.
    */
    Optional< NoteSortOrder::type > order;
    /**
    If this is set to true, then the public notes will be
       displayed in ascending order (e.g. from oldest to newest).  Otherwise,
       the notes will be displayed in descending order (e.g. newest to oldest).
    */
    Optional< bool > ascending;
    /**
    This field may be used to provide a short
       description of the notebook, which may be displayed when (e.g.) the
       notebook is shown in a public view.  Can't begin or end with a space.
       <br/>
       Length:  EDAM_PUBLISHING_DESCRIPTION_LEN_MIN -
                EDAM_PUBLISHING_DESCRIPTION_LEN_MAX
       <br/>
       Regex:  EDAM_PUBLISHING_DESCRIPTION_REGEX
    */
    Optional< QString > publicDescription;

    bool operator==(const Publishing& other) const
    {
        return uri.isEqual(other.uri)
            && order.isEqual(other.order)
            && ascending.isEqual(other.ascending)
            && publicDescription.isEqual(other.publicDescription)
        ;
    }

    bool operator!=(const Publishing& other) const
    {
        return !(*this == other);
    }

};

/**
 * If a Notebook contained in an Evernote Business account has been published
 * the to business library, the Notebook will have a reference to one of these
 * structures, which specifies how the Notebook will be represented in the
 * library.
 *
 * */
struct BusinessNotebook {
    /**
    A short description of the notebook's content that will be displayed
           in the business library user interface. The description may not begin
           or end with whitespace.
       <br/>
       Length: EDAM_BUSINESS_NOTEBOOK_DESCRIPTION_LEN_MIN -
               EDAM_BUSINESS_NOTEBOOK_DESCRIPTION_LEN_MAX
       <br/>
       Regex:  EDAM_BUSINESS_NOTEBOOK_DESCRIPTION_REGEX
    */
    Optional< QString > notebookDescription;
    /**
    The privileges that will be granted to users who join the notebook through
           the business library.
    */
    Optional< SharedNotebookPrivilegeLevel::type > privilege;
    /**
    Whether the notebook should be "recommended" when displayed in the business
           library user interface.
    */
    Optional< bool > recommended;

    bool operator==(const BusinessNotebook& other) const
    {
        return notebookDescription.isEqual(other.notebookDescription)
            && privilege.isEqual(other.privilege)
            && recommended.isEqual(other.recommended)
        ;
    }

    bool operator!=(const BusinessNotebook& other) const
    {
        return !(*this == other);
    }

};

/**
 * A structure defining the scope of a SavedSearch.
 *
 * */
struct SavedSearchScope {
    /**
    The search should include notes from the account that contains the SavedSearch.
    */
    Optional< bool > includeAccount;
    /**
    The search should include notes within those shared notebooks
       that the user has joined that are NOT business notebooks.
    */
    Optional< bool > includePersonalLinkedNotebooks;
    /**
    The search should include notes within those shared notebooks
       that the user has joined that are business notebooks in the business that
       the user is currently a member of.
    */
    Optional< bool > includeBusinessLinkedNotebooks;

    bool operator==(const SavedSearchScope& other) const
    {
        return includeAccount.isEqual(other.includeAccount)
            && includePersonalLinkedNotebooks.isEqual(other.includePersonalLinkedNotebooks)
            && includeBusinessLinkedNotebooks.isEqual(other.includeBusinessLinkedNotebooks)
        ;
    }

    bool operator!=(const SavedSearchScope& other) const
    {
        return !(*this == other);
    }

};

/**
 * A named search associated with the account that can be quickly re-used.
 * */
struct SavedSearch {
    /**
    The unique identifier of this search.  Will be set by the
       service, so may be omitted by the client when creating.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > guid;
    /**
    The name of the saved search to display in the GUI.  The
       account may only contain one search with a given name (case-insensitive
       compare). Can't begin or end with a space.
       <br/>
       Length:  EDAM_SAVED_SEARCH_NAME_LEN_MIN - EDAM_SAVED_SEARCH_NAME_LEN_MAX
       <br/>
       Regex:  EDAM_SAVED_SEARCH_NAME_REGEX
    */
    Optional< QString > name;
    /**
    A string expressing the search to be performed.
       <br/>
       Length:  EDAM_SAVED_SEARCH_QUERY_LEN_MIN - EDAM_SAVED_SEARCH_QUERY_LEN_MAX
    */
    Optional< QString > query;
    /**
    The format of the query string, to determine how to parse
       and process it.
    */
    Optional< QueryFormat::type > format;
    /**
    A number identifying the last transaction to
       modify the state of this object.  The USN values are sequential within an
       account, and can be used to compare the order of modifications within the
       service.
    */
    Optional< qint32 > updateSequenceNum;
    /**
    <p>Specifies the set of notes that should be included in the search, if
        possible.</p>
        <p>Clients are expected to search as much of the desired scope as possible,
        with the understanding that a given client may not be able to cover the full
        specified scope. For example, when executing a search that includes notes in both
        the owner's account and business notebooks, a mobile client may choose to only
        search within the user's account because it is not capable of searching both
        scopes simultaneously. When a search across multiple scopes is not possible,
        a client may choose which scope to search based on the current application
        context. If a client cannot search any of the desired scopes, it should refuse
        to execute the search.</p>
    */
    Optional< SavedSearchScope > scope;

    bool operator==(const SavedSearch& other) const
    {
        return guid.isEqual(other.guid)
            && name.isEqual(other.name)
            && query.isEqual(other.query)
            && format.isEqual(other.format)
            && updateSequenceNum.isEqual(other.updateSequenceNum)
            && scope.isEqual(other.scope)
        ;
    }

    bool operator!=(const SavedSearch& other) const
    {
        return !(*this == other);
    }

};

/**
 * Settings meant for the recipient of a shared notebook, such as
 * for indicating which types of notifications the recipient wishes
 * for reminders, etc.
 *
 * The reminderNotifyEmail and reminderNotifyInApp fields have a
 * 3-state read value but a 2-state write value.  On read, it is
 * possible to observe "unset", true, or false.  The initial state is
 * "unset".  When you choose to set a value, you may set it to either
 * true or false, but you cannot unset the value.  Once one of these
 * members has a true/false value, it will always have a true/false
 * value.
 *
 * */
struct SharedNotebookRecipientSettings {
    /**
    Indicates that the user wishes to receive daily e-mail notifications
         for reminders associated with the shared notebook.  This may be
         true only for business notebooks that belong to the business of
         which the user is a member.  You may only set this value on a
         notebook in your business.
    */
    Optional< bool > reminderNotifyEmail;
    /**
    Indicates that the user wishes to receive notifications for
         reminders by applications that support providing such
         notifications.  The exact nature of the notification is defined
         by the individual applications.
    */
    Optional< bool > reminderNotifyInApp;

    bool operator==(const SharedNotebookRecipientSettings& other) const
    {
        return reminderNotifyEmail.isEqual(other.reminderNotifyEmail)
            && reminderNotifyInApp.isEqual(other.reminderNotifyInApp)
        ;
    }

    bool operator!=(const SharedNotebookRecipientSettings& other) const
    {
        return !(*this == other);
    }

};

/**
 * Shared notebooks represent a relationship between a notebook and a single
 * share invitation recipient.
 * */
struct SharedNotebook {
    /**
    the primary identifier of the share
    */
    Optional< qint64 > id;
    /**
    the user id of the owner of the notebook
    */
    Optional< qint32 > userId;
    /**
    the GUID of the associated notebook shared.
    */
    Optional< QString > notebookGuid;
    /**
    the email address of the recipient - used by the notebook
     owner to identify who they shared with.
    */
    Optional< QString > email;
    /**
    (DEPRECATED) a flag indicating the share is read/write -otherwise it's read
         only.  This field is deprecated in favor of the new "privilege" field.
    */
    Optional< bool > notebookModifiable;
    /**
    (DEPRECATED) indicates that a user must login to access the share.  This
         field is deprecated and will be "true" for all new shared notebooks.  It
         is read-only and ignored when creating or modifying a shared notebook,
         except that a shared notebook can be modified to require login.
         See "allowPreview" for information on privileges and shared notebooks.
    */
    Optional< bool > requireLogin;
    /**
    the date the owner first created the share with the specific email
       address
    */
    Optional< Timestamp > serviceCreated;
    /**
    the date the shared notebook was last updated on the service.  This
         will be updated when authenticateToSharedNotebook is called the first
         time with a shared notebook requiring login (i.e. when the username is
         bound to that shared notebook).
    */
    Optional< Timestamp > serviceUpdated;
    /** NOT DOCUMENTED */
    Optional< QString > shareKey;
    /**
    the username of the user who can access this share.
       Once it's assigned it cannot be changed.
    */
    Optional< QString > username;
    /**
    The privilege level granted to the notebook, activity stream, and
         invitations.  See the corresponding enumeration for details.
    */
    Optional< SharedNotebookPrivilegeLevel::type > privilege;
    /**
    Whether or not to grant "READ_NOTEBOOK" privilege without an
         authentication token, for authenticateToSharedNotebook(...).  With
         the change to "requireLogin" always being true for new shared
         notebooks, this is the only way to access a shared notebook without
         an authorization token.  This setting expires after the first use
         of authenticateToSharedNotebook(...) with a valid authentication
         token.
    */
    Optional< bool > allowPreview;
    /**
    Settings intended for use only by the recipient of this shared
         notebook.  You should skip setting this value unless you want
         to change the value contained inside the structure, and only if
         you are the recipient.
    */
    Optional< SharedNotebookRecipientSettings > recipientSettings;

    bool operator==(const SharedNotebook& other) const
    {
        return id.isEqual(other.id)
            && userId.isEqual(other.userId)
            && notebookGuid.isEqual(other.notebookGuid)
            && email.isEqual(other.email)
            && notebookModifiable.isEqual(other.notebookModifiable)
            && requireLogin.isEqual(other.requireLogin)
            && serviceCreated.isEqual(other.serviceCreated)
            && serviceUpdated.isEqual(other.serviceUpdated)
            && shareKey.isEqual(other.shareKey)
            && username.isEqual(other.username)
            && privilege.isEqual(other.privilege)
            && allowPreview.isEqual(other.allowPreview)
            && recipientSettings.isEqual(other.recipientSettings)
        ;
    }

    bool operator!=(const SharedNotebook& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure captures information about the types of operations
 * that cannot be performed on a given notebook with a type of
 * authenticated access and credentials.  The values filled into this
 * structure are based on then-current values in the server database
 * for shared notebooks and notebook publishing records, as well as
 * information related to the authentication token.  Information from
 * the authentication token includes the application that is accessing
 * the server, as defined by the permissions granted by consumer (api)
 * key, and the method used to obtain the token, for example via
 * authenticateToSharedNotebook, authenticateToBusiness, etc.  Note
 * that changes to values in this structure that are the result of
 * shared notebook or publishing record changes are communicated to
 * the client via a change in the notebook USN during sync.  It is
 * important to use the same access method, parameters, and consumer
 * key in order obtain correct results from the sync engine.
 *
 * The server has the final say on what is allowed as values may
 * change between calls to obtain NotebookRestrictions instances
 * and to operate on data on the service.
 *
 * If the following are set and true, then the given restriction is
 * in effect, as accessed by the same authentication token from which
 * the values were obtained.
 *
 * */
struct NotebookRestrictions {
    /**
    The client is not able to read notes from the service and
       the notebook is write-only.
    */
    Optional< bool > noReadNotes;
    /**
    The client may not create new notes in the notebook.
    */
    Optional< bool > noCreateNotes;
    /**
    The client may not update notes currently in the notebook.
    */
    Optional< bool > noUpdateNotes;
    /**
    The client may not expunge notes currently in the notebook.
    */
    Optional< bool > noExpungeNotes;
    /**
    The client may not share notes in the notebook via the
       shareNote method.
    */
    Optional< bool > noShareNotes;
    /**
    The client may not e-mail notes via the Evernote service by
       using the emailNote method.
    */
    Optional< bool > noEmailNotes;
    /**
    The client may not send messages to the share recipients of
       the notebook.
    */
    Optional< bool > noSendMessageToRecipients;
    /**
    The client may not update the Notebook object itself, for
       example, via the updateNotebook method.
    */
    Optional< bool > noUpdateNotebook;
    /**
    The client may not expunge the Notebook object itself, for
       example, via the expungeNotebook method.
    */
    Optional< bool > noExpungeNotebook;
    /**
    The client may not set this notebook to be the default notebook.
       The caller should leave Notebook.defaultNotebook unset.
    */
    Optional< bool > noSetDefaultNotebook;
    /**
    If the client is able to update the Notebook, the Notebook.stack
       value may not be set.
    */
    Optional< bool > noSetNotebookStack;
    /**
    The client may not change the publish the notebook to the public.
       For example, business notebooks may not be shared publicly.
    */
    Optional< bool > noPublishToPublic;
    /**
    The client may not publish the notebook to the business library.
    */
    Optional< bool > noPublishToBusinessLibrary;
    /**
    The client may not complete an operation that results in a new tag
       being created in the owner's account.
    */
    Optional< bool > noCreateTags;
    /**
    The client may not update tags in the owner's account.
    */
    Optional< bool > noUpdateTags;
    /**
    The client may not expunge tags in the owner's account.
    */
    Optional< bool > noExpungeTags;
    /**
    If the client is able to create or update tags in the owner's account,
       then they will not be able to set the parent tag.  Leave the value unset.
    */
    Optional< bool > noSetParentTag;
    /**
    The client is unable to create shared notebooks for the notebook.
    */
    Optional< bool > noCreateSharedNotebooks;
    /**
    Restrictions on which shared notebook instances can be updated.  If the
       value is not set or null, then the client can update any of the shared notebooks
       associated with the notebook on which the NotebookRestrictions are defined.
       See the enumeration for further details.
    */
    Optional< SharedNotebookInstanceRestrictions::type > updateWhichSharedNotebookRestrictions;
    /**
    Restrictions on which shared notebook instances can be expunged.  If the
       value is not set or null, then the client can expunge any of the shared notebooks
       associated with the notebook on which the NotebookRestrictions are defined.
       See the enumeration for further details.
    */
    Optional< SharedNotebookInstanceRestrictions::type > expungeWhichSharedNotebookRestrictions;

    bool operator==(const NotebookRestrictions& other) const
    {
        return noReadNotes.isEqual(other.noReadNotes)
            && noCreateNotes.isEqual(other.noCreateNotes)
            && noUpdateNotes.isEqual(other.noUpdateNotes)
            && noExpungeNotes.isEqual(other.noExpungeNotes)
            && noShareNotes.isEqual(other.noShareNotes)
            && noEmailNotes.isEqual(other.noEmailNotes)
            && noSendMessageToRecipients.isEqual(other.noSendMessageToRecipients)
            && noUpdateNotebook.isEqual(other.noUpdateNotebook)
            && noExpungeNotebook.isEqual(other.noExpungeNotebook)
            && noSetDefaultNotebook.isEqual(other.noSetDefaultNotebook)
            && noSetNotebookStack.isEqual(other.noSetNotebookStack)
            && noPublishToPublic.isEqual(other.noPublishToPublic)
            && noPublishToBusinessLibrary.isEqual(other.noPublishToBusinessLibrary)
            && noCreateTags.isEqual(other.noCreateTags)
            && noUpdateTags.isEqual(other.noUpdateTags)
            && noExpungeTags.isEqual(other.noExpungeTags)
            && noSetParentTag.isEqual(other.noSetParentTag)
            && noCreateSharedNotebooks.isEqual(other.noCreateSharedNotebooks)
            && updateWhichSharedNotebookRestrictions.isEqual(other.updateWhichSharedNotebookRestrictions)
            && expungeWhichSharedNotebookRestrictions.isEqual(other.expungeWhichSharedNotebookRestrictions)
        ;
    }

    bool operator!=(const NotebookRestrictions& other) const
    {
        return !(*this == other);
    }

};

/**
 * A unique container for a set of notes.
 * */
struct Notebook {
    /**
    The unique identifier of this notebook.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > guid;
    /**
    A sequence of characters representing the name of the
       notebook.  May be changed by clients, but the account may not contain two
       notebooks with names that are equal via a case-insensitive comparison.
       Can't begin or end with a space.
       <br/>
       Length:  EDAM_NOTEBOOK_NAME_LEN_MIN - EDAM_NOTEBOOK_NAME_LEN_MAX
       <br/>
       Regex:  EDAM_NOTEBOOK_NAME_REGEX
    */
    Optional< QString > name;
    /**
    A number identifying the last transaction to
       modify the state of this object.  The USN values are sequential within an
       account, and can be used to compare the order of modifications within the
       service.
    */
    Optional< qint32 > updateSequenceNum;
    /**
    If true, this notebook should be used for new notes
       whenever the user has not (or cannot) specify a desired target notebook.
       For example, if a note is submitted via SMTP email.
       The service will maintain at most one defaultNotebook per account.
       If a second notebook is created or updated with defaultNotebook set to
       true, the service will automatically update the prior notebook's
       defaultNotebook field to false.  If the default notebook is deleted
       (i.e. "active" set to false), the "defaultNotebook" field will be
       set to false by the service.  If the account has no default notebook
       set, the service will use the most recent notebook as the default.
    */
    Optional< bool > defaultNotebook;
    /**
    The time when this notebook was created on the
       service. This will be set on the service during creation, and the service
       will provide this value when it returns a Notebook to a client.
       The service will ignore this value if it is sent by clients.
    */
    Optional< Timestamp > serviceCreated;
    /**
    The time when this notebook was last modified on the
       service.  This will be set on the service during creation, and the service
       will provide this value when it returns a Notebook to a client.
       The service will ignore this value if it is sent by clients.
    */
    Optional< Timestamp > serviceUpdated;
    /**
    If the Notebook has been opened for public access, or
       business users shared with their business (i.e. if 'published' is
       set to true), then this will point to the set of publishing
       information for the Notebook (URI, description, etc.).  A
       Notebook cannot be published without providing this information,
       but it will persist for later use if publishing is ever disabled
       on the Notebook.  Clients that do not wish to change the
       publishing behavior of a Notebook should not set this value when
       calling NoteStore.updateNotebook().
    */
    Optional< Publishing > publishing;
    /**
    If this is set to true, then the Notebook will be
       accessible either to the public, or for business users to their business,
       via the 'publishing' specification, which must also be set.  If this is set
       to false, the Notebook will not be available to the public (or business).
       Clients that do not wish to change the publishing behavior of a Notebook
       should not set this value when calling NoteStore.updateNotebook().
    */
    Optional< bool > published;
    /**
    If this is set, then the notebook is visually contained within a stack
       of notebooks with this name.  All notebooks in the same account with the
       same 'stack' field are considered to be in the same stack.
       Notebooks with no stack set are "top level" and not contained within a
       stack.
    */
    Optional< QString > stack;
    /**
    <i>DEPRECATED</i> - replaced by sharedNotebooks.
    */
    Optional< QList< qint64 > > sharedNotebookIds;
    /**
    The list of recipients to whom this notebook has been shared
       (one SharedNotebook object per recipient email address). This field will
       be unset if you do not have permission to access this data. If you are
       accessing the notebook as the owner or via a shared notebook that is
       modifiable, then you have access to this data and the value will be set.
       This field is read-only. Clients may not make changes to shared notebooks
       via this field.
    */
    Optional< QList< SharedNotebook > > sharedNotebooks;
    /**
    If the notebook is part of a business account and has been published to the
       business library, this will contain information for the library listing.
       The presence or absence of this field is not a reliable test of whether a given
       notebook is in fact a business notebook - the field is only used when a notebook is or
       has been published to the business library.
    */
    Optional< BusinessNotebook > businessNotebook;
    /**
    Intended for use with Business accounts, this field identifies the user who
       has been designated as the "contact".  For notebooks created in business
       accounts, the server will automatically set this value to the user who created
       the notebook unless Notebook.contact.username has been set, in which that value
       will be used.  When updating a notebook, it is common to leave Notebook.contact
       field unset, indicating that no change to the value is being requested and that
       the existing value, if any, should be preserved.
    */
    Optional< User > contact;
    /** NOT DOCUMENTED */
    Optional< NotebookRestrictions > restrictions;

    bool operator==(const Notebook& other) const
    {
        return guid.isEqual(other.guid)
            && name.isEqual(other.name)
            && updateSequenceNum.isEqual(other.updateSequenceNum)
            && defaultNotebook.isEqual(other.defaultNotebook)
            && serviceCreated.isEqual(other.serviceCreated)
            && serviceUpdated.isEqual(other.serviceUpdated)
            && publishing.isEqual(other.publishing)
            && published.isEqual(other.published)
            && stack.isEqual(other.stack)
            && sharedNotebookIds.isEqual(other.sharedNotebookIds)
            && sharedNotebooks.isEqual(other.sharedNotebooks)
            && businessNotebook.isEqual(other.businessNotebook)
            && contact.isEqual(other.contact)
            && restrictions.isEqual(other.restrictions)
        ;
    }

    bool operator!=(const Notebook& other) const
    {
        return !(*this == other);
    }

};

/**
 * A link in an users account that refers them to a public or individual share in
 * another user's account.
 *
 * */
struct LinkedNotebook {
    /**
    the display name of the shared notebook.
       The link owner can change this.
    */
    Optional< QString > shareName;
    /**
    the username of the user who owns the shared or public notebook
    */
    Optional< QString > username;
    /**
    the shard ID of the notebook if the notebook is not public
    
     <dt>shareKey
     the secret key that provides access to the shared notebook
    */
    Optional< QString > shardId;
    /** NOT DOCUMENTED */
    Optional< QString > shareKey;
    /**
    the identifier of the public notebook
    */
    Optional< QString > uri;
    /**
    The unique identifier of this linked notebook.  Will be set whenever
       a linked notebook is retrieved from the service, but may be null when a client
       is creating a linked notebook.
       <br/>
       Length:  EDAM_GUID_LEN_MIN - EDAM_GUID_LEN_MAX
       <br/>
       Regex:  EDAM_GUID_REGEX
    */
    Optional< Guid > guid;
    /**
    A number identifying the last transaction to
       modify the state of this object.  The USN values are sequential within an
       account, and can be used to compare the order of modifications within the
       service.
    */
    Optional< qint32 > updateSequenceNum;
    /**
    This field will contain the full URL that clients should use to make
       NoteStore requests to the server shard that contains that notebook's data.
       I.e. this is the URL that should be used to create the Thrift HTTP client
       transport to send messages to the NoteStore service for the account.
    */
    Optional< QString > noteStoreUrl;
    /**
    This field will contain the initial part of the URLs that should be used
       to make requests to Evernote's thin client "web API", which provide
       optimized operations for clients that aren't capable of manipulating
       the full contents of accounts via the full Thrift data model. Clients
       should concatenate the relative path for the various servlets onto the
       end of this string to construct the full URL, as documented on our
       developer web site.
    */
    Optional< QString > webApiUrlPrefix;
    /**
    If this is set, then the notebook is visually contained within a stack
       of notebooks with this name.  All notebooks in the same account with the
       same 'stack' field are considered to be in the same stack.
       Notebooks with no stack set are "top level" and not contained within a
       stack.  The link owner can change this and this field is for the benefit
       of the link owner.
    */
    Optional< QString > stack;
    /**
    If set, this will be the unique identifier for the business that owns
       the notebook to which the linked notebook refers.
    */
    Optional< qint32 > businessId;

    bool operator==(const LinkedNotebook& other) const
    {
        return shareName.isEqual(other.shareName)
            && username.isEqual(other.username)
            && shardId.isEqual(other.shardId)
            && shareKey.isEqual(other.shareKey)
            && uri.isEqual(other.uri)
            && guid.isEqual(other.guid)
            && updateSequenceNum.isEqual(other.updateSequenceNum)
            && noteStoreUrl.isEqual(other.noteStoreUrl)
            && webApiUrlPrefix.isEqual(other.webApiUrlPrefix)
            && stack.isEqual(other.stack)
            && businessId.isEqual(other.businessId)
        ;
    }

    bool operator!=(const LinkedNotebook& other) const
    {
        return !(*this == other);
    }

};

/**
 * A structure that describes a notebook or a user's relationship with
 * a notebook. NotebookDescriptor is expected to remain a lighter-weight
 * structure when compared to Notebook.
 * */
struct NotebookDescriptor {
    /**
    The unique identifier of the notebook.
    */
    Optional< Guid > guid;
    /**
    A sequence of characters representing the name of the
       notebook.
    */
    Optional< QString > notebookDisplayName;
    /**
    The User.name value of the notebook's "contact".
    */
    Optional< QString > contactName;
    /**
    Whether a SharedNotebook record exists between the calling user and this
       notebook.
    */
    Optional< bool > hasSharedNotebook;
    /**
    The number of users who have joined this notebook.
    */
    Optional< qint32 > joinedUserCount;

    bool operator==(const NotebookDescriptor& other) const
    {
        return guid.isEqual(other.guid)
            && notebookDisplayName.isEqual(other.notebookDisplayName)
            && contactName.isEqual(other.contactName)
            && hasSharedNotebook.isEqual(other.hasSharedNotebook)
            && joinedUserCount.isEqual(other.joinedUserCount)
        ;
    }

    bool operator!=(const NotebookDescriptor& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is used to provide publicly-available user information
 * about a particular account.
 **/
struct PublicUserInfo {
    /**
    The unique numeric user identifier for the user account.
    */
    UserID userId;
    /**
    DEPRECATED - Client applications should have no need to use this field.
    */
    QString shardId;
    /**
    The privilege level of the account, to determine whether
       this is a Premium or Free account.
    */
    Optional< PrivilegeLevel::type > privilege;
    /** NOT DOCUMENTED */
    Optional< QString > username;
    /**
    This field will contain the full URL that clients should use to make
       NoteStore requests to the server shard that contains that user's data.
       I.e. this is the URL that should be used to create the Thrift HTTP client
       transport to send messages to the NoteStore service for the account.
    */
    Optional< QString > noteStoreUrl;
    /**
    This field will contain the initial part of the URLs that should be used
       to make requests to Evernote's thin client "web API", which provide
       optimized operations for clients that aren't capable of manipulating
       the full contents of accounts via the full Thrift data model. Clients
       should concatenate the relative path for the various servlets onto the
       end of this string to construct the full URL, as documented on our
       developer web site.
    */
    Optional< QString > webApiUrlPrefix;

    bool operator==(const PublicUserInfo& other) const
    {
        return (userId == other.userId)
            && (shardId == other.shardId)
            && privilege.isEqual(other.privilege)
            && username.isEqual(other.username)
            && noteStoreUrl.isEqual(other.noteStoreUrl)
            && webApiUrlPrefix.isEqual(other.webApiUrlPrefix)
        ;
    }

    bool operator!=(const PublicUserInfo& other) const
    {
        return !(*this == other);
    }

};

/**
 * When an authentication (or re-authentication) is performed, this structure
 * provides the result to the client.
 **/
struct AuthenticationResult {
    /**
    The server-side date and time when this result was
       generated.
    */
    Timestamp currentTime;
    /**
    Holds an opaque, ASCII-encoded token that can be
       used by the client to perform actions on a NoteStore.
    */
    QString authenticationToken;
    /**
    Holds the server-side date and time when the
       authentication token will expire.
       This time can be compared to "currentTime" to produce an expiration
       time that can be reconciled with the client's local clock.
    */
    Timestamp expiration;
    /**
    Holds the information about the account which was
       authenticated if this was a full authentication.  May be absent if this
       particular authentication did not require user information.
    */
    Optional< User > user;
    /**
    If this authentication result was achieved without full permissions to
       access the full User structure, this field may be set to give back
       a more limited public set of data.
    */
    Optional< PublicUserInfo > publicUserInfo;
    /**
    This field will contain the full URL that clients should use to make
       NoteStore requests to the server shard that contains that user's data.
       I.e. this is the URL that should be used to create the Thrift HTTP client
       transport to send messages to the NoteStore service for the account.
    */
    Optional< QString > noteStoreUrl;
    /**
    This field will contain the initial part of the URLs that should be used
       to make requests to Evernote's thin client "web API", which provide
       optimized operations for clients that aren't capable of manipulating
       the full contents of accounts via the full Thrift data model. Clients
       should concatenate the relative path for the various servlets onto the
       end of this string to construct the full URL, as documented on our
       developer web site.
    */
    Optional< QString > webApiUrlPrefix;
    /**
    If set to true, this field indicates that the user has enabled two-factor
       authentication and must enter their second factor in order to complete 
       authentication. In this case the value of authenticationResult will be
       a short-lived authentication token that may only be used to make a 
       subsequent call to completeTwoFactorAuthentication.
    */
    Optional< bool > secondFactorRequired;
    /**
    When secondFactorRequired is set to true, this field may contain a string
       describing the second factor delivery method that the user has configured. 
       This will typically be an obfuscated mobile device number, such as 
       "(xxx) xxx-x095". This string can be displayed to the user to remind them
       how to obtain the required second factor.
       TODO do we need to differentiate between SMS and voice delivery?
    */
    Optional< QString > secondFactorDeliveryHint;

    bool operator==(const AuthenticationResult& other) const
    {
        return (currentTime == other.currentTime)
            && (authenticationToken == other.authenticationToken)
            && (expiration == other.expiration)
            && user.isEqual(other.user)
            && publicUserInfo.isEqual(other.publicUserInfo)
            && noteStoreUrl.isEqual(other.noteStoreUrl)
            && webApiUrlPrefix.isEqual(other.webApiUrlPrefix)
            && secondFactorRequired.isEqual(other.secondFactorRequired)
            && secondFactorDeliveryHint.isEqual(other.secondFactorDeliveryHint)
        ;
    }

    bool operator!=(const AuthenticationResult& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure describes a collection of bootstrap settings.
 **/
struct BootstrapSettings {
    /**
    The hostname and optional port for composing Evernote web service URLs.
       This URL can be used to access the UserStore and related services,
       but must not be used to compose the NoteStore URL. Client applications
       must handle serviceHost values that include only the hostname
       (e.g. www.evernote.com) or both the hostname and port (e.g. www.evernote.com:8080).
       If no port is specified, or if port 443 is specified, client applications must
       use the scheme "https" when composing URLs. Otherwise, a client must use the
       scheme "http".
    */
    QString serviceHost;
    /**
    The URL stem for the Evernote corporate marketing website, e.g. http://www.evernote.com.
       This stem can be used to compose website URLs. For example, the URL of the Evernote
       Trunk is composed by appending "/about/trunk/" to the value of marketingUrl.
    */
    QString marketingUrl;
    /**
    The full URL for the Evernote customer support website, e.g. https://support.evernote.com.
    */
    QString supportUrl;
    /**
    The domain used for an Evernote user's incoming email address, which allows notes to
       be emailed into an account. E.g. m.evernote.com.
    */
    QString accountEmailDomain;
    /**
    Whether the client application should enable sharing of notes on Facebook.
    */
    Optional< bool > enableFacebookSharing;
    /**
    Whether the client application should enable gift subscriptions.
    */
    Optional< bool > enableGiftSubscriptions;
    /**
    Whether the client application should enable in-client creation of support tickets.
    */
    Optional< bool > enableSupportTickets;
    /**
    Whether the client application should enable shared notebooks.
    */
    Optional< bool > enableSharedNotebooks;
    /**
    Whether the client application should enable single note sharing.
    */
    Optional< bool > enableSingleNoteSharing;
    /**
    Whether the client application should enable sponsored accounts.
    */
    Optional< bool > enableSponsoredAccounts;
    /**
    Whether the client application should enable sharing of notes on Twitter.
    */
    Optional< bool > enableTwitterSharing;
    /** NOT DOCUMENTED */
    Optional< bool > enableLinkedInSharing;
    /** NOT DOCUMENTED */
    Optional< bool > enablePublicNotebooks;

    bool operator==(const BootstrapSettings& other) const
    {
        return (serviceHost == other.serviceHost)
            && (marketingUrl == other.marketingUrl)
            && (supportUrl == other.supportUrl)
            && (accountEmailDomain == other.accountEmailDomain)
            && enableFacebookSharing.isEqual(other.enableFacebookSharing)
            && enableGiftSubscriptions.isEqual(other.enableGiftSubscriptions)
            && enableSupportTickets.isEqual(other.enableSupportTickets)
            && enableSharedNotebooks.isEqual(other.enableSharedNotebooks)
            && enableSingleNoteSharing.isEqual(other.enableSingleNoteSharing)
            && enableSponsoredAccounts.isEqual(other.enableSponsoredAccounts)
            && enableTwitterSharing.isEqual(other.enableTwitterSharing)
            && enableLinkedInSharing.isEqual(other.enableLinkedInSharing)
            && enablePublicNotebooks.isEqual(other.enablePublicNotebooks)
        ;
    }

    bool operator!=(const BootstrapSettings& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure describes a collection of bootstrap settings.
 **/
struct BootstrapProfile {
    /**
    The unique name of the profile, which is guaranteed to remain consistent across
       calls to getBootstrapInfo.
    */
    QString name;
    /**
    The settings for this profile.
    */
    BootstrapSettings settings;

    bool operator==(const BootstrapProfile& other) const
    {
        return (name == other.name)
            && (settings == other.settings)
        ;
    }

    bool operator!=(const BootstrapProfile& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure describes a collection of bootstrap profiles.
 **/
struct BootstrapInfo {
    /**
    List of one or more bootstrap profiles, in descending
       preference order.
    */
    QList< BootstrapProfile > profiles;

    bool operator==(const BootstrapInfo& other) const
    {
        return (profiles == other.profiles)
        ;
    }

    bool operator!=(const BootstrapInfo& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is given out by the NoteStore when a client asks to
 * receive the current state of an account.  The client asks for the server's
 * state one chunk at a time in order to allow clients to retrieve the state
 * of a large account without needing to transfer the entire account in
 * a single message.
 *
 * The server always gives SyncChunks using an ascending series of Update
 * Sequence Numbers (USNs).
 *
 **/
struct SyncChunk {
    /**
    The server's current date and time.
    */
    Timestamp currentTime;
    /**
    The highest USN for any of the data objects represented
       in this sync chunk.  If there are no objects in the chunk, this will not be
       set.
    */
    Optional< qint32 > chunkHighUSN;
    /**
    The total number of updates that have been performed in
       the service for this account.  This is equal to the highest USN within the
       account at the point that this SyncChunk was generated.  If updateCount
       and chunkHighUSN are identical, that means that this is the last chunk
       in the account ... there is no more recent information.
    */
    qint32 updateCount;
    /**
    If present, this is a list of non-expunged notes that
       have a USN in this chunk.  This will include notes that are "deleted"
       but not expunged (i.e. in the trash).  The notes will include their list
       of tags and resources, but the note content, resource content, resource
       recognition data and resource alternate data will not be supplied.
    */
    Optional< QList< Note > > notes;
    /**
    If present, this is a list of non-expunged notebooks that
       have a USN in this chunk.  This will include notebooks that are "deleted"
       but not expunged (i.e. in the trash).
    */
    Optional< QList< Notebook > > notebooks;
    /**
    If present, this is a list of the non-expunged tags that have a
       USN in this chunk.
    */
    Optional< QList< Tag > > tags;
    /**
    If present, this is a list of non-expunged searches that
       have a USN in this chunk.
    */
    Optional< QList< SavedSearch > > searches;
    /**
    If present, this is a list of the non-expunged resources
       that have a USN in this chunk.  This will include the metadata for each
       resource, but not its binary contents or recognition data, which must be
       retrieved separately.
    */
    Optional< QList< Resource > > resources;
    /**
    If present, the GUIDs of all of the notes that were
       permanently expunged in this chunk.
    */
    Optional< QList< Guid > > expungedNotes;
    /**
    If present, the GUIDs of all of the notebooks that
       were permanently expunged in this chunk.  When a notebook is expunged,
       this implies that all of its child notes (and their resources) were
       also expunged.
    */
    Optional< QList< Guid > > expungedNotebooks;
    /**
    If present, the GUIDs of all of the tags that were
       permanently expunged in this chunk.
    */
    Optional< QList< Guid > > expungedTags;
    /**
    If present, the GUIDs of all of the saved searches
       that were permanently expunged in this chunk.
    */
    Optional< QList< Guid > > expungedSearches;
    /**
    If present, this is a list of non-expunged LinkedNotebooks that
       have a USN in this chunk.
    */
    Optional< QList< LinkedNotebook > > linkedNotebooks;
    /**
    If present, the GUIDs of all of the LinkedNotebooks
       that were permanently expunged in this chunk.
    */
    Optional< QList< Guid > > expungedLinkedNotebooks;

    bool operator==(const SyncChunk& other) const
    {
        return (currentTime == other.currentTime)
            && chunkHighUSN.isEqual(other.chunkHighUSN)
            && (updateCount == other.updateCount)
            && notes.isEqual(other.notes)
            && notebooks.isEqual(other.notebooks)
            && tags.isEqual(other.tags)
            && searches.isEqual(other.searches)
            && resources.isEqual(other.resources)
            && expungedNotes.isEqual(other.expungedNotes)
            && expungedNotebooks.isEqual(other.expungedNotebooks)
            && expungedTags.isEqual(other.expungedTags)
            && expungedSearches.isEqual(other.expungedSearches)
            && linkedNotebooks.isEqual(other.linkedNotebooks)
            && expungedLinkedNotebooks.isEqual(other.expungedLinkedNotebooks)
        ;
    }

    bool operator!=(const SyncChunk& other) const
    {
        return !(*this == other);
    }

};

/**
 * A small structure for returning a list of notes out of a larger set.
 *
 **/
struct NoteList {
    /**
    The starting index within the overall set of notes.  This
       is also the number of notes that are "before" this list in the set.
    */
    qint32 startIndex;
    /**
    The number of notes in the larger set.  This can be used
       to calculate how many notes are "after" this note in the set.
       (I.e.  remaining = totalNotes - (startIndex + notes.length)  )
    */
    qint32 totalNotes;
    /**
    The list of notes from this range.  The Notes will include all
       metadata (attributes, resources, etc.), but will not include the ENML
       content of the note or the binary contents of any resources.
    */
    QList< Note > notes;
    /**
    If the NoteList was produced using a text based search
       query that included words that are not indexed or searched by the service,
       this will include a list of those ignored words.
    */
    Optional< QStringList > stoppedWords;
    /**
    If the NoteList was produced using a text based search
       query that included viable search words or quoted expressions, this will
       include a list of those words.  Any stopped words will not be included
       in this list.
    */
    Optional< QStringList > searchedWords;
    /**
    Indicates the total number of transactions that have
       been committed within the account.  This reflects (for example) the
       number of discrete additions or modifications that have been made to
       the data in this account (tags, notes, resources, etc.).
       This number is the "high water mark" for Update Sequence Numbers (USN)
       within the account.
    */
    Optional< qint32 > updateCount;

    bool operator==(const NoteList& other) const
    {
        return (startIndex == other.startIndex)
            && (totalNotes == other.totalNotes)
            && (notes == other.notes)
            && stoppedWords.isEqual(other.stoppedWords)
            && searchedWords.isEqual(other.searchedWords)
            && updateCount.isEqual(other.updateCount)
        ;
    }

    bool operator!=(const NoteList& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is used in the set of results returned by the
 * findNotesMetadata function.  It represents the high-level information about
 * a single Note, without some of the larger deep structure.  This allows
 * for the information about a list of Notes to be returned relatively quickly
 * with less marshalling and data transfer to remote clients.
 * Most fields in this structure are identical to the corresponding field in
 * the Note structure, with the exception of:
 *
 * */
struct NoteMetadata {
    /** NOT DOCUMENTED */
    Guid guid;
    /** NOT DOCUMENTED */
    Optional< QString > title;
    /** NOT DOCUMENTED */
    Optional< qint32 > contentLength;
    /** NOT DOCUMENTED */
    Optional< Timestamp > created;
    /** NOT DOCUMENTED */
    Optional< Timestamp > updated;
    /** NOT DOCUMENTED */
    Optional< Timestamp > deleted;
    /** NOT DOCUMENTED */
    Optional< qint32 > updateSequenceNum;
    /** NOT DOCUMENTED */
    Optional< QString > notebookGuid;
    /** NOT DOCUMENTED */
    Optional< QList< Guid > > tagGuids;
    /** NOT DOCUMENTED */
    Optional< NoteAttributes > attributes;
    /**
    If set, then this will contain the MIME type of the largest Resource
       (in bytes) within the Note.  This may be useful, for example, to choose
       an appropriate icon or thumbnail to represent the Note.
    */
    Optional< QString > largestResourceMime;
    /**
    If set, this will contain the size of the largest Resource file, in
      bytes, within the Note.  This may be useful, for example, to decide whether
      to ask the server for a thumbnail to represent the Note.
    */
    Optional< qint32 > largestResourceSize;

    bool operator==(const NoteMetadata& other) const
    {
        return (guid == other.guid)
            && title.isEqual(other.title)
            && contentLength.isEqual(other.contentLength)
            && created.isEqual(other.created)
            && updated.isEqual(other.updated)
            && deleted.isEqual(other.deleted)
            && updateSequenceNum.isEqual(other.updateSequenceNum)
            && notebookGuid.isEqual(other.notebookGuid)
            && tagGuids.isEqual(other.tagGuids)
            && attributes.isEqual(other.attributes)
            && largestResourceMime.isEqual(other.largestResourceMime)
            && largestResourceSize.isEqual(other.largestResourceSize)
        ;
    }

    bool operator!=(const NoteMetadata& other) const
    {
        return !(*this == other);
    }

};

/**
 * This structure is returned from calls to the findNotesMetadata function to
 * give the high-level metadata about a subset of Notes that are found to
 * match a specified NoteFilter in a search.
 *
 **/
struct NotesMetadataList {
    /**
    The starting index within the overall set of notes.  This
       is also the number of notes that are "before" this list in the set.
    */
    qint32 startIndex;
    /**
    The number of notes in the larger set.  This can be used
       to calculate how many notes are "after" this note in the set.
       (I.e.  remaining = totalNotes - (startIndex + notes.length)  )
    */
    qint32 totalNotes;
    /**
    The list of metadata for Notes in this range.  The set of optional fields
       that are set in each metadata structure will depend on the
       NotesMetadataResultSpec provided by the caller when the search was
       performed.  Only the 'guid' field will be guaranteed to be set in each
       Note.
    */
    QList< NoteMetadata > notes;
    /**
    If the NoteList was produced using a text based search
       query that included words that are not indexed or searched by the service,
       this will include a list of those ignored words.
    */
    Optional< QStringList > stoppedWords;
    /**
    If the NoteList was produced using a text based search
       query that included viable search words or quoted expressions, this will
       include a list of those words.  Any stopped words will not be included
       in this list.
    */
    Optional< QStringList > searchedWords;
    /**
    Indicates the total number of transactions that have
       been committed within the account.  This reflects (for example) the
       number of discrete additions or modifications that have been made to
       the data in this account (tags, notes, resources, etc.).
       This number is the "high water mark" for Update Sequence Numbers (USN)
       within the account.
    */
    Optional< qint32 > updateCount;

    bool operator==(const NotesMetadataList& other) const
    {
        return (startIndex == other.startIndex)
            && (totalNotes == other.totalNotes)
            && (notes == other.notes)
            && stoppedWords.isEqual(other.stoppedWords)
            && searchedWords.isEqual(other.searchedWords)
            && updateCount.isEqual(other.updateCount)
        ;
    }

    bool operator!=(const NotesMetadataList& other) const
    {
        return !(*this == other);
    }

};

/**
 * Parameters that must be given to the NoteStore emailNote call. These allow
 * the caller to specify the note to send, the recipient addresses, etc.
 *
 * */
struct NoteEmailParameters {
    /**
    If set, this must be the GUID of a note within the user's account that
          should be retrieved from the service and sent as email.  If not set,
          the 'note' field must be provided instead.
    */
    Optional< QString > guid;
    /**
    If the 'guid' field is not set, this field must be provided, including
          the full contents of the note note (and all of its Resources) to send.
          This can be used for a Note that as not been created in the service,
          for example by a local client with local notes.
    */
    Optional< Note > note;
    /**
    If provided, this should contain a list of the SMTP email addresses
          that should be included in the "To:" line of the email.
          Callers must specify at least one "to" or "cc" email address.
    */
    Optional< QStringList > toAddresses;
    /**
    If provided, this should contain a list of the SMTP email addresses
          that should be included in the "Cc:" line of the email.
          Callers must specify at least one "to" or "cc" email address.
    */
    Optional< QStringList > ccAddresses;
    /**
    If provided, this should contain the subject line of the email that
          will be sent.  If not provided, the title of the note will be used
          as the subject of the email.
    */
    Optional< QString > subject;
    /**
    If provided, this is additional personal text that should be included
          into the email as a message from the owner to the recipient(s).
    */
    Optional< QString > message;

    bool operator==(const NoteEmailParameters& other) const
    {
        return guid.isEqual(other.guid)
            && note.isEqual(other.note)
            && toAddresses.isEqual(other.toAddresses)
            && ccAddresses.isEqual(other.ccAddresses)
            && subject.isEqual(other.subject)
            && message.isEqual(other.message)
        ;
    }

    bool operator!=(const NoteEmailParameters& other) const
    {
        return !(*this == other);
    }

};

/**
 * The result of calling findRelated().  The contents of the notes,
 * notebooks, and tags fields will be in decreasing order of expected
 * relevance.  It is possible that fewer results than requested will be
 * returned even if there are enough distinct entities in the account
 * in cases where the relevance is estimated to be low.
 *
 * */
struct RelatedResult {
    /**
    If notes have been requested to be included, this will be the
         list of notes.
    */
    Optional< QList< Note > > notes;
    /**
    If notebooks have been requested to be included, this will be the
         list of notebooks.
    */
    Optional< QList< Notebook > > notebooks;
    /**
    If tags have been requested to be included, this will be the list
         of tags.
    */
    Optional< QList< Tag > > tags;
    /**
    If <code>includeContainingNotebooks</code> is set to <code>true</code>
         in the RelatedResultSpec, return the list of notebooks to
         to which the returned related notes belong. The notebooks in this
         list will occur once per notebook GUID and are represented as
         NotebookDescriptor objects.
    */
    Optional< QList< NotebookDescriptor > > containingNotebooks;

    bool operator==(const RelatedResult& other) const
    {
        return notes.isEqual(other.notes)
            && notebooks.isEqual(other.notebooks)
            && tags.isEqual(other.tags)
            && containingNotebooks.isEqual(other.containingNotebooks)
        ;
    }

    bool operator!=(const RelatedResult& other) const
    {
        return !(*this == other);
    }

};


/**
 * This exception is thrown by EDAM procedures when a call fails as a result of 
 * a problem that a caller may be able to resolve.  For example, if the user
 * attempts to add a note to their account which would exceed their storage
 * quota, this type of exception may be thrown to indicate the source of the
 * error so that they can choose an alternate action.
 *
 * This exception would not be used for internal system errors that do not
 * reflect user actions, but rather reflect a problem within the service that
 * the user cannot resolve.
 *
 * errorCode:  The numeric code indicating the type of error that occurred.
 *   must be one of the values of EDAMErrorCode.
 *
 * parameter:  If the error applied to a particular input parameter, this will
 *   indicate which parameter.
 */
class EDAMUserException: public EvernoteException {
public:
    EDAMErrorCode::type errorCode;
    Optional< QString > parameter;

    EDAMUserException() {}
    ~EDAMUserException() throw() {}
    const char* what() const throw() Q_DECL_OVERRIDE;
    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const Q_DECL_OVERRIDE;
};

/**
 * This exception is thrown by EDAM procedures when a call fails as a result of
 * a problem in the service that could not be changed through caller action.
 *
 * errorCode:  The numeric code indicating the type of error that occurred.
 *   must be one of the values of EDAMErrorCode.
 *
 * message:  This may contain additional information about the error
 *
 * rateLimitDuration:  Indicates the minimum number of seconds that an application should
 *   expect subsequent API calls for this user to fail. The application should not retry
 *   API requests for the user until at least this many seconds have passed. Present only
 *   when errorCode is RATE_LIMIT_REACHED,
 */
class EDAMSystemException: public EvernoteException {
public:
    EDAMErrorCode::type errorCode;
    Optional< QString > message;
    Optional< qint32 > rateLimitDuration;

    EDAMSystemException() {}
    ~EDAMSystemException() throw() {}
    const char* what() const throw() Q_DECL_OVERRIDE;
    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const Q_DECL_OVERRIDE;
};

/**
 * This exception is thrown by EDAM procedures when a caller asks to perform
 * an operation on an object that does not exist.  This may be thrown based on an invalid
 * primary identifier (e.g. a bad GUID), or when the caller refers to an object
 * by another unique identifier (e.g. a User's email address).
 *
 * identifier:  A description of the object that was not found on the server.
 *   For example, "Note.notebookGuid" when a caller attempts to create a note in a
 *   notebook that does not exist in the user's account.
 *
 * key:  The value passed from the client in the identifier, which was not
 *   found. For example, the GUID that was not found.
 */
class EDAMNotFoundException: public EvernoteException {
public:
    Optional< QString > identifier;
    Optional< QString > key;

    EDAMNotFoundException() {}
    ~EDAMNotFoundException() throw() {}
    const char* what() const throw() Q_DECL_OVERRIDE;
    virtual QSharedPointer<EverCloudExceptionData> exceptionData() const Q_DECL_OVERRIDE;
};


}

Q_DECLARE_METATYPE(qevercloud::SyncState)
Q_DECLARE_METATYPE(qevercloud::SyncChunkFilter)
Q_DECLARE_METATYPE(qevercloud::NoteFilter)
Q_DECLARE_METATYPE(qevercloud::NotesMetadataResultSpec)
Q_DECLARE_METATYPE(qevercloud::NoteCollectionCounts)
Q_DECLARE_METATYPE(qevercloud::NoteVersionId)
Q_DECLARE_METATYPE(qevercloud::ClientUsageMetrics)
Q_DECLARE_METATYPE(qevercloud::RelatedQuery)
Q_DECLARE_METATYPE(qevercloud::RelatedResultSpec)
Q_DECLARE_METATYPE(qevercloud::Data)
Q_DECLARE_METATYPE(qevercloud::UserAttributes)
Q_DECLARE_METATYPE(qevercloud::Accounting)
Q_DECLARE_METATYPE(qevercloud::BusinessUserInfo)
Q_DECLARE_METATYPE(qevercloud::PremiumInfo)
Q_DECLARE_METATYPE(qevercloud::User)
Q_DECLARE_METATYPE(qevercloud::Tag)
Q_DECLARE_METATYPE(qevercloud::LazyMap)
Q_DECLARE_METATYPE(qevercloud::ResourceAttributes)
Q_DECLARE_METATYPE(qevercloud::Resource)
Q_DECLARE_METATYPE(qevercloud::NoteAttributes)
Q_DECLARE_METATYPE(qevercloud::Note)
Q_DECLARE_METATYPE(qevercloud::Publishing)
Q_DECLARE_METATYPE(qevercloud::BusinessNotebook)
Q_DECLARE_METATYPE(qevercloud::SavedSearchScope)
Q_DECLARE_METATYPE(qevercloud::SavedSearch)
Q_DECLARE_METATYPE(qevercloud::SharedNotebookRecipientSettings)
Q_DECLARE_METATYPE(qevercloud::SharedNotebook)
Q_DECLARE_METATYPE(qevercloud::NotebookRestrictions)
Q_DECLARE_METATYPE(qevercloud::Notebook)
Q_DECLARE_METATYPE(qevercloud::LinkedNotebook)
Q_DECLARE_METATYPE(qevercloud::NotebookDescriptor)
Q_DECLARE_METATYPE(qevercloud::PublicUserInfo)
Q_DECLARE_METATYPE(qevercloud::AuthenticationResult)
Q_DECLARE_METATYPE(qevercloud::BootstrapSettings)
Q_DECLARE_METATYPE(qevercloud::BootstrapProfile)
Q_DECLARE_METATYPE(qevercloud::BootstrapInfo)
Q_DECLARE_METATYPE(qevercloud::SyncChunk)
Q_DECLARE_METATYPE(qevercloud::NoteList)
Q_DECLARE_METATYPE(qevercloud::NoteMetadata)
Q_DECLARE_METATYPE(qevercloud::NotesMetadataList)
Q_DECLARE_METATYPE(qevercloud::NoteEmailParameters)
Q_DECLARE_METATYPE(qevercloud::RelatedResult)

#endif // QEVERCLOUD_GENERATED_TYPES_H
