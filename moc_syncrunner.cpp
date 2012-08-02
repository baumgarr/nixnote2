/****************************************************************************
** Meta object code from reading C++ file 'syncrunner.h'
**
** Created: Fri Jul 27 13:59:51 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "threads/syncrunner.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'syncrunner.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SyncRunner[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      36,   27,   11,   11, 0x05,
      63,   27,   11,   11, 0x05,
      87,   27,   11,   11, 0x05,
     120,  116,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     137,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SyncRunner[] = {
    "SyncRunner\0\0syncComplete()\0lid,name\0"
    "searchUpdated(int,QString)\0"
    "tagUpdated(int,QString)\0"
    "notebookUpdated(int,QString)\0lid\0"
    "noteUpdated(int)\0synchronize()\0"
};

const QMetaObject SyncRunner::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_SyncRunner,
      qt_meta_data_SyncRunner, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SyncRunner::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SyncRunner::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SyncRunner::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SyncRunner))
        return static_cast<void*>(const_cast< SyncRunner*>(this));
    return QThread::qt_metacast(_clname);
}

int SyncRunner::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: syncComplete(); break;
        case 1: searchUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 2: tagUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: notebookUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: noteUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: synchronize(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void SyncRunner::syncComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void SyncRunner::searchUpdated(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void SyncRunner::tagUpdated(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void SyncRunner::notebookUpdated(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void SyncRunner::noteUpdated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
