/****************************************************************************
** Meta object code from reading C++ file 'ntagview.h'
**
** Created: Thu Jul 12 10:21:28 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/ntagview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ntagview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NTagView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   41,   37,    9, 0x08,
      83,    9,    9,    9, 0x08,
     101,    9,    9,    9, 0x08,
     125,  116,    9,    9, 0x0a,
     149,    9,    9,    9, 0x0a,
     163,    9,    9,    9, 0x0a,
     184,  180,    9,    9, 0x0a,
     199,    9,    9,    9, 0x0a,
     214,    9,    9,    9, 0x0a,
     236,    9,    9,    9, 0x0a,
     254,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NTagView[] = {
    "NTagView\0\0updateSelectionRequested()\0"
    "int\0item\0calculateHeightRec(QTreeWidgetItem*)\0"
    "calculateHeight()\0editComplete()\0"
    "lid,name\0tagUpdated(int,QString)\0"
    "rebuildTree()\0buildSelection()\0lid\0"
    "addNewTag(int)\0addRequested()\0"
    "propertiesRequested()\0deleteRequested()\0"
    "renameRequested()\0"
};

const QMetaObject NTagView::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_NTagView,
      qt_meta_data_NTagView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NTagView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NTagView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NTagView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NTagView))
        return static_cast<void*>(const_cast< NTagView*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int NTagView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateSelectionRequested(); break;
        case 1: { int _r = calculateHeightRec((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: calculateHeight(); break;
        case 3: editComplete(); break;
        case 4: tagUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: rebuildTree(); break;
        case 6: buildSelection(); break;
        case 7: addNewTag((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: addRequested(); break;
        case 9: propertiesRequested(); break;
        case 10: deleteRequested(); break;
        case 11: renameRequested(); break;
        default: ;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void NTagView::updateSelectionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
