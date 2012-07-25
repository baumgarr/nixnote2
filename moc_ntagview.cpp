/****************************************************************************
** Meta object code from reading C++ file 'ntagview.h'
**
** Created: Wed Jul 25 12:56:55 2012
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
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      57,   37,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      98,   93,   89,    9, 0x08,
     135,    9,    9,    9, 0x08,
     153,    9,    9,    9, 0x08,
     177,  168,    9,    9, 0x0a,
     201,    9,    9,    9, 0x0a,
     215,    9,    9,    9, 0x0a,
     236,  232,    9,    9, 0x0a,
     251,    9,    9,    9, 0x0a,
     266,    9,    9,    9, 0x0a,
     288,    9,    9,    9, 0x0a,
     306,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NTagView[] = {
    "NTagView\0\0updateSelectionRequested()\0"
    "lid,oldName,newName\0tagRenamed(int,QString,QString)\0"
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
        case 1: tagRenamed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: { int _r = calculateHeightRec((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: calculateHeight(); break;
        case 4: editComplete(); break;
        case 5: tagUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: rebuildTree(); break;
        case 7: buildSelection(); break;
        case 8: addNewTag((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: addRequested(); break;
        case 10: propertiesRequested(); break;
        case 11: deleteRequested(); break;
        case 12: renameRequested(); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void NTagView::updateSelectionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void NTagView::tagRenamed(int _t1, QString _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
