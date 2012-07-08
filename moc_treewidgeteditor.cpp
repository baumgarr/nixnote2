/****************************************************************************
** Meta object code from reading C++ file 'treewidgeteditor.h'
**
** Created: Wed Jun 20 09:55:22 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/treewidgeteditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'treewidgeteditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TreeWidgetEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TreeWidgetEditor[] = {
    "TreeWidgetEditor\0\0editComplete()\0"
};

const QMetaObject TreeWidgetEditor::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_TreeWidgetEditor,
      qt_meta_data_TreeWidgetEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TreeWidgetEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TreeWidgetEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TreeWidgetEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TreeWidgetEditor))
        return static_cast<void*>(const_cast< TreeWidgetEditor*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int TreeWidgetEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: editComplete(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void TreeWidgetEditor::editComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
