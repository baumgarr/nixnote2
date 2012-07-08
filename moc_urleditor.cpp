/****************************************************************************
** Meta object code from reading C++ file 'urleditor.h'
**
** Created: Tue Jun 12 11:33:42 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "gui/browserWidgets/urleditor.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'urleditor.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_UrlEditor[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   11,   10,   10, 0x05,
      35,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      49,   10,   10,   10, 0x0a,
      71,   66,   10,   10, 0x0a,
      99,   93,   10,   10, 0x0a,
     126,  117,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_UrlEditor[] = {
    "UrlEditor\0\0hasFocus\0focussed(bool)\0"
    "textUpdated()\0setActiveColor()\0text\0"
    "textModified(QString)\0focus\0"
    "gainedFocus(bool)\0lid,text\0"
    "setUrl(int,QString)\0"
};

const QMetaObject UrlEditor::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_UrlEditor,
      qt_meta_data_UrlEditor, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &UrlEditor::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *UrlEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *UrlEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_UrlEditor))
        return static_cast<void*>(const_cast< UrlEditor*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int UrlEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: focussed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: textUpdated(); break;
        case 2: setActiveColor(); break;
        case 3: textModified((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: gainedFocus((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: setUrl((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void UrlEditor::focussed(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UrlEditor::textUpdated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
