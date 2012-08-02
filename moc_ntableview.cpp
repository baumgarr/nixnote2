/****************************************************************************
** Meta object code from reading C++ file 'ntableview.h'
**
** Created: Thu Aug 2 14:10:19 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/ntableview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ntableview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NTableView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NTableView[] = {
    "NTableView\0\0newWindow\0openNote(bool)\0"
    "refreshData()\0"
};

const QMetaObject NTableView::staticMetaObject = {
    { &QTableView::staticMetaObject, qt_meta_stringdata_NTableView,
      qt_meta_data_NTableView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NTableView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NTableView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NTableView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NTableView))
        return static_cast<void*>(const_cast< NTableView*>(this));
    return QTableView::qt_metacast(_clname);
}

int NTableView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: openNote((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: refreshData(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void NTableView::openNote(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
