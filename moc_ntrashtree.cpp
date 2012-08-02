/****************************************************************************
** Meta object code from reading C++ file 'ntrashtree.h'
**
** Created: Fri Jul 27 14:00:28 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/ntrashtree.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ntrashtree.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NTrashTree[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      48,   43,   39,   11, 0x08,
      85,   11,   11,   11, 0x08,
     103,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NTrashTree[] = {
    "NTrashTree\0\0updateSelectionRequested()\0"
    "int\0item\0calculateHeightRec(QTreeWidgetItem*)\0"
    "calculateHeight()\0buildSelection()\0"
};

const QMetaObject NTrashTree::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_NTrashTree,
      qt_meta_data_NTrashTree, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NTrashTree::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NTrashTree::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NTrashTree::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NTrashTree))
        return static_cast<void*>(const_cast< NTrashTree*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int NTrashTree::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 3: buildSelection(); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NTrashTree::updateSelectionRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
