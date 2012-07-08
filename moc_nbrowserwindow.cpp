/****************************************************************************
** Meta object code from reading C++ file 'nbrowserwindow.h'
**
** Created: Mon Jun 18 13:01:31 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/nbrowserwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nbrowserwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NBrowserWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      37,   15,   33,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   51,   15,   15, 0x0a,
      80,   15,   15,   15, 0x08,
      99,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NBrowserWindow[] = {
    "NBrowserWindow\0\0noteUpdated(int)\0int\0"
    "tagAdded(int)\0value\0changeExpandState(int)\0"
    "sendUpdateSignal()\0newTagAdded(int)\0"
};

const QMetaObject NBrowserWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NBrowserWindow,
      qt_meta_data_NBrowserWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NBrowserWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NBrowserWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NBrowserWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NBrowserWindow))
        return static_cast<void*>(const_cast< NBrowserWindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int NBrowserWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: noteUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: { int _r = tagAdded((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: changeExpandState((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: sendUpdateSignal(); break;
        case 4: newTagAdded((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void NBrowserWindow::noteUpdated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
int NBrowserWindow::tagAdded(int _t1)
{
    int _t0;
    void *_a[] = { const_cast<void*>(reinterpret_cast<const void*>(&_t0)), const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
    return _t0;
}
QT_END_MOC_NAMESPACE
