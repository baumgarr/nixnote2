#ifndef JAVAMACHINE_H
#define JAVAMACHINE_H

#include <jni.h>
#include <QString>
#include "messageblock.h"
#include <evernote/NoteStore.h>
#include <evernote/UserStore.h>

using namespace evernote::edam;
using namespace std;

class JavaMachine
{
private:
    JavaVM* jvm;
    JNIEnv* env;
    JavaVMInitArgs args;
    JavaVMOption options[1];
    jobject syncChunk;

    // Class & method used to pass error messages back
    jclass messageBlockClass;
    jmethodID messageBlock_isError;
    jmethodID messageBlock_getErrorMessage;

    // Class & methods used to connect
    jclass communicationClass;
    jobject communication;
    jmethodID communication_connect;
    jmethodID communication_constructor;
    jmethodID communication_getMessageBlock;
    jmethodID communication_getSyncState;
    jmethodID communication_loadSyncChunk;

    // Class & method used for sync state
    jclass syncStateClass;
    jmethodID syncState_isSetCurrentTime;
    jmethodID syncState_isSetUpdateCount;
    jmethodID syncState_isSetUploaded;
    jmethodID syncState_isSetFullSyncBefore;
    jmethodID syncState_getCurrentTime;
    jmethodID syncState_getFullSyncBefore;
    jmethodID syncState_getUpdateCount;
    jmethodID syncState_getUploaded;

    // Class & method used for sync state
    jclass syncChunkClass;
    jmethodID syncChunk_getCurrentTime;
    jmethodID syncChunk_isCurrentTimeSet;
    jmethodID syncChunk_getChunkHighUSN;
    jmethodID syncChunk_isSetChunkHighUSN;
    jmethodID syncChunk_getUpdateCount;
    jmethodID syncChunk_isSetUpdateCount;
    jmethodID syncChunk_getNotesSize;
    jmethodID syncChunk_getNotes;
    jmethodID syncChunk_isSetNotes;
    jmethodID syncChunk_getNotebooksSize;
    jmethodID syncChunk_isSetNotebooks;
    jmethodID syncChunk_getNotebooks;
    jmethodID syncChunk_getTagsSize;
    jmethodID syncChunk_getTags;
    jmethodID syncChunk_isSetTags;
    jmethodID syncChunk_getSearchesSize;
    jmethodID syncChunk_getSearches;
    jmethodID syncChunk_isSetSearches;
    jmethodID syncChunk_getResourcesSize;
    jmethodID syncChunk_getResources;
    jmethodID syncChunk_isSetResources;
    jmethodID syncChunk_getLinkedNotebooksSize;
    jmethodID syncChunk_getLinkedNotebooks;
    jmethodID syncChunk_isSetLinkedNotebooks;
    jmethodID syncChunk_getExpungedNotesSize;
    jmethodID syncChunk_getExpungedNotes;
    jmethodID syncChunk_isSetExpungedNotes;
    jmethodID syncChunk_getExpungedNotebooksSize;
    jmethodID syncChunk_getExpungedNotebooks;
    jmethodID syncChunk_isSetExpungedNotebooks;
    jmethodID syncChunk_getExpungedTagsSize;
    jmethodID syncChunk_getExpungedTags;
    jmethodID syncChunk_isSetExpungedTags;
    jmethodID syncChunk_getExpungedSearchesSize;
    jmethodID syncChunk_getExpungedSearches;
    jmethodID syncChunk_isSetExpungedSearches;
    jmethodID syncChunk_getExpungedLinkedNotebooksSize;
    jmethodID syncChunk_getExpungedLinkedNotebooks;
    jmethodID syncChunk_isSetExpungedLinkedNotebooks;

    jclass listClass;
    jmethodID list_get;


    // Class & method used for user
    jclass userClass;
    jmethodID user_getID;
    jmethodID user_isIDSet;
    jmethodID user_getUsername;
    jmethodID user_isUsernameSet;
    jmethodID user_getEmail;
    jmethodID user_isEmailSet;
    jmethodID user_getName;
    jmethodID user_isNameSet;
    jmethodID user_getTimezone;
    jmethodID user_isTimezoneSet;
    jmethodID user_getPrivelegeLevel;
    jmethodID user_isPrivilegeLevelSet;
    jmethodID user_getCreated;
    jmethodID user_isCreatedSet;
    jmethodID user_getUpdated;
    jmethodID user_isUpdatedSet;
    jmethodID user_getDeleted;
    jmethodID user_isDeletedSet;
    jmethodID user_getActive;
    jmethodID user_isActiveSet;
    jmethodID user_getShardID;
    jmethodID user_isShardIDSet;
    jmethodID user_getAttributes;
    jmethodID user_isAttributesSet;
    jmethodID user_getAccounting;
    jmethodID user_isAccountingSet;

    // Class & method used for user attributes
    jclass userAttributesClass;
    jmethodID userAttributes_getDefaultLocationName;
    jmethodID userAttributes_isdefaultLocationNameSet;
    jmethodID userAttributes_getDefaultLatitude;
    jmethodID userAttributes_isDefaultLatitudeSet;
    jmethodID userAttributes_getDefaultLongitude;
    jmethodID userAttributes_isDefaultLongitudeSet;
    jmethodID userAttributes_getPreactivation;
    jmethodID userAttributes_isPreactivationSet;
    jmethodID userAttributes_incomingEmailAddress;
    jmethodID userAttributes_isIncomingEmailAddressSet;
    jmethodID userAttributes_getComments;
    jmethodID userAttributes_isCommentsSet;
    jmethodID userAttributes_getDateAgreedToTermsOfService;
    jmethodID userAttributes_isDateAgreedToTermsOfServiceSet;
    jmethodID userAttributes_getMaxReferrals;
    jmethodID userAttributes_isMaxReferralsSet;
    jmethodID userAttributes_getReferralCount;
    jmethodID userAttributes_isReferralCountSet;
    jmethodID userAttributes_getRefererCode;
    jmethodID userAttributes_isRefererCodeSet;
    jmethodID userAttributes_getSentEmailDate;
    jmethodID userAttributes_isSentEmailDateSet;
    jmethodID userAttributes_getSentEmailCount;
    jmethodID userAttributes_isSentEmailCountSet;
    jmethodID userAttributes_getDailyEmailLimit;
    jmethodID userAttributes_isDailyEmailLimitSet;
    jmethodID userAttributes_getEmailOptOutDate;
    jmethodID userAttributes_isEmailOptOutDateSet;
    jmethodID userAttributes_getPreferredLanguage;
    jmethodID userAttributes_isPreferreredLanguageSet;
    jmethodID userAttributes_getClipFullPage;
    jmethodID userAttributes_isClipFullPageSet;
    jmethodID userAttributes_getTwitterID;
    jmethodID userAttributes_isTwitterIDSet;
    jmethodID userAttributes_getGroupName;
    jmethodID userAttributes_isGroupNameSet;
    jmethodID userAttributes_getRecognitionLanguage;
    jmethodID userAttributes_isRecognitionLanguageSet;
    jmethodID userAttributes_getCustomerProfileID;
    jmethodID userAttributes_isCustomerProfileIDSet;
    jmethodID userAttributes_getReferralProof;
    jmethodID userAttributes_isReferralProofSet;
    jmethodID userAttributes_getEducationalDiscount;
    jmethodID userAttributes_isEducationalDiscountSet;
    jmethodID userAttributes_getBusinessAddress;
    jmethodID userAttributes_isBusinessAddressSet;
    jmethodID userAttributes_getHideSponsorBilling;
    jmethodID userAttributes_isHideSponsorBillingSet;


    jclass accountingClass;
    jmethodID accounting_getUploadLimit;
    jmethodID accounting_isUploadLimitSet;
    jmethodID accounting_getUploadLimitEnd;
    jmethodID accounting_isUploadLimitEndSet;
    jmethodID accounting_getUploadLimitNextMonth;
    jmethodID accounting_isUploadLimitNextMonthSet;
    jmethodID accounting_getPremiumServiceStatus;
    jmethodID accounting_isPremiumServiceStatusSet;
    jmethodID accounting_getPremiumServiceStart;
    jmethodID accounting_isPremiumServiceStartSet;
    jmethodID accounting_getPremiumOrderNumber;
    jmethodID accounting_isUPremiumOrderNumberSet;
    jmethodID accounting_getPremiumCommerceService;
    jmethodID accounting_isPremiumCommerceServiceSet;
    jmethodID accounting_getLastSuccessfulCharge;
    jmethodID accounting_isLastSuccessfulChargeSet;
    jmethodID accounting_getLastFailedCharge;
    jmethodID accounting_isLastFailedChargeSet;
    jmethodID accounting_getLastFailedChargeReason;
    jmethodID accounting_isLastFailedChargeReasonSet;
    jmethodID accounting_getNextPaymentDue;
    jmethodID accounting_isNextPaymentDueSet;
    jmethodID accounting_getPremiumLockUntil;
    jmethodID accounting_isPremiumLockUntilSet;
    jmethodID accounting_getUpdated;
    jmethodID accounting_isUpdatedSet;
    jmethodID accounting_getPremiumSubscriptionNumber;
    jmethodID accounting_isPremiumSubscriptionNumberSet;
    jmethodID accounting_getLastRequestedCharge;
    jmethodID accounting_isLastRequestedChargeSet;
    jmethodID accounting_getCurrency;
    jmethodID accounting_isCurrencySet;
    jmethodID accounting_getUnitPrice;
    jmethodID accounting_isUnitPriceSet;

    jclass dataClass;
    jmethodID data_getBodyHash;
    jmethodID data_isBodyHashSet;
    jmethodID data_getSize;
    jmethodID data_isSizeSet;
    jmethodID data_getBody;
    jmethodID data_isBodySet;

    jclass tagClass;
    jmethodID tag_getGuid;
    jmethodID tag_isGuidSet;
    jmethodID tag_getName;
    jmethodID tag_isNameSet;
    jmethodID tag_getParentGuid;
    jmethodID tag_isParentGuidSet;
    jmethodID tag_getSequenceNumber;
    jmethodID tag_isSequenceNumberSet;

    jclass resourceAttributesClass;
    jmethodID resourceAttributes_getSourceUrl;
    jmethodID resourceAttributes_isSourceUrlSet;
    jmethodID resourceAttributes_getTimestamp;
    jmethodID resourceAttributes_isTimestampSet;
    jmethodID resourceAttributes_getLatitude;
    jmethodID resourceAttributes_isLatitudeSet;
    jmethodID resourceAttributes_getLongitude;
    jmethodID resourceAttributes_isLongitudeSet;
    jmethodID resourceAttributes_getAltitude;
    jmethodID resourceAttributes_isAltitudeSet;
    jmethodID resourceAttributes_getCameraMake;
    jmethodID resourceAttributes_isCameraMakeSet;
    jmethodID resourceAttributes_getCameraModel;
    jmethodID resourceAttributes_isCameraModelSet;
    jmethodID resourceAttributes_getClientWillIndex;
    jmethodID resourceAttributes_isClientWillIndexSet;
    jmethodID resourceAttributes_getRecoType;
    jmethodID resourceAttributes_isRecoTypeSet;
    jmethodID resourceAttributes_getFileName;
    jmethodID resourceAttributes_isFileNameSet;
    jmethodID resourceAttributes_getAttachment;
    jmethodID resourceAttributes_isAttachmentSet;
    jmethodID resourceAttributes_getApplicationData;
    jmethodID resourceAttributes_isApplicationDataSet;


    jclass resourceClass;
    jmethodID resource_getGuid;
    jmethodID resource_isGuidSet;
    jmethodID resource_getNoteGuid;
    jmethodID resource_isNoteGuidSet;
    jmethodID resource_getData;
    jmethodID resource_isDataSet;
    jmethodID resource_getMime;
    jmethodID resource_isMimeSet;
    jmethodID resource_getWidth;
    jmethodID resource_isWidthSet;
    jmethodID resource_getHeight;
    jmethodID resource_isHeightSet;
    jmethodID resource_getDuration;
    jmethodID resource_isDurationSet;
    jmethodID resource_getActive;
    jmethodID resource_isActiveSet;
    jmethodID resource_getRecognition;
    jmethodID resource_isRecognitionSet;
    jmethodID resource_getAttributes;
    jmethodID resource_isAttributesSet;
    jmethodID resource_getUpdateSequenceNumber;
    jmethodID resource_isUpdateSequenceNumberSet;
    jmethodID resource_getAlternateData;
    jmethodID resource_isAlternateDataSet;

    jclass noteAttributesClass;
    jmethodID noteAttributes_getSubjectDate;
    jmethodID noteAttributes_isSubjectDateSet;
    jmethodID noteAttributes_getLatitude;
    jmethodID noteAttributes_isLatitudeSet;
    jmethodID noteAttributes_getLongitude;
    jmethodID noteAttributes_isLongitudeSet;
    jmethodID noteAttributes_getAltitude;
    jmethodID noteAttributes_isAltitudeSet;
    jmethodID noteAttributes_getAuthor;
    jmethodID noteAttributes_isAuthorSet;
    jmethodID noteAttributes_getSource;
    jmethodID noteAttributes_isSourceSet;
    jmethodID noteAttributes_getSourceURL;
    jmethodID noteAttributes_isSourceURLSet;
    jmethodID noteAttributes_getSourceApplication;
    jmethodID noteAttributes_isSourceApplicationSet;
    jmethodID noteAttributes_getShareDate;
    jmethodID noteAttributes_isShareDateSet;
    jmethodID noteAttributes_getPlaceName;
    jmethodID noteAttributes_isPlaceNameSet;
    jmethodID noteAttributes_getContentClass;
    jmethodID noteAttributes_isContentClassSet;
    jmethodID noteAttributes_getApplicationData;
    jmethodID noteAttributes_isApplicationDataSet;
    jmethodID noteAttributes_getLastEditedBy;
    jmethodID noteAttributes_isLastEditedBySet;
    jclass lazyMapClass;

    jclass noteClass;
    jmethodID noteClass_getGuid;
    jmethodID noteClass_isGuidSet;
    jmethodID noteClass_getTitle;
    jmethodID noteClass_isTitleSet;
    jmethodID noteClass_getContent;
    jmethodID noteClass_isContentSet;
    jmethodID noteClass_getContentHash;
    jmethodID noteClass_isContentHashSet;
    jmethodID noteClass_getContentLength;
    jmethodID noteClass_isContentLengthSet;
    jmethodID noteClass_getCreated;
    jmethodID noteClass_isCreatedSet;
    jmethodID noteClass_getUpdated;
    jmethodID noteClass_isUpdatedSet;
    jmethodID noteClass_getDeleted;
    jmethodID noteClass_isDeletedSet;
    jmethodID noteClass_getActive;
    jmethodID noteClass_isActiveSet;
    jmethodID noteClass_getUpdateSequenceNumber;
    jmethodID noteClass_isUpdateSequenceNumberSet;
    jmethodID noteClass_getNotebookGuid;
    jmethodID noteClass_isNotebookGuidSet;
    jmethodID noteClass_getTagGuids;
    jmethodID noteClass_isTagGuidsSet;
    jmethodID noteClass_getTagGuidsSize;
    jmethodID noteClass_getResources;
    jmethodID noteClass_isResourcesSet;
    jmethodID noteClass_getAttributes;
    jmethodID noteClass_isAttributesSet;
    jmethodID noteClass_getTagNames;
    jmethodID noteClass_isTagNamesSet;
    jmethodID noteClass_getTagNamesSize;
    jmethodID noteClass_getResourcesSize;

    jclass publishingClass;
    jmethodID publishingClass_getUri;
    jmethodID publishingClass_isUriSet;
    jmethodID publishingClass_getOrder;
    jmethodID publishingClass_isOrderSet;
    jmethodID publishingClass_getAscending;
    jmethodID publishingClass_isAscendingSet;
    jmethodID publishingClass_getPublicDescription;
    jmethodID publishingClass_isPublicDescriptionSet;

    jclass notebookClass;
    jmethodID notebook_getGuid;
    jmethodID notebook_isGuidSet;
    jmethodID notebook_getName;
    jmethodID notebook_isNameSet;
    jmethodID notebook_getUpdateSequenceNumber;
    jmethodID notebook_isUpdateSequenceNumberSet;
    jmethodID notebook_getDefaultNotebook;
    jmethodID notebook_isDefaultNotebookSet;
    jmethodID notebook_getServiceCreated;
    jmethodID notebook_isServiceCreatedSet;
    jmethodID notebook_getServiceUpdated;
    jmethodID notebook_isServiceUpdatedSet;
    jmethodID notebook_getPublishing;
    jmethodID notebook_isPublishingSet;
    jmethodID notebook_getPublished;
    jmethodID notebook_isPublishedSet;
    jmethodID notebook_getStack;
    jmethodID notebook_isStackSet;
    jmethodID notebook_getSharedNotebookIDs;
    jmethodID notebook_isSharedNotebookIDsSet;

    jclass savedSearchClass;
    jmethodID savedSearch_getGuid;
    jmethodID savedSearch_isGuidSet;
    jmethodID savedSearch_getName;
    jmethodID savedSearch_isNameSet;
    jmethodID savedSearch_getQuery;
    jmethodID savedSearch_isQuerySet;
    jmethodID savedSearch_getFormat;
    jmethodID savedSearch_isFormatSet;
    jmethodID savedSearch_getUpdateSequenceNumber;
    jmethodID savedSearch_isUpdateSequenceNumberSet;

    jclass queryFormatClass;
    jmethodID queryFormat_toString;

    jclass sharedNotebookClass;
    jmethodID sharedNotebook_getID;
    jmethodID sharedNotebook_isIDSet;
    jmethodID sharedNotebook_getUserID;
    jmethodID sharedNotebook_isUserIDSet;
    jmethodID sharedNotebook_getNotebookGuid;
    jmethodID sharedNotebook_isNotebookGuidSet;
    jmethodID sharedNotebook_getRequireLogin;
    jmethodID sharedNotebook_isRequireLoginSet;
    jmethodID sharedNotebook_getServiceCreated;
    jmethodID sharedNotebook_isServiceCreatedSet;
    jmethodID sharedNotebook_getShareKey;
    jmethodID sharedNotebook_isShareKeySet;
    jmethodID sharedNotebook_getUsername;
    jmethodID sharedNotebook_isUsernameSet;

    jclass linkedNotebookClass;
    jmethodID linkedNotebook_getShareName;
    jmethodID linkedNotebook_isShareNameSet;
    jmethodID linkedNotebook_getUsername;
    jmethodID linkedNotebook_isUsernameSet;
    jmethodID linkedNotebook_getShardID;
    jmethodID linkedNotebook_isShardIDSet;
    jmethodID linkedNotebook_getUri;
    jmethodID linkedNotebook_isUriSet;
    jmethodID linkedNotebook_getGuid;
    jmethodID linkedNotebook_isGuidSet;
    jmethodID linkedNotebook_getUpdateSequenceNumber;
    jmethodID linkedNotebook_isUpdateSequenceNumberSet;

    void processChunkNotes(SyncChunk &syncChunk, jobject list, int size);
    void processChunkNotebooks(SyncChunk &syncChunk, jobject list, int size);
    void processChunkTags(SyncChunk &syncChunk, jobject list, int size);
    void processChunkSearches(SyncChunk &syncChunk, jobject list, int size);
    void processChunkLinkedNotebooks(SyncChunk &syncChunk, jobject list, int size);
    void processChunkExpungedNotes(SyncChunk &syncChunk, jobject list, int size);
    void processChunkExpungedNotebooks(SyncChunk &syncChunk, jobject list, int size);
    void processChunkExpungedTags(SyncChunk &syncChunk, jobject list, int size);
    void processChunkExpungedLinkedNotebooks(SyncChunk &syncChunk, jobject list, int size);
    void processChunkExpungedSearches(SyncChunk &syncChunk, jobject list, int size);
    void processChunkResources(SyncChunk &syncChunk, jobject list, int size);

    void mapToTag(Tag &tag, jobject jtag);
    void mapToNotebook(Notebook &notebook, jobject jnotebook);
    void mapToNote(Note &note, jobject jnote);
    void mapToLinkedNotebook(LinkedNotebook &linkedNotebook,jobject notebook);
    void mapToSearch(SavedSearch &savedSearch, jobject search);
    QStringList mapToStringList(jobject jlist, int size);
    void mapToResources(vector<Resource> &resourceList, jobject jresourceList, int size);
    void mapToAttributes(NoteAttributes &noteAttributes, jobject attributes);
    void mapToData(Data &data, jobject jdata);
    void mapToResourceAttributes(ResourceAttributes &attributes, jobject jattributes);

public:
    JavaMachine();
    bool connect();
    void create_jvm();
    void convertMessageBlock(MessageBlock &mb, jobject messageBlock);
    void jString2QString(QString &qstr, jstring &jstring, bool release=true);
    void jString2String(string &qstr, jstring &jstring, bool release=true);
    void jByteArray2QByteArray(QByteArray &qstr, jbyteArray &jb, bool release=true);
    bool getSyncState(SyncState &syncState);
    bool getSyncChunk(SyncChunk &chunk, int start, int chunkSize, bool fullSync=false);
};

#endif // JAVAMACHINE_H
