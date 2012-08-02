/****************************************************************************
** Meta object code from reading C++ file 'nsearchview.h'
**
** Created: Fri Jul 27 13:59:57 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/nsearchview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nsearchview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NSearchView[] = {

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
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   44,   40,   12, 0x08,
      86,   12,   12,   12, 0x08,
     104,   12,   12,   12, 0x08,
     128,  119,   12,   12, 0x0a,
     155,   12,   12,   12, 0x0a,
     172,   12,   12,   12, 0x0a,
     190,   12,   12,   12, 0x0a,
     205,   12,   12,   12, 0x0a,
     227,   12,   12,   12, 0x0a,
     245,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NSearchView[] = {
    "NSearchView\0\0updateSelectionRequested()\0"
    "int\0item\0calculateHeightRec(QTreeWidgetItem*)\0"
    "calculateHeight()\0editComplete()\0"
    "lid,name\0searchUpdated(int,QString)\0"
    "buildSelection()\0updateSelection()\0"
    "addRequested()\0propertiesRequested()\0"
    "deleteRequested()\0renameRequested()\0"
};

const QMetaObject NSearchView::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_NSearchView,
      qt_meta_data_NSearchView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NSearchView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NSearchView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NSearchView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NSearchView))
        return static_cast<void*>(const_cast< NSearchView*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int NSearchView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 4: searchUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: buildSelection(); break;
        case 6: updateSelection(); break;
        case 7: addRequested(); break;
        case 8: propertiesRequested(); break;
        case 9: deleteRequested(); break;
        case 10: renameRequested(); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void NSearchView::updateSelectionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
