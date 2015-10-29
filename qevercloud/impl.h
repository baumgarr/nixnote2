#ifndef QEVERCLOUD_IMPL_H
#define QEVERCLOUD_IMPL_H

#include "exceptions.h"
#include "globals.h"
#include "http.h"
#include "thrift.h"
#include "Optional.h"


/**

@mainpage About QEverCloud

This library presents complete Evernote SDK for Qt.
All the functionality that is described on <a href="http://dev.evernote.com/doc/">Evernote site</a>
is implemented and ready to use. In particular OAuth autentication is implemented.

Include *QEverCloud.h* or *QEverCloudOAuth.h* to use the library. The latter header is needed if you use OAuth functionality.

<a href="https://github.com/mgsxx/QEverCloud">QEverCloud on GitHub</a>

*/

namespace qevercloud {

/** @cond HIDDEN_SYMBOLS  */

ThriftException readThriftException(ThriftBinaryBufferReader& r);

void throwEDAMSystemException(const EDAMSystemException &e);

/** @endcond */
}

#endif // QEVERCLOUD_IMPL_H
