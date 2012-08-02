/****************************************************************************
** Meta object code from reading C++ file 'notebookmenubutton.h'
**
** Created: Fri Jul 27 14:00:43 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/browserWidgets/notebookmenubutton.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'notebookmenubutton.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NotebookMenuButton[] = {

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
      20,   19,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NotebookMenuButton[] = {
    "NotebookMenuButton\0\0notebookChanged()\0"
    "notebookSelected()\0"
};

const QMetaObject NotebookMenuButton::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_NotebookMenuButton,
      qt_meta_data_NotebookMenuButton, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NotebookMenuButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NotebookMenuButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NotebookMenuButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NotebookMenuButton))
        return static_cast<void*>(const_cast< NotebookMenuButton*>(this));
    return QPushButton::qt_metacast(_clname);
}

int NotebookMenuButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: notebookChanged(); break;
        case 1: notebookSelected(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void NotebookMenuButton::notebookChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
