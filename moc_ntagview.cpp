/****************************************************************************
** Meta object code from reading C++ file 'ntagview.h'
**
** Created: Fri Jul 27 14:00:05 2012
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
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      57,   37,    9,    9, 0x05,
      98,   89,    9,    9, 0x05,
     126,  122,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     149,  144,  140,    9, 0x08,
     186,    9,    9,    9, 0x08,
     204,    9,    9,    9, 0x08,
     219,   89,    9,    9, 0x0a,
     243,    9,    9,    9, 0x0a,
     257,    9,    9,    9, 0x0a,
     274,  122,    9,    9, 0x0a,
     289,    9,    9,    9, 0x0a,
     304,    9,    9,    9, 0x0a,
     326,    9,    9,    9, 0x0a,
     344,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NTagView[] = {
    "NTagView\0\0updateSelectionRequested()\0"
    "lid,oldName,newName\0tagRenamed(int,QString,QString)\0"
    "lid,name\0tagDeleted(int,QString)\0lid\0"
    "tagAdded(int)\0int\0item\0"
    "calculateHeightRec(QTreeWidgetItem*)\0"
    "calculateHeight()\0editComplete()\0"
    "tagUpdated(int,QString)\0rebuildTree()\0"
    "buildSelection()\0addNewTag(int)\0"
    "addRequested()\0propertiesRequested()\0"
    "deleteRequested()\0renameRequested()\0"
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
        case 2: tagDeleted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: tagAdded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: { int _r = calculateHeightRec((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 5: calculateHeight(); break;
        case 6: editComplete(); break;
        case 7: tagUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 8: rebuildTree(); break;
        case 9: buildSelection(); break;
        case 10: addNewTag((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: addRequested(); break;
        case 12: propertiesRequested(); break;
        case 13: deleteRequested(); break;
        case 14: renameRequested(); break;
        default: ;
        }
        _id -= 15;
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

// SIGNAL 2
void NTagView::tagDeleted(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NTagView::tagAdded(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
