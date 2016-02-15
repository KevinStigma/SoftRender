/****************************************************************************
** Meta object code from reading C++ file 'softrender.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../softrender.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'softrender.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SoftRender_t {
    QByteArrayData data[17];
    char stringdata[200];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_SoftRender_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_SoftRender_t qt_meta_stringdata_SoftRender = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 15),
QT_MOC_LITERAL(2, 27, 0),
QT_MOC_LITERAL(3, 28, 11),
QT_MOC_LITERAL(4, 40, 10),
QT_MOC_LITERAL(5, 51, 11),
QT_MOC_LITERAL(6, 63, 16),
QT_MOC_LITERAL(7, 80, 19),
QT_MOC_LITERAL(8, 100, 14),
QT_MOC_LITERAL(9, 115, 11),
QT_MOC_LITERAL(10, 127, 12),
QT_MOC_LITERAL(11, 140, 8),
QT_MOC_LITERAL(12, 149, 10),
QT_MOC_LITERAL(13, 160, 10),
QT_MOC_LITERAL(14, 171, 7),
QT_MOC_LITERAL(15, 179, 8),
QT_MOC_LITERAL(16, 188, 10)
    },
    "SoftRender\0setCameraAttrib\0\0setBackface\0"
    "plusButton\0minusButton\0FlatshadingCheck\0"
    "GouraudshadingCheck\0WireFrameCheck\0"
    "VertexCheck\0TextureCheck\0loadCube\0"
    "loadViolin\0loadSphere\0loadRec\0loadTank\0"
    "loadTeapot\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SoftRender[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x0a,
       3,    0,   90,    2, 0x0a,
       4,    0,   91,    2, 0x0a,
       5,    0,   92,    2, 0x0a,
       6,    0,   93,    2, 0x0a,
       7,    0,   94,    2, 0x0a,
       8,    0,   95,    2, 0x0a,
       9,    0,   96,    2, 0x0a,
      10,    0,   97,    2, 0x0a,
      11,    0,   98,    2, 0x0a,
      12,    0,   99,    2, 0x0a,
      13,    0,  100,    2, 0x0a,
      14,    0,  101,    2, 0x0a,
      15,    0,  102,    2, 0x0a,
      16,    0,  103,    2, 0x0a,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SoftRender::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SoftRender *_t = static_cast<SoftRender *>(_o);
        switch (_id) {
        case 0: _t->setCameraAttrib(); break;
        case 1: _t->setBackface(); break;
        case 2: _t->plusButton(); break;
        case 3: _t->minusButton(); break;
        case 4: _t->FlatshadingCheck(); break;
        case 5: _t->GouraudshadingCheck(); break;
        case 6: _t->WireFrameCheck(); break;
        case 7: _t->VertexCheck(); break;
        case 8: _t->TextureCheck(); break;
        case 9: _t->loadCube(); break;
        case 10: _t->loadViolin(); break;
        case 11: _t->loadSphere(); break;
        case 12: _t->loadRec(); break;
        case 13: _t->loadTank(); break;
        case 14: _t->loadTeapot(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SoftRender::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_SoftRender.data,
      qt_meta_data_SoftRender,  qt_static_metacall, 0, 0}
};


const QMetaObject *SoftRender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SoftRender::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SoftRender.stringdata))
        return static_cast<void*>(const_cast< SoftRender*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int SoftRender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
