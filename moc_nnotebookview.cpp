/****************************************************************************
** Meta object code from reading C++ file 'nnotebookview.h'
**
** Created: Wed Jul 25 18:00:57 2012
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
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      62,   42,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     108,  103,   99,   14, 0x08,
     145,   14,   14,   14, 0x08,
     163,   14,   14,   14, 0x08,
     187,  178,   14,   14, 0x0a,
     216,   14,   14,   14, 0x0a,
     230,   14,   14,   14, 0x0a,
     247,   14,   14,   14, 0x0a,
     262,   14,   14,   14, 0x0a,
     284,   14,   14,   14, 0x0a,
     302,   14,   14,   14, 0x0a,
     320,   14,   14,   14, 0x0a,
     343,   14,   14,   14, 0x0a,
     369,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NNotebookView[] = {
    "NNotebookView\0\0updateSelectionRequested()\0"
    "lid,oldName,newName\0"
    "notebookRenamed(int,QString,QString)\0"
    "int\0item\0calculateHeightRec(QTreeWidgetItem*)\0"
    "calculateHeight()\0editComplete()\0"
    "lid,name\0notebookUpdated(int,QString)\0"
    "rebuildTree()\0buildSelection()\0"
    "addRequested()\0propertiesRequested()\0"
    "deleteRequested()\0renameRequested()\0"
    "moveToStackRequested()\0moveToNewStackRequested()\0"
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
        case 2: { int _r = calculateHeightRec((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: calculateHeight(); break;
        case 4: editComplete(); break;
        case 5: notebookUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 6: rebuildTree(); break;
        case 7: buildSelection(); break;
        case 8: addRequested(); break;
        case 9: propertiesRequested(); break;
        case 10: deleteRequested(); break;
        case 11: renameRequested(); break;
        case 12: moveToStackRequested(); break;
        case 13: moveToNewStackRequested(); break;
        case 14: removeFromStackRequested(); break;
        default: ;
        }
        _id -= 15;
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
QT_END_MOC_NAMESPACE
