// This file is generated from Evernote Thrift API and is a part of the QEverCloud project

#ifndef QEVERCLOUD_GENERATED_EDAMERRORCODE_H
#define QEVERCLOUD_GENERATED_EDAMERRORCODE_H

namespace qevercloud {

/**
 * Numeric codes indicating the type of error that occurred on the
 * service.
 * <dl>
 *   <dt>UNKNOWN</dt>
 *     <dd>No information available about the error</dd>
 *   <dt>BAD_DATA_FORMAT</dt>
 *     <dd>The format of the request data was incorrect</dd>
 *   <dt>PERMISSION_DENIED</dt>
 *     <dd>Not permitted to perform action</dd>
 *   <dt>INTERNAL_ERROR</dt>
 *     <dd>Unexpected problem with the service</dd>
 *   <dt>DATA_REQUIRED</dt>
 *     <dd>A required parameter/field was absent</dd>
 *   <dt>LIMIT_REACHED</dt>
 *     <dd>Operation denied due to data model limit</dd>
 *   <dt>QUOTA_REACHED</dt>
 *     <dd>Operation denied due to user storage limit</dd>
 *   <dt>INVALID_AUTH</dt>
 *     <dd>Username and/or password incorrect</dd>
 *   <dt>AUTH_EXPIRED</dt>
 *     <dd>Authentication token expired</dd>
 *   <dt>DATA_CONFLICT</dt>
 *     <dd>Change denied due to data model conflict</dd>
 *   <dt>ENML_VALIDATION</dt>
 *     <dd>Content of submitted note was malformed</dd>
 *   <dt>SHARD_UNAVAILABLE</dt>
 *     <dd>Service shard with account data is temporarily down</dd>
 *   <dt>LEN_TOO_SHORT</dt>
 *     <dd>Operation denied due to data model limit, where something such
 *         as a string length was too short</dd>
 *   <dt>LEN_TOO_LONG</dt>
 *     <dd>Operation denied due to data model limit, where something such
 *         as a string length was too long</dd>
 *   <dt>TOO_FEW</dt>
 *     <dd>Operation denied due to data model limit, where there were
 *         too few of something.</dd>
 *   <dt>TOO_MANY</dt>
 *     <dd>Operation denied due to data model limit, where there were
 *         too many of something.</dd>
 *   <dt>UNSUPPORTED_OPERATION</dt>
 *     <dd>Operation denied because it is currently unsupported.</dd>
 *   <dt>TAKEN_DOWN</dt>
 *     <dd>Operation denied because access to the corresponding object is
 *         prohibited in response to a take-down notice.</dd>
 *   <dt>RATE_LIMIT_REACHED</dt>
 *     <dd>Operation denied because the calling application has reached
 *         its hourly API call limit for this user.</dd>
 * </dl>
 */
struct EDAMErrorCode {
    enum type {
        UNKNOWN = 1,
        BAD_DATA_FORMAT = 2,
        PERMISSION_DENIED = 3,
        INTERNAL_ERROR = 4,
        DATA_REQUIRED = 5,
        LIMIT_REACHED = 6,
        QUOTA_REACHED = 7,
        INVALID_AUTH = 8,
        AUTH_EXPIRED = 9,
        DATA_CONFLICT = 10,
        ENML_VALIDATION = 11,
        SHARD_UNAVAILABLE = 12,
        LEN_TOO_SHORT = 13,
        LEN_TOO_LONG = 14,
        TOO_FEW = 15,
        TOO_MANY = 16,
        UNSUPPORTED_OPERATION = 17,
        TAKEN_DOWN = 18,
        RATE_LIMIT_REACHED = 19
    };
};



}
#endif // QEVERCLOUD_GENERATED_EDAMERRORCODE_H
