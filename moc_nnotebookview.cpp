/****************************************************************************
** Meta object code from reading C++ file 'nnotebookview.h'
**
** Created: Fri Jul 27 14:00:11 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/nnotebookview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'nnotebookview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NNotebookView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      62,   42,   14,   14, 0x05,
     108,   99,   14,   14, 0x05,
     141,  137,   14,   14, 0x05,
     165,  160,   14,   14, 0x05,
     185,  160,   14,   14, 0x05,
     223,  207,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     262,  257,  253,   14, 0x08,
     299,   14,   14,   14, 0x08,
     317,   14,   14,   14, 0x08,
     332,   99,   14,   14, 0x0a,
     361,   14,   14,   14, 0x0a,
     375,   14,   14,   14, 0x0a,
     392,   14,   14,   14, 0x0a,
     407,   14,   14,   14, 0x0a,
     429,   14,   14,   14, 0x0a,
     447,   14,   14,   14, 0x0a,
     465,   14,   14,   14, 0x0a,
     488,   14,   14,   14, 0x0a,
     514,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NNotebookView[] = {
    "NNotebookView\0\0updateSelectionRequested()\0"
    "lid,oldName,newName\0"
    "notebookRenamed(int,QString,QString)\0"
    "lid,name\0notebookDeleted(int,QString)\0"
    "lid\0notebookAdded(int)\0name\0"
    "stackAdded(QString)\0stackDeleted(QString)\0"
    "oldName,newName\0stackRenamed(QString,QString)\0"
    "int\0item\0calculateHeightRec(QTreeWidgetItem*)\0"
    "calculateHeight()\0editComplete()\0"
    "notebookUpdated(int,QString)\0rebuildTree()\0"
    "buildSelection()\0addRequested()\0"
    "propertiesRequested()\0deleteRequested()\0"
    "renameRequested()\0moveToStackRequested()\0"
    "moveToNewStackRequested()\0"
    "removeFromStackRequested()\0"
};

const QMetaObject NNotebookView::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_NNotebookView,
      qt_meta_data_NNotebookView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NNotebookView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NNotebookView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NNotebookView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NNotebookView))
        return static_cast<void*>(const_cast< NNotebookView*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int NNotebookView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateSelectionRequested(); break;
        case 1: notebookRenamed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: notebookDeleted((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: notebookAdded((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: stackAdded((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: stackDeleted((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: stackRenamed((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 7: { int _r = calculateHeightRec((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 8: calculateHeight(); break;
        case 9: editComplete(); break;
        case 10: notebookUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: rebuildTree(); break;
        case 12: buildSelection(); break;
        case 13: addRequested(); break;
        case 14: propertiesRequested(); break;
        case 15: deleteRequested(); break;
        case 16: renameRequested(); break;
        case 17: moveToStackRequested(); break;
        case 18: moveToNewStackRequested(); break;
        case 19: removeFromStackRequested(); break;
        default: ;
        }
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void NNotebookView::updateSelectionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void NNotebookView::notebookRenamed(int _t1, QString _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NNotebookView::notebookDeleted(int _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void NNotebookView::notebookAdded(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void NNotebookView::stackAdded(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void NNotebookView::stackDeleted(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void NNotebookView::stackRenamed(QString _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE
