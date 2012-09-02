#include "javamachine.h"
#include "global.h"
#include <QString>
#include "oauth/oauthtokenizer.h"
#include <sstream>
#include <string>
#include <cstdio>
#include <iostream>

extern Global global;


JavaMachine::JavaMachine()
{
}



void JavaMachine::create_jvm() {
    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;
    QString cp = global.fileManager.getProgramDirPath("java/");
    QString clp = "-Djava.class.path="+cp+"nixnote.jar:"+cp+
            "evernote-api-1.21.jar:"+cp+"libthrift.jar:"+cp+
            "log4j-1.2.14.jar -XstartOnFirstThread -Xss32m -Xms32m -Xmx512m";
    char *cp2;
    cp2 = new char[clp.length()+1];
    strcpy(cp2,clp.toAscii());
    options[0].optionString = cp2;
    args.options = options;
    args.ignoreUnrecognized = JNI_FALSE;

    JNI_CreateJavaVM(&jvm, (void **)&env, &args);


    int status =jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if(status < 0)
    {
        status = jvm->AttachCurrentThread((void**)&env, NULL);
    }

    // Start loading classes
    messageBlockClass = env->FindClass("org/nixnote/bridge/MessageBlock");
    messageBlock_isError = env->GetMethodID(messageBlockClass, "isError", "()Z");
    messageBlock_getErrorMessage = env->GetMethodID(messageBlockClass, "getErrorMessage", "()Ljava/lang/String;");

    communicationClass = env->FindClass("org/nixnote/bridge/Communication");
    //env->ExceptionDescribe();
    communication_constructor = env->GetMethodID(communicationClass, "<init>", "()V");
    communication_connect = env->GetMethodID(communicationClass, "connect", "(Ljava/lang/String;)Z");
    communication = env->NewObject(communicationClass, communication_constructor);
    communication_getMessageBlock = env->GetMethodID(communicationClass, "getMessageBlock", "()Lorg/nixnote/bridge/MessageBlock;");
    communication_getSyncState = env->GetMethodID(communicationClass, "getSyncState", "()Lcom/evernote/edam/notestore/SyncState;");
    communication_loadSyncChunk = env->GetMethodID(communicationClass, "loadChunk", "(IIZ)Lcom/evernote/edam/notestore/SyncChunk;");

    syncStateClass = env->FindClass("com/evernote/edam/notestore/SyncState");
    syncState_getCurrentTime = env->GetMethodID(syncStateClass, "getCurrentTime", "()J");
    syncState_getFullSyncBefore = env->GetMethodID(syncStateClass, "getFullSyncBefore", "()J");
    syncState_getUpdateCount = env->GetMethodID(syncStateClass, "getUpdateCount", "()I");
    syncState_getUploaded = env->GetMethodID(syncStateClass, "getUploaded", "()J");
    syncState_isSetCurrentTime = env->GetMethodID(syncStateClass, "isSetCurrentTime", "()Z");
    syncState_isSetUpdateCount = env->GetMethodID(syncStateClass, "isSetUpdateCount", "()Z");
    syncState_isSetUploaded = env->GetMethodID(syncStateClass, "isSetUploaded", "()Z");
    syncState_isSetFullSyncBefore = env->GetMethodID(syncStateClass, "isSetFullSyncBefore", "()Z");

    syncChunkClass = env->FindClass("com/evernote/edam/notestore/SyncChunk");
    syncChunk_getCurrentTime = env->GetMethodID(syncChunkClass, "getCurrentTime", "()J");
    syncChunk_isCurrentTimeSet = env->GetMethodID(syncChunkClass, "isSetCurrentTime", "()Z");
    syncChunk_getChunkHighUSN = env->GetMethodID(syncChunkClass, "getChunkHighUSN", "()I");
    syncChunk_isSetChunkHighUSN = env->GetMethodID(syncChunkClass, "isSetChunkHighUSN", "()Z");
    syncChunk_getUpdateCount = env->GetMethodID(syncChunkClass, "getUpdateCount", "()I");
    syncChunk_isSetUpdateCount = env->GetMethodID(syncChunkClass, "isSetUpdateCount", "()Z");
    syncChunk_getNotesSize = env->GetMethodID(syncChunkClass, "getNotesSize", "()I");
    syncChunk_getNotes = env->GetMethodID(syncChunkClass, "getNotes", "()Ljava/util/List;");
    syncChunk_isSetNotes = env->GetMethodID(syncChunkClass, "isSetNotes", "()Z");
    syncChunk_getNotebooksSize = env->GetMethodID(syncChunkClass, "getNotebooksSize", "()I");
    syncChunk_isSetNotebooks = env->GetMethodID(syncChunkClass, "isSetNotebooks", "()Z");
    syncChunk_getNotebooks = env->GetMethodID(syncChunkClass, "getNotebooks", "()Ljava/util/List;");
    syncChunk_getTagsSize = env->GetMethodID(syncChunkClass, "getTagsSize", "()I");
    syncChunk_getTags = env->GetMethodID(syncChunkClass, "getTags", "()Ljava/util/List;");;
    syncChunk_isSetTags = env->GetMethodID(syncChunkClass, "isSetTags", "()Z");
    syncChunk_getSearchesSize = env->GetMethodID(syncChunkClass, "getSearchesSize", "()I");
    syncChunk_getSearches = env->GetMethodID(syncChunkClass, "getSearches", "()Ljava/util/List;");;
    syncChunk_isSetSearches = env->GetMethodID(syncChunkClass, "isSetSearches", "()Z");
    syncChunk_getResourcesSize = env->GetMethodID(syncChunkClass, "getResourcesSize", "()I");
    syncChunk_getResources = env->GetMethodID(syncChunkClass, "getResources", "()Ljava/util/List;");;
    syncChunk_isSetResources = env->GetMethodID(syncChunkClass, "isSetResources", "()Z");
    syncChunk_getLinkedNotebooksSize = env->GetMethodID(syncChunkClass, "getLinkedNotebooksSize", "()I");
    syncChunk_getLinkedNotebooks = env->GetMethodID(syncChunkClass, "getLinkedNotebooks", "()Ljava/util/List;");;
    syncChunk_isSetLinkedNotebooks = env->GetMethodID(syncChunkClass, "isSetLinkedNotebooks", "()Z");
    syncChunk_getExpungedNotesSize = env->GetMethodID(syncChunkClass, "getExpungedNotesSize", "()I");
    syncChunk_getExpungedNotes = env->GetMethodID(syncChunkClass, "getExpungedNotes", "()Ljava/util/List;");;
    syncChunk_isSetExpungedNotes = env->GetMethodID(syncChunkClass, "isSetExpungedNotes", "()Z");
    syncChunk_getExpungedNotebooksSize = env->GetMethodID(syncChunkClass, "getExpungedNotebooksSize", "()I");
    syncChunk_getExpungedNotebooks = env->GetMethodID(syncChunkClass, "getExpungedNotebooks", "()Ljava/util/List;");
    syncChunk_isSetExpungedNotebooks = env->GetMethodID(syncChunkClass, "isSetExpungedNotebooks", "()Z");
    syncChunk_getExpungedTagsSize = env->GetMethodID(syncChunkClass, "getExpungedTagsSize", "()I");
    syncChunk_getExpungedTags = env->GetMethodID(syncChunkClass, "getExpungedTags", "()Ljava/util/List;");
    syncChunk_isSetExpungedTags = env->GetMethodID(syncChunkClass, "isSetExpungedTags", "()Z");
    syncChunk_getExpungedSearchesSize = env->GetMethodID(syncChunkClass, "getExpungedSearchesSize", "()I");
    syncChunk_getExpungedSearches = env->GetMethodID(syncChunkClass, "getExpungedSearches", "()Ljava/util/List;");
    syncChunk_isSetExpungedSearches = env->GetMethodID(syncChunkClass, "isSetExpungedSearches", "()Z");
    syncChunk_getExpungedLinkedNotebooksSize = env->GetMethodID(syncChunkClass, "getExpungedLinkedNotebooksSize", "()I");
    syncChunk_getExpungedLinkedNotebooks = env->GetMethodID(syncChunkClass, "getExpungedLinkedNotebooks", "()Ljava/util/List;");
    syncChunk_isSetExpungedLinkedNotebooks = env->GetMethodID(syncChunkClass, "isSetExpungedLinkedNotebooks", "()Z");


    userClass = env->FindClass("com/evernote/edam/type/User");
    user_getID = env->GetMethodID(userClass, "getId", "()I");
    user_getUsername = env->GetMethodID(userClass, "getUsername","()Ljava/lang/String;");
    user_getEmail = env->GetMethodID(userClass, "getEmail", "()Ljava/lang/String;");
    user_getName = env->GetMethodID(userClass, "getName", "()Ljava/lang/String;");
    user_getTimezone = env->GetMethodID(userClass, "getTimezone", "()Ljava/lang/String;");
    user_getPrivelegeLevel = env->GetMethodID(userClass, "getPrivilege", "()Lcom/evernote/edam/type/PrivilegeLevel;");
    user_getCreated = env->GetMethodID(userClass, "getCreated", "()J");
    user_getUpdated = env->GetMethodID(userClass, "getUpdated", "()J");
    user_getDeleted = env->GetMethodID(userClass, "getDeleted", "()J");
    user_getActive = env->GetMethodID(userClass, "isActive", "()Z");
    user_getShardID = env->GetMethodID(userClass, "getShardId", "()Ljava/lang/String;");
    user_getAttributes = env->GetMethodID(userClass, "getAttributes", "()Lcom/evernote/edam/type/UserAttributes;");
    user_getAccounting = env->GetMethodID(userClass, "getAccounting", "()Lcom/evernote/edam/type/Accounting;");
    user_isIDSet = env->GetMethodID(userClass, "isSetId", "()Z");
    user_isUsernameSet = env->GetMethodID(userClass, "isSetUsername", "()Z");
    user_isEmailSet = env->GetMethodID(userClass, "isSetEmail", "()Z");
    user_isNameSet = env->GetMethodID(userClass, "isSetName", "()Z");
    user_isTimezoneSet = env->GetMethodID(userClass, "isSetTimezone", "()Z");
    user_isPrivilegeLevelSet = env->GetMethodID(userClass, "isSetPrivilege", "()Z");
    user_isCreatedSet = env->GetMethodID(userClass, "isSetCreated", "()Z");
    user_isUpdatedSet = env->GetMethodID(userClass, "isSetUpdated", "()Z");
    user_isDeletedSet = env->GetMethodID(userClass, "isSetDeleted", "()Z");
    user_isActiveSet = env->GetMethodID(userClass, "isSetActive", "()Z");
    user_isShardIDSet = env->GetMethodID(userClass, "isSetShardId", "()Z");
    user_isAttributesSet = env->GetMethodID(userClass, "isSetAttributes", "()Z");
    user_isAccountingSet = env->GetMethodID(userClass, "isSetAccounting", "()Z");

    // Class & method used for user attributes
    userAttributesClass = env->FindClass("com/evernote/edam/type/UserAttributes");
    userAttributes_getDefaultLocationName = env->GetMethodID(userAttributesClass, "getDefaultLocationName", "()Ljava/lang/String;");
    userAttributes_isdefaultLocationNameSet = env->GetMethodID(userAttributesClass, "isSetDefaultLocationName", "()Z");
    userAttributes_getDefaultLatitude = env->GetMethodID(userAttributesClass, "getDefaultLatitude", "()D");
    userAttributes_isDefaultLatitudeSet = env->GetMethodID(userAttributesClass, "isSetDefaultLatitude", "()Z");
    userAttributes_getDefaultLongitude = env->GetMethodID(userAttributesClass, "getDefaultLongitude", "()D");
    userAttributes_isDefaultLongitudeSet = env->GetMethodID(userAttributesClass, "isSetDefaultLongitude", "()Z");
    userAttributes_getPreactivation = env->GetMethodID(userAttributesClass, "isPreactivation", "()Z");
    userAttributes_isPreactivationSet = env->GetMethodID(userAttributesClass, "isSetPreactivation", "()Z");
    userAttributes_incomingEmailAddress = env->GetMethodID(userAttributesClass, "getIncomingEmailAddress", "()Ljava/lang/String;");
    userAttributes_isIncomingEmailAddressSet = env->GetMethodID(userAttributesClass, "isSetIncomingEmailAddress", "()Z");
    userAttributes_getComments = env->GetMethodID(userAttributesClass, "getComments", "()Ljava/lang/String;");
    userAttributes_isCommentsSet = env->GetMethodID(userAttributesClass, "isSetComments", "()Z");
    userAttributes_getDateAgreedToTermsOfService = env->GetMethodID(userAttributesClass, "getDateAgreedToTermsOfService", "()J");
    userAttributes_isDateAgreedToTermsOfServiceSet = env->GetMethodID(userAttributesClass, "isSetDateAgreedToTermsOfService", "()Z");
    userAttributes_getMaxReferrals = env->GetMethodID(userAttributesClass, "getMaxReferrals", "()I");
    userAttributes_isMaxReferralsSet = env->GetMethodID(userAttributesClass, "isSetDefaultLocationName", "()Z");
    userAttributes_getReferralCount = env->GetMethodID(userAttributesClass, "getReferralCount", "()I");
    userAttributes_isReferralCountSet = env->GetMethodID(userAttributesClass, "isSetReferralCount", "()Z");
    userAttributes_getRefererCode = env->GetMethodID(userAttributesClass, "getRefererCode", "()Ljava/lang/String;");
    userAttributes_isRefererCodeSet = env->GetMethodID(userAttributesClass, "isSetRefererCode", "()Z");
    userAttributes_getSentEmailDate = env->GetMethodID(userAttributesClass, "getSentEmailDate", "()J");
    userAttributes_isSentEmailDateSet = env->GetMethodID(userAttributesClass, "isSetSentEmailDate", "()Z");
    userAttributes_getSentEmailCount = env->GetMethodID(userAttributesClass, "getSentEmailCount", "()I");
    userAttributes_isSentEmailCountSet = env->GetMethodID(userAttributesClass, "isSetSentEmailCount", "()Z");
    userAttributes_getDailyEmailLimit = env->GetMethodID(userAttributesClass, "getDailyEmailLimit", "()I");
    userAttributes_isDailyEmailLimitSet = env->GetMethodID(userAttributesClass, "isSetDailyEmailLimit", "()Z");
    userAttributes_getEmailOptOutDate  = env->GetMethodID(userAttributesClass, "getEmailOptOutDate", "()J");
    userAttributes_isEmailOptOutDateSet = env->GetMethodID(userAttributesClass, "isSetEmailOptOutDate", "()Z");
    userAttributes_getPreferredLanguage = env->GetMethodID(userAttributesClass, "getPreferredLanguage", "()Ljava/lang/String;");
    userAttributes_isPreferreredLanguageSet = env->GetMethodID(userAttributesClass, "isSetPreferredLanguage", "()Z");
    userAttributes_getClipFullPage = env->GetMethodID(userAttributesClass, "isClipFullPage", "()Z");
    userAttributes_isClipFullPageSet = env->GetMethodID(userAttributesClass, "isSetClipFullPage", "()Z");
    userAttributes_getTwitterID = env->GetMethodID(userAttributesClass, "getTwitterId", "()Ljava/lang/String;");
    userAttributes_isTwitterIDSet = env->GetMethodID(userAttributesClass, "isSetTwitterId", "()Z");
    userAttributes_getGroupName = env->GetMethodID(userAttributesClass, "getGroupName", "()Ljava/lang/String;");
    userAttributes_isGroupNameSet = env->GetMethodID(userAttributesClass, "isSetGroupName", "()Z");
    userAttributes_getRecognitionLanguage = env->GetMethodID(userAttributesClass, "getRecognitionLanguage", "()Ljava/lang/String;");
    userAttributes_isRecognitionLanguageSet = env->GetMethodID(userAttributesClass, "isSetRecognitionLanguage", "()Z");
    userAttributes_getCustomerProfileID = env->GetMethodID(userAttributesClass, "getCustomerProfileId", "()J");
    userAttributes_isCustomerProfileIDSet = env->GetMethodID(userAttributesClass, "isSetCustomerProfileId", "()Z");
    userAttributes_getReferralProof = env->GetMethodID(userAttributesClass, "getReferralProof", "()Ljava/lang/String;");
    userAttributes_isReferralProofSet = env->GetMethodID(userAttributesClass, "isSetReferralProof", "()Z");
    userAttributes_getEducationalDiscount = env->GetMethodID(userAttributesClass, "isEducationalDiscount", "()Z");
    userAttributes_isEducationalDiscountSet = env->GetMethodID(userAttributesClass, "isSetEducationalDiscount", "()Z");
    userAttributes_getBusinessAddress = env->GetMethodID(userAttributesClass, "getBusinessAddress", "()Ljava/lang/String;");
    userAttributes_isBusinessAddressSet = env->GetMethodID(userAttributesClass, "isSetBusinessAddress", "()Z");
    userAttributes_getHideSponsorBilling = env->GetMethodID(userAttributesClass, "isHideSponsorBilling", "()Z");
    userAttributes_isHideSponsorBillingSet = env->GetMethodID(userAttributesClass, "isSetHideSponsorBilling", "()Z");

    accountingClass = env->FindClass("com/evernote/edam/type/Accounting");
    accounting_getUploadLimit = env->GetMethodID(accountingClass, "getUploadLimit", "()J");
    accounting_isUploadLimitSet = env->GetMethodID(accountingClass, "isSetUploadLimit", "()Z");
    accounting_getUploadLimitEnd  = env->GetMethodID(accountingClass, "getUploadLimitEnd", "()J");
    accounting_isUploadLimitEndSet  = env->GetMethodID(accountingClass, "isSetUploadLimitEnd", "()Z");
    accounting_getUploadLimitNextMonth  = env->GetMethodID(accountingClass, "getUploadLimitNextMonth", "()J");
    accounting_isUploadLimitNextMonthSet = env->GetMethodID(accountingClass, "isSetUploadLimitNextMonth", "()Z");
    accounting_getPremiumServiceStatus = env->GetMethodID(accountingClass, "getPremiumServiceStatus", "()Lcom/evernote/edam/type/PremiumOrderStatus;");
    accounting_isPremiumServiceStatusSet = env->GetMethodID(accountingClass, "isSetPremiumServiceStatus", "()Z");
    accounting_getPremiumServiceStart = env->GetMethodID(accountingClass, "getPremiumServiceStart", "()J");
    accounting_isPremiumServiceStartSet = env->GetMethodID(accountingClass, "isSetPremiumServiceStart", "()Z");
    accounting_getPremiumOrderNumber = env->GetMethodID(accountingClass, "getPremiumOrderNumber", "()Ljava/lang/String;");
    accounting_isUPremiumOrderNumberSet = env->GetMethodID(accountingClass, "isSetPremiumOrderNumber", "()Z");
    accounting_getPremiumCommerceService = env->GetMethodID(accountingClass, "getPremiumCommerceService", "()Ljava/lang/String;");
    accounting_isPremiumCommerceServiceSet = env->GetMethodID(accountingClass, "isSetPremiumCommerceService", "()Z");
    accounting_getLastSuccessfulCharge =env->GetMethodID(accountingClass, "getLastSuccessfulCharge", "()J");
    accounting_isLastSuccessfulChargeSet = env->GetMethodID(accountingClass, "isSetLastSuccessfulCharge", "()Z");
    accounting_getLastFailedCharge = env->GetMethodID(accountingClass, "getLastFailedCharge", "()J");
    accounting_isLastFailedChargeSet = env->GetMethodID(accountingClass, "isSetLastFailedCharge", "()Z");
    accounting_getLastFailedChargeReason = env->GetMethodID(accountingClass, "getLastFailedChargeReason", "()Ljava/lang/String;");
    accounting_isLastFailedChargeReasonSet = env->GetMethodID(accountingClass, "isSetLastFailedChargeReason", "()Z");
    accounting_getNextPaymentDue = env->GetMethodID(accountingClass, "getNextPaymentDue", "()J");
    accounting_isNextPaymentDueSet = env->GetMethodID(accountingClass, "isSetNextPaymentDue", "()Z");
    accounting_getPremiumLockUntil = env->GetMethodID(accountingClass, "getPremiumLockUntil", "()J");
    accounting_isPremiumLockUntilSet = env->GetMethodID(accountingClass, "isSetPremiumLockUntil", "()Z");
    accounting_getUpdated = env->GetMethodID(accountingClass, "getUpdated", "()J");
    accounting_isUpdatedSet = env->GetMethodID(accountingClass, "isSetUpdated", "()Z");
    accounting_getPremiumSubscriptionNumber = env->GetMethodID(accountingClass, "getPremiumSubscriptionNumber", "()Ljava/lang/String;");
    accounting_isPremiumSubscriptionNumberSet = env->GetMethodID(accountingClass, "isSetPremiumSubscriptionNumber", "()Z");
    accounting_getLastRequestedCharge = env->GetMethodID(accountingClass, "getLastRequestedCharge", "()J");
    accounting_isLastRequestedChargeSet = env->GetMethodID(accountingClass, "isSetLastRequestedCharge", "()Z");
    accounting_getCurrency = env->GetMethodID(accountingClass, "getCurrency", "()Ljava/lang/String;");
    accounting_isCurrencySet = env->GetMethodID(accountingClass, "isSetCurrency", "()Z");
    accounting_getUnitPrice = env->GetMethodID(accountingClass, "getUnitPrice", "()I");
    accounting_isUnitPriceSet = env->GetMethodID(accountingClass, "isSetUnitPrice", "()Z");


    dataClass = env->FindClass("com/evernote/edam/type/Data");
    data_getBodyHash = env->GetMethodID(dataClass, "getBodyHash", "()[B");
    data_isBodyHashSet = env->GetMethodID(dataClass, "isSetBodyHash", "()Z");
    data_getSize = env->GetMethodID(dataClass, "getSize", "()I");
    data_isSizeSet = env->GetMethodID(dataClass, "isSetSize", "()Z");
    data_getBody = env->GetMethodID(dataClass, "getBody", "()[B");
    data_isBodySet = env->GetMethodID(dataClass, "isSetBody", "()Z");


    tagClass = env->FindClass("com/evernote/edam/type/Tag");
    tag_getGuid = env->GetMethodID(tagClass, "getGuid", "()Ljava/lang/String;");
    tag_isGuidSet = env->GetMethodID(tagClass, "isSetGuid", "()Z");
    tag_getName = env->GetMethodID(tagClass, "getName", "()Ljava/lang/String;");
    tag_isNameSet = env->GetMethodID(tagClass, "isSetName", "()Z");
    tag_getParentGuid = env->GetMethodID(tagClass, "getParentGuid", "()Ljava/lang/String;");
    tag_isParentGuidSet = env->GetMethodID(tagClass, "isSetParentGuid", "()Z");
    tag_getSequenceNumber = env->GetMethodID(tagClass, "getUpdateSequenceNum", "()I");
    tag_isSequenceNumberSet = env->GetMethodID(tagClass, "isSetUpdateSequenceNum", "()Z");

    resourceAttributesClass = env->FindClass("com/evernote/edam/type/ResourceAttributes");
    resourceAttributes_getSourceUrl = env->GetMethodID(resourceAttributesClass, "getSourceURL", "()Ljava/lang/String;");
    resourceAttributes_isSourceUrlSet = env->GetMethodID(resourceAttributesClass, "isSetSourceURL", "()Z");
    resourceAttributes_getTimestamp = env->GetMethodID(resourceAttributesClass, "getTimestamp", "()J");
    resourceAttributes_isTimestampSet = env->GetMethodID(resourceAttributesClass, "isSetTimestamp", "()Z");
    resourceAttributes_getLatitude = env->GetMethodID(resourceAttributesClass, "getLatitude", "()D");
    resourceAttributes_isLatitudeSet = env->GetMethodID(resourceAttributesClass, "isSetLatitude", "()Z");
    resourceAttributes_getLongitude = env->GetMethodID(resourceAttributesClass, "getLongitude", "()D");
    resourceAttributes_isLongitudeSet = env->GetMethodID(resourceAttributesClass, "isSetLongitude", "()Z");
    resourceAttributes_getAltitude = env->GetMethodID(resourceAttributesClass, "getAltitude", "()D");
    resourceAttributes_isAltitudeSet= env->GetMethodID(resourceAttributesClass, "isSetAltitude", "()Z");
    resourceAttributes_getCameraMake = env->GetMethodID(resourceAttributesClass, "getCameraMake", "()Ljava/lang/String;");
    resourceAttributes_isCameraMakeSet = env->GetMethodID(resourceAttributesClass, "isSetCameraMake", "()Z");
    resourceAttributes_getCameraModel = env->GetMethodID(resourceAttributesClass, "getCameraModel", "()Ljava/lang/String;");
    resourceAttributes_isCameraModelSet = env->GetMethodID(resourceAttributesClass, "isSetCameraModel", "()Z");
    resourceAttributes_getClientWillIndex = env->GetMethodID(resourceAttributesClass, "isClientWillIndex", "()Z");
    resourceAttributes_isClientWillIndexSet = env->GetMethodID(resourceAttributesClass, "isSetClientWillIndex", "()Z");
    resourceAttributes_getRecoType = env->GetMethodID(resourceAttributesClass, "getRecoType", "()Ljava/lang/String;");
    resourceAttributes_isRecoTypeSet = env->GetMethodID(resourceAttributesClass, "isSetRecoType", "()Z");
    resourceAttributes_getFileName = env->GetMethodID(resourceAttributesClass, "getFileName", "()Ljava/lang/String;");
    resourceAttributes_isFileNameSet = env->GetMethodID(resourceAttributesClass, "isSetFileName", "()Z");
    resourceAttributes_getAttachment = env->GetMethodID(resourceAttributesClass, "isAttachment", "()Z");
    resourceAttributes_isAttachmentSet = env->GetMethodID(resourceAttributesClass, "isSetAttachment", "()Z");
    resourceAttributes_getApplicationData = env->GetMethodID(resourceAttributesClass, "getApplicationData", "()Lcom/evernote/edam/type/LazyMap;");
    resourceAttributes_isApplicationDataSet = env->GetMethodID(resourceAttributesClass, "isSetApplicationData", "()Z");

    resourceClass = env->FindClass("com/evernote/edam/type/Resource");
    resource_getGuid = env->GetMethodID(resourceClass, "getGuid", "()Ljava/lang/String;");
    resource_isGuidSet = env->GetMethodID(resourceClass, "isSetGuid", "()Z");
    resource_getNoteGuid = env->GetMethodID(resourceClass, "getNoteGuid", "()Ljava/lang/String;");
    resource_isNoteGuidSet = env->GetMethodID(resourceClass, "isSetNoteGuid", "()Z");
    resource_getData  = env->GetMethodID(resourceClass, "getData", "()Lcom/evernote/edam/type/Data;");
    resource_isDataSet = env->GetMethodID(resourceClass, "isSetData", "()Z");
    resource_getMime = env->GetMethodID(resourceClass, "getMime", "()Ljava/lang/String;");;
    resource_isMimeSet = env->GetMethodID(resourceClass, "isSetMime", "()Z");
    resource_getWidth = env->GetMethodID(resourceClass, "getWidth", "()S");
    resource_isWidthSet = env->GetMethodID(resourceClass, "isSetWidth", "()Z");
    resource_getHeight = env->GetMethodID(resourceClass, "getHeight", "()S");
    resource_isHeightSet = env->GetMethodID(resourceClass, "isSetHeight", "()Z");
    resource_getDuration = env->GetMethodID(resourceClass, "getDuration", "()S");
    resource_isDurationSet  = env->GetMethodID(resourceClass, "isSetDuration", "()Z");
    resource_getActive  = env->GetMethodID(resourceClass, "isActive", "()Z");
    resource_isActiveSet = env->GetMethodID(resourceClass, "isSetActive", "()Z");
    resource_getRecognition = env->GetMethodID(resourceClass, "getRecognition", "()Lcom/evernote/edam/type/Data;");
    resource_isRecognitionSet = env->GetMethodID(resourceClass, "isSetRecognition", "()Z");
    resource_getAttributes = env->GetMethodID(resourceClass, "getAttributes", "()Lcom/evernote/edam/type/ResourceAttributes;");
    resource_isAttributesSet = env->GetMethodID(resourceClass, "isSetAttributes", "()Z");
    resource_getUpdateSequenceNumber = env->GetMethodID(resourceClass, "getUpdateSequenceNum", "()I");;
    resource_isUpdateSequenceNumberSet  = env->GetMethodID(resourceClass, "isSetUpdateSequenceNum", "()Z");
    resource_getAlternateData = env->GetMethodID(resourceClass, "getAlternateData", "()Lcom/evernote/edam/type/Data;");
    resource_isAlternateDataSet = env->GetMethodID(resourceClass, "isSetAlternateData", "()Z");

    noteAttributesClass = env->FindClass("com/evernote/edam/type/NoteAttributes");
    noteAttributes_getSubjectDate = env->GetMethodID(noteAttributesClass, "getSubjectDate", "()J");
    noteAttributes_isSubjectDateSet = env->GetMethodID(noteAttributesClass, "isSetSubjectDate", "()Z");
    noteAttributes_getLatitude  = env->GetMethodID(noteAttributesClass, "getLatitude", "()D");;
    noteAttributes_isLatitudeSet = env->GetMethodID(noteAttributesClass, "isSetLatitude", "()Z");
    noteAttributes_getLongitude = env->GetMethodID(noteAttributesClass, "getLongitude", "()D");
    noteAttributes_isLongitudeSet = env->GetMethodID(noteAttributesClass, "isSetLongitude", "()Z");
    noteAttributes_getAltitude  = env->GetMethodID(noteAttributesClass, "getAltitude", "()D");
    noteAttributes_isAltitudeSet = env->GetMethodID(noteAttributesClass, "isSetAltitude", "()Z");
    noteAttributes_getAuthor = env->GetMethodID(noteAttributesClass, "getAuthor", "()Ljava/lang/String;");
    noteAttributes_isAuthorSet = env->GetMethodID(noteAttributesClass, "isSetAuthor", "()Z");
    noteAttributes_getSource  = env->GetMethodID(noteAttributesClass, "getSource", "()Ljava/lang/String;");
    noteAttributes_isSourceSet = env->GetMethodID(noteAttributesClass, "isSetSource", "()Z");
    noteAttributes_getSourceURL = env->GetMethodID(noteAttributesClass, "getSourceURL", "()Ljava/lang/String;");
    noteAttributes_isSourceURLSet = env->GetMethodID(noteAttributesClass, "isSetSourceURL", "()Z");
    noteAttributes_getSourceApplication  = env->GetMethodID(noteAttributesClass, "getSourceApplication", "()Ljava/lang/String;");
    noteAttributes_isSourceApplicationSet = env->GetMethodID(noteAttributesClass, "isSetSourceApplication", "()Z");
    noteAttributes_getShareDate  = env->GetMethodID(noteAttributesClass, "getShareDate", "()J");
    noteAttributes_isShareDateSet = env->GetMethodID(noteAttributesClass, "isSetShareDate", "()Z");
    noteAttributes_getPlaceName = env->GetMethodID(noteAttributesClass, "getPlaceName", "()Ljava/lang/String;");
    noteAttributes_isPlaceNameSet = env->GetMethodID(noteAttributesClass, "isSetPlaceName", "()Z");
    noteAttributes_getContentClass  = env->GetMethodID(noteAttributesClass, "getContentClass", "()Ljava/lang/String;");
    noteAttributes_isContentClassSet = env->GetMethodID(noteAttributesClass, "isSetContentClass", "()Z");
    noteAttributes_getApplicationData  = env->GetMethodID(noteAttributesClass, "getApplicationData", "()Lcom/evernote/edam/type/LazyMap;");
    noteAttributes_isApplicationDataSet = env->GetMethodID(noteAttributesClass, "isSetApplicationData", "()Z");
    noteAttributes_getLastEditedBy  = env->GetMethodID(noteAttributesClass, "getLastEditedBy", "()Ljava/lang/String;");
    noteAttributes_isLastEditedBySet = env->GetMethodID(noteAttributesClass, "isSetLastEditedBy", "()Z");


    lazyMapClass = env->FindClass("com/evernote/edam/type/LazyMap");

    noteClass = env->FindClass("com/evernote/edam/type/Note");
    noteClass_getGuid = env->GetMethodID(noteClass, "getGuid", "()Ljava/lang/String;");
    noteClass_isGuidSet = env->GetMethodID(noteClass, "isSetGuid", "()Z");
    noteClass_getTitle  = env->GetMethodID(noteClass, "getTitle", "()Ljava/lang/String;");
    noteClass_isTitleSet  = env->GetMethodID(noteClass, "isSetTitle", "()Z");
    noteClass_getContent = env->GetMethodID(noteClass, "getContent", "()Ljava/lang/String;");
    noteClass_isContentSet = env->GetMethodID(noteClass, "isSetContent", "()Z");
    noteClass_getContentHash = env->GetMethodID(noteClass, "getContentHash", "()[B");
    noteClass_isContentHashSet  = env->GetMethodID(noteClass, "isSetContentHash", "()Z");
    noteClass_getContentLength  = env->GetMethodID(noteClass, "getContentLength", "()I");
    noteClass_isContentLengthSet  = env->GetMethodID(noteClass, "isSetContentLength", "()Z");
    noteClass_getCreated = env->GetMethodID(noteClass, "getCreated", "()J");
    noteClass_isCreatedSet = env->GetMethodID(noteClass, "isSetCreated", "()Z");
    noteClass_getUpdated = env->GetMethodID(noteClass, "getUpdated", "()J");
    noteClass_isUpdatedSet = env->GetMethodID(noteClass, "isSetUpdated", "()Z");
    noteClass_getDeleted = env->GetMethodID(noteClass, "getDeleted", "()J");
    noteClass_isDeletedSet  = env->GetMethodID(noteClass, "isSetDeleted", "()Z");
    noteClass_getActive = env->GetMethodID(noteClass, "isActive", "()Z");
    noteClass_isActiveSet = env->GetMethodID(noteClass, "isSetActive", "()Z");
    noteClass_getUpdateSequenceNumber = env->GetMethodID(noteClass, "getUpdateSequenceNum", "()I");
    noteClass_isUpdateSequenceNumberSet  = env->GetMethodID(noteClass, "isSetUpdateSequenceNum", "()Z");
    noteClass_getNotebookGuid = env->GetMethodID(noteClass, "getNotebookGuid", "()Ljava/lang/String;");
    noteClass_isNotebookGuidSet = env->GetMethodID(noteClass, "isSetNotebookGuid", "()Z");
    noteClass_getTagGuids = env->GetMethodID(noteClass, "getTagGuids", "()Ljava/util/List;");
    noteClass_isTagGuidsSet = env->GetMethodID(noteClass, "isSetTagGuids", "()Z");
    noteClass_getTagGuidsSize = env->GetMethodID(noteClass, "getTagGuidsSize", "()I");
    noteClass_getResources  = env->GetMethodID(noteClass, "getResources", "()Ljava/util/List;");
    noteClass_isResourcesSet = env->GetMethodID(noteClass, "isSetResources", "()Z");
    noteClass_getAttributes = env->GetMethodID(noteClass, "getAttributes", "()Lcom/evernote/edam/type/NoteAttributes;");
    noteClass_isAttributesSet  = env->GetMethodID(noteClass, "isSetAttributes", "()Z");
    noteClass_getTagNames = env->GetMethodID(noteClass, "getTagNames", "()Ljava/util/List;");
    noteClass_isTagNamesSet = env->GetMethodID(noteClass, "isSetTagNames", "()Z");
    noteClass_getTagNamesSize = env->GetMethodID(noteClass, "getTagNamesSize", "()I");
    noteClass_getResourcesSize = env->GetMethodID(noteClass, "getResourcesSize", "()I");

    publishingClass = env->FindClass("com/evernote/edam/type/Publishing");
    publishingClass_getUri = env->GetMethodID(publishingClass, "getUri", "()Ljava/lang/String;");
    publishingClass_isUriSet = env->GetMethodID(publishingClass, "isSetUri", "()Z");
    publishingClass_getOrder  = env->GetMethodID(publishingClass, "getUri", "()Ljava/lang/String;");
    publishingClass_isOrderSet = env->GetMethodID(publishingClass, "isSetOrder", "()Z");
    publishingClass_getAscending = env->GetMethodID(publishingClass, "isAscending", "()Z");
    publishingClass_isAscendingSet = env->GetMethodID(publishingClass, "isSetAscending", "()Z");
    publishingClass_getPublicDescription  = env->GetMethodID(publishingClass, "getPublicDescription", "()Ljava/lang/String;");
    publishingClass_isPublicDescriptionSet = env->GetMethodID(publishingClass, "isSetPublicDescription", "()Z");

    notebookClass = env->FindClass("com/evernote/edam/type/Notebook");
    notebook_getGuid = env->GetMethodID(notebookClass, "getGuid", "()Ljava/lang/String;");
    notebook_isGuidSet = env->GetMethodID(notebookClass, "isSetGuid", "()Z");
    notebook_getName = env->GetMethodID(notebookClass, "getName", "()Ljava/lang/String;");
    notebook_isNameSet = env->GetMethodID(notebookClass, "isSetName", "()Z");
    notebook_getUpdateSequenceNumber = env->GetMethodID(notebookClass, "getUpdateSequenceNum", "()I");
    notebook_isUpdateSequenceNumberSet = env->GetMethodID(notebookClass, "isSetUpdateSequenceNum", "()Z");
    notebook_getDefaultNotebook = env->GetMethodID(notebookClass, "isDefaultNotebook", "()Z");
    notebook_isDefaultNotebookSet = env->GetMethodID(notebookClass, "isSetDefaultNotebook", "()Z");
    notebook_getServiceCreated = env->GetMethodID(notebookClass, "getServiceCreated", "()J");
    notebook_isServiceCreatedSet = env->GetMethodID(notebookClass, "isSetServiceCreated", "()Z");
    notebook_getServiceUpdated = env->GetMethodID(notebookClass, "getServiceUpdated", "()J");
    notebook_isServiceUpdatedSet = env->GetMethodID(notebookClass, "isSetServiceUpdated", "()Z");
    notebook_getPublishing = env->GetMethodID(notebookClass, "getPublishing", "()Lcom/evernote/edam/type/Publishing;");
    notebook_isPublishingSet = env->GetMethodID(notebookClass, "isSetPublishing", "()Z");
    notebook_getPublished = env->GetMethodID(notebookClass, "isPublished", "()Z");
    notebook_isPublishedSet  = env->GetMethodID(notebookClass, "isSetPublished", "()Z");
    notebook_getStack  = env->GetMethodID(notebookClass, "getStack", "()Ljava/lang/String;");
    notebook_isStackSet = env->GetMethodID(notebookClass, "isSetStack", "()Z");
    notebook_getSharedNotebookIDs = env->GetMethodID(notebookClass, "getSharedNotebookIds", "()Ljava/util/List;");
    notebook_isSharedNotebookIDsSet = env->GetMethodID(notebookClass, "isSetSharedNotebookIds", "()Z");

    savedSearchClass = env->FindClass("com/evernote/edam/type/SavedSearch");
    savedSearch_getGuid  = env->GetMethodID(savedSearchClass, "getGuid", "()Ljava/lang/String;");
    savedSearch_isGuidSet = env->GetMethodID(savedSearchClass, "isSetGuid", "()Z");
    savedSearch_getName = env->GetMethodID(savedSearchClass, "getName", "()Ljava/lang/String;");
    savedSearch_isNameSet = env->GetMethodID(savedSearchClass, "isSetName", "()Z");
    savedSearch_getQuery  = env->GetMethodID(savedSearchClass, "getQuery", "()Ljava/lang/String;");
    savedSearch_isQuerySet = env->GetMethodID(savedSearchClass, "isSetQuery", "()Z");
    savedSearch_getFormat =  env->GetMethodID(savedSearchClass, "getFormat", "()Lcom/evernote/edam/type/QueryFormat;");
    savedSearch_isFormatSet = env->GetMethodID(savedSearchClass, "isSetFormat", "()Z");
    savedSearch_getUpdateSequenceNumber  = env->GetMethodID(savedSearchClass, "getUpdateSequenceNum", "()I");
    savedSearch_isUpdateSequenceNumberSet = env->GetMethodID(savedSearchClass, "isSetUpdateSequenceNum", "()Z");

    queryFormatClass = env->FindClass("com/evernote/edam/type/QueryFormat");
    queryFormat_toString = env->GetMethodID(queryFormatClass, "toString", "()Ljava/lang/String;");

    sharedNotebookClass = env->FindClass("com/evernote/edam/type/SharedNotebook");
    sharedNotebook_getID = env->GetMethodID(sharedNotebookClass, "getId", "()J");
    sharedNotebook_isIDSet = env->GetMethodID(sharedNotebookClass, "isSetId", "()Z");
    sharedNotebook_getUserID = env->GetMethodID(sharedNotebookClass, "getUserId", "()I");
    sharedNotebook_isUserIDSet = env->GetMethodID(sharedNotebookClass, "isSetUserId", "()Z");
    sharedNotebook_getNotebookGuid = env->GetMethodID(sharedNotebookClass, "getNotebookGuid", "()Ljava/lang/String;");
    sharedNotebook_isNotebookGuidSet = env->GetMethodID(sharedNotebookClass, "isSetNotebookGuid", "()Z");
    sharedNotebook_getRequireLogin = env->GetMethodID(sharedNotebookClass, "isSetRequireLogin", "()Z");
    sharedNotebook_isRequireLoginSet = env->GetMethodID(sharedNotebookClass, "isSetRequireLogin", "()Z");
    sharedNotebook_getServiceCreated = env->GetMethodID(sharedNotebookClass, "getServiceCreated", "()J");
    sharedNotebook_isServiceCreatedSet = env->GetMethodID(sharedNotebookClass, "isSetServiceCreated", "()Z");
    sharedNotebook_getShareKey = env->GetMethodID(sharedNotebookClass, "getShareKey", "()Ljava/lang/String;");
    sharedNotebook_isShareKeySet = env->GetMethodID(sharedNotebookClass, "isSetShareKey", "()Z");
    sharedNotebook_getUsername = env->GetMethodID(sharedNotebookClass, "getUsername", "()Ljava/lang/String;");
    sharedNotebook_isUsernameSet = env->GetMethodID(sharedNotebookClass, "isSetUsername", "()Z");

    linkedNotebookClass = env->FindClass("com/evernote/edam/type/LinkedNotebook");
    linkedNotebook_getShareName = env->GetMethodID(linkedNotebookClass, "getShareName", "()Ljava/lang/String;");
    linkedNotebook_isShareNameSet = env->GetMethodID(linkedNotebookClass, "isSetShareName", "()Z");
    linkedNotebook_getUsername = env->GetMethodID(linkedNotebookClass, "getUsername", "()Ljava/lang/String;");
    linkedNotebook_isUsernameSet = env->GetMethodID(linkedNotebookClass, "isSetUsername", "()Z");
    linkedNotebook_getShardID = env->GetMethodID(linkedNotebookClass, "getShardId", "()Ljava/lang/String;");
    linkedNotebook_isShardIDSet = env->GetMethodID(linkedNotebookClass, "isSetShardId", "()Z");
    linkedNotebook_getUri = env->GetMethodID(linkedNotebookClass, "getUri", "()Ljava/lang/String;");
    linkedNotebook_isUriSet = env->GetMethodID(linkedNotebookClass, "isSetUri", "()Z");
    linkedNotebook_getGuid = env->GetMethodID(linkedNotebookClass, "getGuid", "()Ljava/lang/String;");
    linkedNotebook_isGuidSet = env->GetMethodID(linkedNotebookClass, "isSetGuid", "()Z");
    linkedNotebook_getUpdateSequenceNumber = env->GetMethodID(linkedNotebookClass, "getUpdateSequenceNum", "()I");
    linkedNotebook_isUpdateSequenceNumberSet = env->GetMethodID(linkedNotebookClass, "isSetUpdateSequenceNum", "()Z");

    listClass = env->FindClass("java/util/List");
    list_get = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");

    delete cp2;
    return;
}



bool JavaMachine::connect() {
    // Attach to the current thread
    int status =jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if(status < 0)
    {
        status = jvm->AttachCurrentThread((void**)&env, NULL);
    }

    // Get the oAuth token
    QFile oauthFile(global.fileManager.getHomeDirPath("oauth.txt"));
    oauthFile.open(QIODevice::ReadOnly);
    OAuthTokenizer tokenizer;
    QByteArray data = oauthFile.readAll();
    oauthFile.close();
    tokenizer.tokenize(data);

    //jstring userid = env->NewStringUTF(global.username.c_str());
    //jstring password = env->NewStringUTF(global.password.c_str());
    jstring authToken = env->NewStringUTF(tokenizer.oauth_token.toStdString().c_str());

    // Call the actual java conenct method.  We get back a message block
    jboolean connected = env->CallBooleanMethod(communication, communication_connect, authToken);

    if (!connected) {
        jobject messageBlock = env->CallObjectMethod(communication, communication_getMessageBlock);

        // Convert the message block into something we can use
        MessageBlock mb;
        convertMessageBlock(mb, messageBlock);
        QLOG_DEBUG() << "Error Connecting: " << mb.errorMessage;
        global.connected = false;
        return false;
    }
    //        jobject syncStateObj = env->CallObjectMethod(communication, communication_getMessageBlock);
    global.connected = true;
    return true;
}

void JavaMachine::jString2QString(QString &qstr, jstring &jstring, bool release) {
    jboolean blnIsCopy;
    const char* strCIn = env->GetStringUTFChars(jstring, &blnIsCopy);
    qstr = QString(strCIn);
    if (release)
        env->ReleaseStringUTFChars(jstring , strCIn); // release jstring
}

void JavaMachine::jByteArray2QByteArray(QByteArray &qstr, jbyteArray &jb, bool release) {
    jbyte *b = env->GetByteArrayElements(jb, NULL);
    int len = env->GetArrayLength(jb);
    QByteArray ba((const char*)b,len);
    qstr = ba;
    if (release)
        env->ReleaseByteArrayElements(jb,b,0);
}



void JavaMachine::jString2String(string &qstr, jstring &jstring, bool release) {
    if (jstring == NULL) {
        qstr = "";
        return;
    }
    const char* strCIn = env->GetStringUTFChars(jstring, NULL);
    qstr = strCIn;
    if (release)
        env->ReleaseStringUTFChars(jstring , strCIn); // release jstring
}


bool JavaMachine::getSyncState(SyncState &syncState) {
    // If we are not connected, return
    if (!global.connected)
        return false;


    // Call the actual java conenct method.  We get back a message block
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    int status =jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if(status < 0)
    {
        status = jvm->AttachCurrentThread((void**)&env, NULL);
    }
    jobject syncStateObj = env->CallObjectMethod(communication, communication_getSyncState);

    if (syncStateObj == NULL) {
        jobject messageBlock = env->CallObjectMethod(communication, communication_getMessageBlock);

        // Convert the message block into something we can use
        MessageBlock mb;
        convertMessageBlock(mb, messageBlock);
        QLOG_DEBUG() << "Error getting Sync State: " << mb.errorMessage;
    }

    jboolean currentTimeSet = env->CallBooleanMethod(syncStateObj, syncState_isSetCurrentTime);
    jboolean updateCountSet = env->CallBooleanMethod(syncStateObj, syncState_isSetUpdateCount);
    jboolean fullSyncBeforeSet = env->CallBooleanMethod(syncStateObj, syncState_isSetFullSyncBefore);
    jboolean uploadedSet = env->CallBooleanMethod(syncStateObj, syncState_isSetUploaded);

    if (uploadedSet) {
        syncState.__isset.uploaded = true;
        syncState.uploaded = env->CallIntMethod(syncStateObj, syncState_getUploaded);
    }

    if (currentTimeSet) {
        syncState.currentTime = env->CallLongMethod(syncStateObj, syncState_getCurrentTime);
    }
    if (updateCountSet) {
        syncState.updateCount = env->CallIntMethod(syncStateObj, syncState_getUpdateCount);
    }
    if (fullSyncBeforeSet) {
        syncState.fullSyncBefore = env->CallLongMethod(syncStateObj, syncState_getFullSyncBefore);
    }

    return true;
}



void JavaMachine::convertMessageBlock(MessageBlock &mb, jobject messageBlock) {

    jboolean error = env->CallBooleanMethod(messageBlock, messageBlock_isError);
    jstring message = (jstring)env->CallObjectMethod(messageBlock, messageBlock_getErrorMessage);

    mb.error = error;
    jString2QString(mb.errorMessage, message);
}


bool JavaMachine::getSyncChunk(SyncChunk &chunk, int start, int chunkSize, bool fullSync) {
    // Call the actual java conenct method.  We get back a message block
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    int status =jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if(status < 0)
        status = jvm->AttachCurrentThread((void**)&env, NULL);

    syncChunk = env->CallObjectMethod(communication, communication_loadSyncChunk, start, chunkSize, fullSync);
    if (syncChunk == NULL) {
        QLOG_ERROR() << "Chunk is null!!!";
        return false;
    }
    chunk.chunkHighUSN = env->CallIntMethod(syncChunk, syncChunk_getChunkHighUSN);
    chunk.currentTime = env->CallLongMethod(syncChunk, syncChunk_getCurrentTime);

    // Get notes
    jboolean isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetNotes);
    jint size = env->CallIntMethod(syncChunk, syncChunk_getNotesSize);
    jobject list = env->CallObjectMethod(syncChunk, syncChunk_getNotes);
    if (isSet && size>0)
        processChunkNotes(chunk, list, size);

    // Get notebooks
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetNotebooks);
    size = env->CallIntMethod(syncChunk, syncChunk_getNotebooksSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getNotebooks);
    if (isSet && size>0)
        processChunkNotebooks(chunk, list, size);

    // Get Tags
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetTags);
    size = env->CallIntMethod(syncChunk, syncChunk_getTagsSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getTags);
    if (isSet && size>0)
        processChunkTags(chunk, list, size);

    // Get Searches
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetSearches);
    size = env->CallIntMethod(syncChunk, syncChunk_getSearchesSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getSearches);
    if (isSet && size>0)
        processChunkSearches(chunk, list, size);

    // Get Resources
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetResources);
    size = env->CallIntMethod(syncChunk, syncChunk_getResourcesSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getResources);
    if (isSet && size>0)
        processChunkResources(chunk, list, size);

    // Get Linked Notebooks
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetLinkedNotebooks);
    size = env->CallIntMethod(syncChunk, syncChunk_getLinkedNotebooksSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getLinkedNotebooks);
    if (isSet && size>0)
        processChunkLinkedNotebooks(chunk, list, size);


    // Get expunged notes
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetExpungedNotes);
    size = env->CallIntMethod(syncChunk, syncChunk_getExpungedNotesSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getExpungedNotes);
    if (isSet && size>0)
        processChunkExpungedNotes(chunk, list, size);

    // Get expunged notebooks
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetExpungedNotebooks);
    size = env->CallIntMethod(syncChunk, syncChunk_getExpungedNotebooksSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getExpungedNotebooks);
    if (isSet && size>0)
        processChunkExpungedNotebooks(chunk, list, size);

    // Get expunged tags
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetExpungedTags);
    size = env->CallIntMethod(syncChunk, syncChunk_getExpungedTagsSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getExpungedTags);
    if (isSet && size>0)
        processChunkExpungedTags(chunk, list, size);


    // Get expunged searches
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetExpungedSearches);
    size = env->CallIntMethod(syncChunk, syncChunk_getExpungedSearchesSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getExpungedSearches);
    if (isSet && size>0)
        processChunkExpungedSearches(chunk, list, size);

    // Get expunged notes
    isSet = env->CallBooleanMethod(syncChunk, syncChunk_isSetExpungedLinkedNotebooks);
    size = env->CallIntMethod(syncChunk, syncChunk_getExpungedLinkedNotebooksSize);
    list = env->CallObjectMethod(syncChunk, syncChunk_getExpungedLinkedNotebooks);
    if (isSet && size>0)
        processChunkExpungedLinkedNotebooks(chunk, list, size);

    env->DeleteLocalRef(syncChunk);
    env->DeleteLocalRef(list);

    return true;
}



void JavaMachine::processChunkNotes(SyncChunk &syncChunk, jobject list, int size) {
    for (int i=0;i<size;i++) {
        jobject jobj = env->CallObjectMethod(list, list_get, i);
        Note newNote;
        mapToNote(newNote, jobj);
        syncChunk.notes.push_back(newNote);
    }
}


void JavaMachine::processChunkTags(SyncChunk &syncChunk, jobject list, int size) {
    for (int i=0;i<size;i++) {
        jobject jobj = env->CallObjectMethod(list, list_get, i);
        Tag newTag;
        mapToTag(newTag, jobj);
        syncChunk.tags.push_back(newTag);
    }
}

void JavaMachine::processChunkNotebooks(SyncChunk &syncChunk, jobject list, int size) {
    for (int i=0;i<size;i++) {
        jobject jobj = env->CallObjectMethod(list, list_get, i);
        Notebook book;
        mapToNotebook(book, jobj);
        syncChunk.notebooks.push_back(book);
    }
}


void JavaMachine::processChunkSearches(SyncChunk &syncChunk, jobject jlist, int size) {
    for (int i=0;i<size;i++) {
        jobject jobj = env->CallObjectMethod(jlist, list_get, i);
        SavedSearch search;
        mapToSearch(search, jobj);
        syncChunk.searches.push_back(search);
    }
}

void JavaMachine::processChunkExpungedNotes(SyncChunk &syncChunk, jobject jlist, int size) {
    QStringList list = mapToStringList(jlist, size);
    for (int i=0; i<list.size(); i++)
        syncChunk.expungedNotes.push_back(list.at(i).toStdString());
}

void JavaMachine::processChunkExpungedNotebooks(SyncChunk &syncChunk, jobject jlist, int size) {
        QStringList list = mapToStringList(jlist, size);
        for (int i=0; i<list.size(); i++)
            syncChunk.expungedNotebooks.push_back(list.at(i).toStdString());
}

void JavaMachine::processChunkExpungedTags(SyncChunk &syncChunk, jobject jlist, int size) {
    QStringList list = mapToStringList(jlist, size);
    for (int i=0; i<list.size(); i++)
        syncChunk.expungedTags.push_back(list.at(i).toStdString());
}

void JavaMachine::processChunkExpungedSearches(SyncChunk &syncChunk, jobject jlist, int size) {
    QStringList list = mapToStringList(jlist, size);
    for (int i=0; i<list.size(); i++)
        syncChunk.expungedSearches.push_back(list.at(i).toStdString());
}

void JavaMachine::processChunkExpungedLinkedNotebooks(SyncChunk &syncChunk, jobject jlist, int size) {
    QStringList list = mapToStringList(jlist, size);
    for (int i=0; i<list.size(); i++)
        syncChunk.expungedLinkedNotebooks.push_back(list.at(i).toStdString());
}

void JavaMachine::processChunkResources(SyncChunk &syncChunk, jobject jlist, int size) {
    vector<Resource> resourceList;
    QLOG_DEBUG() << "Mapping resources";
    mapToResources(resourceList, jlist, size);
    QLOG_DEBUG() << "Mapping resources complete: " << resourceList.size() << " found.";
    for (unsigned int i=0; i<resourceList.size(); i++) {
        syncChunk.resources.push_back(resourceList[i]);
    }
    QLOG_DEBUG() << "Resource Chunk Processed";
}

void JavaMachine::processChunkLinkedNotebooks(SyncChunk &syncChunk, jobject list, int size) {
    for (int i=0;i<size;i++) {
        jobject jobj = env->CallObjectMethod(list, list_get, i);
        LinkedNotebook notebook;
        mapToLinkedNotebook(notebook, jobj);
        syncChunk.linkedNotebooks.push_back(notebook);
    }
}




void JavaMachine::mapToNote(Note &newNote, jobject jnote) {
    jstring value;
    bool isSet = env->CallBooleanMethod(jnote, noteClass_isGuidSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jnote, noteClass_getGuid);
        jString2String(newNote.guid, value);
        newNote.__isset.guid = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isTitleSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jnote, noteClass_getTitle);
        jString2String(newNote.title, value);
        newNote.__isset.title = true;
    }

    isSet = env->CallBooleanMethod(jnote, noteClass_isContentSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jnote, noteClass_getContent);
        jString2String(newNote.content, value);
        newNote.__isset.content = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isContentLengthSet);
    if (isSet) {
        newNote.contentLength = env->CallIntMethod(jnote, noteClass_getContentLength);
        newNote.__isset.content = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isContentHashSet);
    if (isSet) {
//        jbyteArray jb = (jbyteArray)env->CallObjectMethod(jnote, noteClass_getContentHash);
//        jbyte *b = env->GetByteArrayElements(jb, NULL);
//        int len = env->GetArrayLength(jb);
//        QByteArray ba;
//        this->jString2QByteArray(ba,jb);
//        QString s(ba.toHex());
//        newNote.contentHash = s.toStdString();
//        newNote.__isset.contentHash = true;
    }

    isSet = env->CallBooleanMethod(jnote, noteClass_isCreatedSet);
    if (isSet) {
        newNote.created = env->CallLongMethod(jnote, noteClass_getCreated);
        newNote.__isset.created = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isUpdatedSet);
    if (isSet) {
        newNote.updated = env->CallLongMethod(jnote, noteClass_getUpdated);
        newNote.__isset.updated = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isDeletedSet);
    if (isSet) {
        newNote.deleted = env->CallLongMethod(jnote, noteClass_getDeleted);
        newNote.__isset.deleted = true;
    }

    isSet = env->CallBooleanMethod(jnote, noteClass_isActiveSet);
    if (isSet) {
        newNote.active = env->CallBooleanMethod(jnote, noteClass_getActive);
        newNote.__isset.active = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isUpdateSequenceNumberSet);
    if (isSet) {
        newNote.updateSequenceNum = env->CallIntMethod(jnote, noteClass_getUpdateSequenceNumber);
        newNote.__isset.updateSequenceNum = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isNotebookGuidSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jnote, noteClass_getNotebookGuid);
        jString2String(newNote.notebookGuid, value);
        newNote.__isset.notebookGuid = true;
    }

    isSet = env->CallBooleanMethod(jnote, noteClass_isTagGuidsSet);
    if (isSet) {
        jobject list = env->CallObjectMethod(jnote, noteClass_getTagGuids);
        int size = env->CallIntMethod(jnote, noteClass_getTagGuidsSize);
        QStringList taglist = mapToStringList(list, size);
        for (int i=0; i<taglist.size(); i++) {
            newNote.tagGuids.push_back(taglist.at(i).toStdString());
        }
        newNote.__isset.tagGuids = true;
    }
    isSet = env->CallBooleanMethod(jnote, noteClass_isTagNamesSet);
    if (isSet) {
        jobject list = env->CallObjectMethod(jnote, noteClass_getTagNames);
        int size = env->CallIntMethod(jnote, noteClass_getTagNamesSize);
        QStringList nameList = mapToStringList(list, size);
        for (int i=0; i<nameList.size(); i++) {
            newNote.tagNames.push_back(nameList.at(i).toStdString());
        }
        newNote.__isset.tagNames = true;
    }

    isSet = env->CallBooleanMethod(jnote, noteClass_isAttributesSet);
    if (isSet) {
        jobject attributes = env->CallObjectMethod(jnote, noteClass_getAttributes);
        mapToAttributes(newNote.attributes, attributes);
        newNote.__isset.attributes = true;
    }

    isSet = env->CallBooleanMethod(jnote, noteClass_isResourcesSet);
    if (isSet) {
        jobject resources = env->CallObjectMethod(jnote, noteClass_getResources);
        int size = env->CallIntMethod(jnote, noteClass_getResourcesSize);
        mapToResources(newNote.resources, resources, size);
        newNote.__isset.resources = true;
    }
}

void JavaMachine::mapToAttributes(NoteAttributes &attributes, jobject jattrib) {
    bool isSet = env->CallBooleanMethod(jattrib, noteAttributes_isSubjectDateSet);
    if (isSet) {
        attributes.subjectDate = env->CallLongMethod(jattrib, noteAttributes_getSubjectDate);
        attributes.__isset.subjectDate = true;
    }

    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isLatitudeSet);
    if (isSet) {
        jdouble value = env->CallDoubleMethod(jattrib, noteAttributes_getLatitude);
        attributes.latitude = value;
        attributes.__isset.latitude = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isAltitudeSet);
    if (isSet) {
        jdouble value = env->CallDoubleMethod(jattrib, noteAttributes_getAltitude);
        attributes.altitude = value;
        attributes.__isset.altitude = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isLongitudeSet);
    if (isSet) {
        jdouble value = env->CallDoubleMethod(jattrib, noteAttributes_getLongitude);
        attributes.longitude = value;
        attributes.__isset.longitude = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isAuthorSet);
    if (isSet) {
        jstring value = (jstring)env->CallObjectMethod(jattrib, noteAttributes_getAuthor);
        jString2String(attributes.author,value);
        attributes.__isset.author = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isSourceSet);
    if (isSet) {
        jstring value = (jstring)env->CallObjectMethod(jattrib, noteAttributes_getSource);
        jString2String(attributes.source,value);
        attributes.__isset.source = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isSourceURLSet);
    if (isSet) {
        jstring value = (jstring)env->CallObjectMethod(jattrib, noteAttributes_getSourceURL);
        jString2String(attributes.sourceURL,value);
        attributes.__isset.sourceURL = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isSourceApplicationSet);
    if (isSet) {
        jstring value = (jstring)env->CallObjectMethod(jattrib, noteAttributes_getSourceApplication);
        jString2String(attributes.sourceApplication,value);
        attributes.__isset.sourceApplication = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isPlaceNameSet);
    if (isSet) {
        jstring value = (jstring)env->CallObjectMethod(jattrib, noteAttributes_getPlaceName);
        jString2String(attributes.placeName,value);
        attributes.__isset.placeName = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isContentClassSet);
    if (isSet) {
        jstring value = (jstring)env->CallObjectMethod(jattrib, noteAttributes_getContentClass);
        jString2String(attributes.contentClass,value);
        attributes.__isset.contentClass = true;
    }
    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isLastEditedBySet);
    if (isSet) {
        jstring value = (jstring)env->CallObjectMethod(jattrib, noteAttributes_getLastEditedBy);
        jString2String(attributes.lastEditedBy,value);
        attributes.__isset.lastEditedBy = true;
    }

    isSet = env->CallBooleanMethod(jattrib, noteAttributes_isShareDateSet);
    if (isSet) {
        attributes.shareDate = env->CallLongMethod(jattrib, noteAttributes_getShareDate);
        attributes.__isset.shareDate = true;
    }
    attributes.__isset.applicationData = false;
}

void JavaMachine::mapToNotebook(Notebook &newNotebook, jobject jnotebook) {
    jstring value;
    bool isSet = env->CallBooleanMethod(jnotebook, notebook_isGuidSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jnotebook, notebook_getGuid);
        jString2String(newNotebook.guid, value);
        newNotebook.__isset.guid = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isNameSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jnotebook, notebook_getName);
        jString2String(newNotebook.name, value);
        newNotebook.__isset.name = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isUpdateSequenceNumberSet);
    if (isSet) {
        newNotebook.updateSequenceNum = env->CallIntMethod(jnotebook, notebook_getUpdateSequenceNumber);
        newNotebook.__isset.updateSequenceNum = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isDefaultNotebookSet);
    if (isSet) {
        newNotebook.defaultNotebook = env->CallBooleanMethod(jnotebook, notebook_getDefaultNotebook);
        newNotebook.__isset.defaultNotebook = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isPublishedSet);
    if (isSet) {
        newNotebook.defaultNotebook = env->CallBooleanMethod(jnotebook, notebook_getPublished);
        newNotebook.__isset.published = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isServiceCreatedSet);
    if (isSet) {
        newNotebook.serviceCreated = env->CallLongMethod(jnotebook, notebook_getServiceCreated);
        newNotebook.__isset.serviceCreated = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isServiceUpdatedSet);
    if (isSet) {
        newNotebook.serviceUpdated = env->CallLongMethod(jnotebook, notebook_getServiceUpdated);
        newNotebook.__isset.serviceUpdated = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isStackSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jnotebook, notebook_getStack);
        jString2String(newNotebook.stack, value);
        newNotebook.__isset.serviceCreated = true;
    }

    isSet = env->CallBooleanMethod(jnotebook, notebook_isPublishingSet);
    if (isSet) {
        jobject jpublishing = env->CallObjectMethod(jnotebook, notebook_getPublishing);
        isSet = env->CallBooleanMethod(jpublishing, publishingClass_isUriSet);
        if (isSet) {
            value = (jstring)env->CallObjectMethod(jpublishing, publishingClass_getUri);
            jString2String(newNotebook.publishing.uri, value);
            newNotebook.publishing.__isset.uri = true;
        }

        isSet = env->CallBooleanMethod(jpublishing, publishingClass_isOrderSet);
        if (isSet) {
            value = (jstring)env->CallObjectMethod(jnotebook, publishingClass_getOrder);
            //jString2String(newNotebook.publishing.order, value);
            newNotebook.publishing.__isset.order = true;
        }

        isSet = env->CallBooleanMethod(jpublishing, publishingClass_isAscendingSet);
        if (isSet) {
            newNotebook.publishing.ascending = env->CallBooleanMethod(jpublishing, publishingClass_getAscending);
            newNotebook.publishing.__isset.ascending = true;
        }

        isSet = env->CallBooleanMethod(jpublishing, publishingClass_isPublicDescriptionSet);
        if (isSet) {
            value = (jstring)env->CallObjectMethod(jnotebook, publishingClass_getPublicDescription);
            jString2String(newNotebook.publishing.publicDescription, value);
            newNotebook.publishing.__isset.publicDescription = true;
        }
    }

}

void JavaMachine::mapToTag(Tag &newTag, jobject jtag) {
    jstring value;
    bool isSet = env->CallBooleanMethod(jtag, tag_isGuidSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jtag, tag_getGuid);
        jString2String(newTag.guid, value);
        newTag.__isset.guid = true;
    }

    isSet = env->CallBooleanMethod(jtag, tag_isNameSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jtag, tag_getName);
        jString2String(newTag.name, value);
        newTag.__isset.name = true;
    }

    isSet = env->CallBooleanMethod(jtag, tag_isParentGuidSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jtag, tag_getParentGuid);
        jString2String(newTag.parentGuid, value);
        newTag.__isset.parentGuid = true;
    }

    isSet = env->CallBooleanMethod(jtag, tag_isSequenceNumberSet);
    if (isSet) {
        newTag.updateSequenceNum = env->CallIntMethod(jtag, tag_getSequenceNumber);
        newTag.__isset.updateSequenceNum = true;
    }
}

void JavaMachine::mapToSearch(SavedSearch &newSearch, jobject jsearch) {
    jstring value;
    bool isSet = env->CallBooleanMethod(jsearch, savedSearch_isGuidSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jsearch, savedSearch_getGuid);
        jString2String(newSearch.guid, value);
        newSearch.__isset.guid = true;
    }

    isSet = env->CallBooleanMethod(jsearch, savedSearch_isNameSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jsearch, savedSearch_getName);
        jString2String(newSearch.name, value);
        newSearch.__isset.name = true;
    }

    isSet = env->CallBooleanMethod(jsearch, savedSearch_isQuerySet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jsearch, savedSearch_getQuery);
        jString2String(newSearch.query, value);
        newSearch.__isset.query = true;
    }

    isSet = env->CallBooleanMethod(jsearch, savedSearch_isFormatSet);
    if (isSet) {
        jobject qformat;
        qformat = (jobject)env->CallObjectMethod(jsearch, savedSearch_getFormat);
        value = (jstring)env->CallObjectMethod(qformat, queryFormat_toString);
        QString vs;
        jString2QString(vs,value);
        if (vs == "USER") {
            newSearch.format = USER;
        }
        if (vs == "SEXP") {
            newSearch.format = SEXP;
        }
        newSearch.__isset.query = true;
    }

    isSet = env->CallBooleanMethod(jsearch, savedSearch_isUpdateSequenceNumberSet);
    if (isSet) {
        newSearch.updateSequenceNum = env->CallIntMethod(jsearch, savedSearch_getUpdateSequenceNumber);
        newSearch.__isset.updateSequenceNum = true;
    }

}

void JavaMachine::mapToLinkedNotebook(LinkedNotebook &linkedNotebook, jobject notebook) {
    /* suppress unused */
    linkedNotebook = linkedNotebook;
    notebook=notebook;
}

void JavaMachine::mapToResources(vector<Resource> &newResList, jobject jresourceList, int size) {
    jclass listClass = env->FindClass("java/util/List");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    for (int i=0; i<size; i++) {
        Resource newRes;
        jobject jres;
        jres = env->CallObjectMethod(jresourceList, getMethod, i);
        jstring value;

        bool isSet = env->CallBooleanMethod(jres, resource_isGuidSet);
        if (isSet) {
            value = (jstring)env->CallObjectMethod(jres, resource_getGuid);
            jString2String(newRes.guid, value);
            newRes.__isset.guid = true;
        }

        isSet = env->CallBooleanMethod(jres, resource_isNoteGuidSet);
        if (isSet) {
            value = (jstring)env->CallObjectMethod(jres, resource_getNoteGuid);
            jString2String(newRes.noteGuid, value);
            newRes.__isset.noteGuid = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isMimeSet);
        if (isSet) {
            value = (jstring)env->CallObjectMethod(jres, resource_getMime);
            jString2String(newRes.mime, value);
            newRes.__isset.mime = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isHeightSet);
        if (isSet) {
            newRes.height = env->CallShortMethod(jres, resource_getHeight);
            newRes.__isset.height = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isWidthSet);
        if (isSet) {
            newRes.width = env->CallShortMethod(jres, resource_getWidth);
            newRes.__isset.width = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isDurationSet);
        if (isSet) {
            newRes.duration = env->CallShortMethod(jres, resource_getDuration);
            newRes.__isset.duration = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isUpdateSequenceNumberSet);
        if (isSet) {
            newRes.updateSequenceNum = env->CallIntMethod(jres, resource_getUpdateSequenceNumber);
            newRes.__isset.updateSequenceNum = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isActiveSet);
        if (isSet) {
            newRes.active = env->CallBooleanMethod(jres, resource_getActive);
            newRes.__isset.active = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isRecognitionSet);
        if (isSet) {
            jobject jdata = env->CallObjectMethod(jres, resource_getRecognition);
            mapToData(newRes.recognition, jdata);
            newRes.__isset.recognition = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isDataSet);
        if (isSet) {
            jobject jdata = env->CallObjectMethod(jres, resource_getData);
            mapToData(newRes.data, jdata);
            newRes.__isset.data = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isAlternateDataSet);
        if (isSet) {
            jobject jdata = env->CallObjectMethod(jres, resource_getAlternateData);
            mapToData(newRes.alternateData, jdata);
            newRes.__isset.alternateData = true;
        }
        isSet = env->CallBooleanMethod(jres, resource_isAttributesSet);
        if (isSet) {
            jobject jdata = env->CallObjectMethod(jres, resource_getAttributes);
            mapToResourceAttributes(newRes.attributes, jdata);
            newRes.__isset.attributes = true;
        }
        newResList.push_back(newRes);
    }
}


QStringList JavaMachine::mapToStringList(jobject obj, int size) {
    QStringList stringList;
    jclass listClass = env->FindClass("java/util/List");
    jmethodID getMethod = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
    for (int i=0; i<size; i++) {
        jstring item = (jstring)env->CallObjectMethod(obj, getMethod, i);
        QString text;
        jString2QString(text, item);
        stringList.append(text);
    }
    return stringList;

}

void JavaMachine::mapToData(Data &newData, jobject jdata) {
    int size = 0;
    bool isSet = env->CallBooleanMethod(jdata, data_isBodyHashSet);
    if (isSet) {
        jbyteArray jb = (jbyteArray)env->CallObjectMethod(jdata, data_getBodyHash);
        QByteArray ba;
        this->jByteArray2QByteArray(ba,jb, 16);
        newData.bodyHash = ba.data();
        newData.__isset.bodyHash = true;
    }
    isSet = env->CallBooleanMethod(jdata, data_isSizeSet);
    if (isSet) {
        newData.size = env->CallIntMethod(jdata, data_getSize);
        newData.__isset.size = true;
        size = newData.size;
    }
    isSet = env->CallBooleanMethod(jdata, data_isBodySet);
    if (isSet) {
        jbyteArray jb = (jbyteArray)env->CallObjectMethod(jdata, data_getBody);
        QByteArray ba;
        jByteArray2QByteArray(ba,jb, size);
        newData.body.clear();
        newData.body.append(ba.data(), ba.size());
        newData.__isset.body = true;
    }
}

void JavaMachine::mapToResourceAttributes(ResourceAttributes &attributes, jobject jattributes) {
    jstring value;
    bool isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isSourceUrlSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jattributes, resourceAttributes_getSourceUrl);
        jString2String(attributes.sourceURL, value);
        attributes.__isset.sourceURL = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isTimestampSet);
    if (isSet) {
        attributes.timestamp = env->CallLongMethod(jattributes, resourceAttributes_getTimestamp);
        attributes.__isset.timestamp = true;
    }

    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isLongitudeSet);
    if (isSet) {
        attributes.longitude = env->CallShortMethod(jattributes, resourceAttributes_getLongitude);
        attributes.__isset.longitude = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isLatitudeSet);
    if (isSet) {
        attributes.latitude = env->CallShortMethod(jattributes, resourceAttributes_getLatitude);
        attributes.__isset.latitude = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isAltitudeSet);
    if (isSet) {
        attributes.altitude= env->CallShortMethod(jattributes, resourceAttributes_getAltitude);
        attributes.__isset.altitude = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isCameraMakeSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jattributes, resourceAttributes_getCameraMake);
        jString2String(attributes.cameraMake, value);
        attributes.__isset.cameraMake = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isCameraModelSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jattributes, resourceAttributes_getCameraModel);
        jString2String(attributes.cameraModel, value);
        attributes.__isset.cameraModel = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isRecoTypeSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jattributes, resourceAttributes_getRecoType);
        jString2String(attributes.recoType, value);
        attributes.__isset.recoType = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isFileNameSet);
    if (isSet) {
        value = (jstring)env->CallObjectMethod(jattributes, resourceAttributes_getFileName);
        jString2String(attributes.fileName, value);
        attributes.__isset.fileName = true;
    }
    isSet = env->CallBooleanMethod(jattributes, resourceAttributes_isAttachmentSet);
    if (isSet) {
        attributes.attachment = env->CallBooleanMethod(jattributes, resourceAttributes_getAttachment);
        attributes.__isset.attachment = true;
    }
}
