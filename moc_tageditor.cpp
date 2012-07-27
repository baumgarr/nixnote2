/****************************************************************************
** Meta object code from reading C++ file 'tageditor.h'
**
** Created: Fri Jul 27 11:20:20 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/browserWidgets/tageditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tageditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TagEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,
      34,   10,   10,   10, 0x05,
      52,   48,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      76,   71,   10,   10, 0x0a,
     101,   95,   10,   10, 0x0a,
     123,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TagEditor[] = {
    "TagEditor\0\0buttonClicked(QString)\0"
    "tagsUpdated()\0lid\0newTagCreated(int)\0"
    "text\0removeTag(QString)\0focus\0"
    "newTagFocusLost(bool)\0newTagTabPressed()\0"
};

const QMetaObject TagEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TagEditor,
      qt_meta_data_TagEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TagEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TagEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TagEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TagEditor))
        return static_cast<void*>(const_cast< TagEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int TagEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: buttonClicked((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: tagsUpdated(); break;
        case 2: newTagCreated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: removeTag((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: newTagFocusLost((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: newTagTabPressed(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void TagEditor::buttonClicked(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TagEditor::tagsUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void TagEditor::newTagCreated(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
