/*
 * This header provides the "backports" of several Qt5 macros into Qt4
 * so that one can use Qt5 candies with Qt4 as well
 */

#ifndef QEVERCLOUD_QT4_HELPERS_H
#define QEVERCLOUD_QT4_HELPERS_H

#if QT_VERSION < 0x050000

#if __cplusplus >= 201103L
#define Q_DECL_OVERRIDE override
#define Q_DECL_FINAL final
#define Q_STATIC_ASSERT_X(x1,x2) static_assert(x1, x2)
#define Q_DECL_EQ_DELETE = delete
#else
#define Q_DECL_OVERRIDE
#define Q_DECL_FINAL
#define Q_STATIC_ASSERT_X(x1,x2)
#define Q_DECL_EQ_DELETE
#endif

#define QStringLiteral(x) QString(QLatin1String(x))

#define QEC_SIGNAL(className, methodName, ...) SIGNAL(methodName(__VA_ARGS__))
#define QEC_SLOT(className, methodName, ...) SLOT(methodName(__VA_ARGS__))

#else // QT_VERSION

#define QEC_SIGNAL(className, methodName, ...) &className::methodName
#define QEC_SLOT(className, methodName, ...) &className::methodName

#endif // QT_VERSION

#endif // QEVERCLOUD_QT4_HELPERS_H
