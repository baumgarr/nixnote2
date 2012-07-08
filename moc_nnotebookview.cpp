/****************************************************************************
** Meta object code from reading C++ file 'nnotebookview.h'
**
** Created: Fri Jun 8 14:53:19 2012
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
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   46,   42,   14, 0x08,
      88,   14,   14,   14, 0x08,
     115,  106,   14,   14, 0x0a,
     144,   14,   14,   14, 0x0a,
     158,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NNotebookView[] = {
    "NNotebookView\0\0updateSelectionRequested()\0"
    "int\0item\0calculateHeightRec(QTreeWidgetItem*)\0"
    "calculateHeight()\0lid,name\0"
    "notebookUpdated(int,QString)\0rebuildTree()\0"
    "buildSelection()\0"
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
        case 1: { int _r = calculateHeightRec((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 2: calculateHeight(); break;
        case 3: notebookUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 4: rebuildTree(); break;
        case 5: buildSelection(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void NNotebookView::updateSelectionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
