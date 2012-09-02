#ifndef ENCRYPT_GLOBAL_H
#define ENCRYPT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ENCRYPT_LIBRARY)
#  define ENCRYPTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ENCRYPTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ENCRYPT_GLOBAL_H
