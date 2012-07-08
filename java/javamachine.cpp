#include "javamachine.h"
#include "global.h"
#include <QString>

extern Global global;


JavaMachine::JavaMachine()
{
}



void JavaMachine::create_jvm() {
    args.version = JNI_VERSION_1_6;
    args.nOptions = 1;
//    options[0].optionString = "-Djava.class.path=./java/nixnote.jar:./java/evernote-api-1.20.jar:./java/libthrift.jar:./java/log4j-1.2.14.jar";
    QString cp = global.fileManager.getProgramDirPath("java/");
    QString clp = "-Djava.class.path="+cp+"nixnote.jar:"+cp+
            "evernote-api-1.20.jar:"+cp+"libthrift.jar:"+cp+
            "log4j-1.2.14.jar";
    char *cp2;
    cp2 = new char[clp.length()+1];
    strcpy(cp2,clp.toAscii());
    options[0].optionString = cp2;
    args.options = options;
    args.ignoreUnrecognized = JNI_FALSE;

    JNI_CreateJavaVM(&jvm, (void **)&env, &args);

    // Start loading classes
    messageBlockClass = env->FindClass("org/nixnote/bridge/MessageBlock");
    messageBlock_isError = env->GetMethodID(messageBlockClass, "isError", "()Z");
    messageBlock_getErrorMessage = env->GetMethodID(messageBlockClass, "getErrorMessage", "()Ljava/lang/String;");
    communicationClass = env->FindClass("org/nixnote/bridge/Communication");
    //env->ExceptionDescribe();
    communication_constructor = env->GetMethodID(communicationClass, "<init>", "()V");
    communication_connect = env->GetMethodID(communicationClass, "connect", "(Ljava/lang/String;Ljava/lang/String;)Z");
    communication = env->NewObject(communicationClass, communication_constructor);
    communication_getMessageBlock = env->GetMethodID(communicationClass, "getMessageBlock", "()Lorg/nixnote/bridge/MessageBlock;");
    communication_getSyncState = env->GetMethodID(communicationClass, "getSyncState", "()Lcom/evernote/edam/notestore/SyncState;");

    syncStateClass = env->FindClass("com/evernote/edam/notestore/SyncState");
    syncState_getCurrentTime = env->GetMethodID(syncStateClass, "getCurrentTime", "()J");
    syncState_getFullSyncBefore = env->GetMethodID(syncStateClass, "getFullSyncBefore", "()J");
    syncState_getUpdateCount = env->GetMethodID(syncStateClass, "getUpdateCount", "()I");
    syncState_getUploaded = env->GetMethodID(syncStateClass, "getUploaded", "()J");
    syncState_isSetCurrentTime = env->GetMethodID(syncStateClass, "isSetCurrentTime", "()Z");
    syncState_isSetUpdateCount = env->GetMethodID(syncStateClass, "isSetUpdateCount", "()Z");
    syncState_isSetUploaded = env->GetMethodID(syncStateClass, "isSetUploaded", "()Z");
    syncState_isSetFullSyncBefore = env->GetMethodID(syncStateClass, "isSetFullSyncBefore", "()Z");

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
    data_isGetBodySet = env->GetMethodID(dataClass, "isSetBody", "()Z");


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


    lazyMapClass = env->FindClass("com/evernote/edam/type/LazyMap");
    lazyMap_getKeysOnly;
    lazyMap_getFullMap;

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
    noteClass_getResources  = env->GetMethodID(noteClass, "getResources", "()Ljava/util/List;");
    noteClass_isResourcesSet = env->GetMethodID(noteClass, "isSetResources", "()Z");
    noteClass_getAttributes = env->GetMethodID(noteClass, "getAttributes", "()Lcom/evernote/edam/type/NoteAttributes;");
    noteClass_isAttributesSet  = env->GetMethodID(noteClass, "isSetAttributes", "()Z");
    noteClass_getTagNames = env->GetMethodID(noteClass, "getTagNames", "()Ljava/util/List;");
    noteClass_isTagNamesSet = env->GetMethodID(noteClass, "isSetTagNames", "()Z");

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

    delete cp2;
    return;
}



bool JavaMachine::connect() {
        jstring userid = env->NewStringUTF(global.username.c_str());
        jstring password = env->NewStringUTF(global.password.c_str());

        // Call the actual java conenct method.  We get back a message block
        jboolean connected = env->CallBooleanMethod(communication, communication_connect, userid, password);

        if (!connected) {
            jobject messageBlock = env->CallObjectMethod(communication, communication_getMessageBlock);

            // Convert the message block into something we can use
            MessageBlock mb;
            convertMessageBlock(mb, messageBlock);
            QLOG_DEBUG() << "Error Connecting: " << mb.errorMessage;
        }
        jobject syncStateObj = env->CallObjectMethod(communication, communication_getMessageBlock);
        int i=1;
        i++;

}

void JavaMachine::jString2QString(QString &qstr, jstring &jstring, bool release) {
    jboolean blnIsCopy;
    char* strCOut;
    const char* strCIn = env->GetStringUTFChars(jstring, &blnIsCopy);
    qstr = QString(strCIn);
    if (release)
        env->ReleaseStringUTFChars(jstring , strCIn); // release jstring
}


bool JavaMachine::getSyncState(SyncState &syncState) {
    // If we are not connected, return
    if (!global.connected)
        return false;


    // Call the actual java conenct method.  We get back a message block
    jobject syncStateObj = env->CallObjectMethod(communication, communication_getMessageBlock);

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

    QLOG_DEBUG() << "Done";
}



void JavaMachine::convertMessageBlock(MessageBlock &mb, jobject messageBlock) {

    jboolean error = env->CallBooleanMethod(messageBlock, messageBlock_isError);
    jstring message = (jstring)env->CallObjectMethod(messageBlock, messageBlock_getErrorMessage);

    mb.error = error;
    jString2QString(mb.errorMessage, message);
}
