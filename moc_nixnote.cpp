/****************************************************************************
** Meta object code from reading C++ file 'nixnote.h'
**
** Created: Wed Jun 20 14:36:29 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "nixnote.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nixnote.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NixNote[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      25,    8,    8,    8, 0x0a,
      40,    8,    8,    8, 0x0a,
      54,    8,    8,    8, 0x0a,
      67,    8,    8,    8, 0x0a,
      86,    8,    8,    8, 0x0a,
     104,    8,    8,    8, 0x0a,
     130,    8,    8,    8, 0x0a,
     152,    8,    8,    8, 0x0a,
     185,  175,    8,    8, 0x0a,
     200,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NixNote[] = {
    "NixNote\0\0syncRequested()\0closeNixNote()\0"
    "synchronize()\0disconnect()\0"
    "updateSyncButton()\0syncButtonReset()\0"
    "updateSelectionCriteria()\0"
    "leftButtonTriggered()\0rightButtonTriggered()\0"
    "newWindow\0openNote(bool)\0databaseRestore()\0"
};

const QMetaObject NixNote::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_NixNote,
      qt_meta_data_NixNote, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NixNote::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NixNote::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NixNote::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NixNote))
        return static_cast<void*>(const_cast< NixNote*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int NixNote::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: syncRequested(); break;
        case 1: closeNixNote(); break;
        case 2: synchronize(); break;
        case 3: disconnect(); break;
        case 4: updateSyncButton(); break;
        case 5: syncButtonReset(); break;
        case 6: updateSelectionCriteria(); break;
        case 7: leftButtonTriggered(); break;
        case 8: rightButtonTriggered(); break;
        case 9: openNote((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: databaseRestore(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void NixNote::syncRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
