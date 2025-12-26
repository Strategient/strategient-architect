/****************************************************************************
** Meta object code from reading C++ file 'DiagramView.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../studio/src/DiagramView.hpp"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DiagramView.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_DiagramView_t {
    uint offsetsAndSizes[30];
    char stringdata0[12];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[13];
    char stringdata5[6];
    char stringdata6[8];
    char stringdata7[21];
    char stringdata8[7];
    char stringdata9[17];
    char stringdata10[17];
    char stringdata11[8];
    char stringdata12[14];
    char stringdata13[11];
    char stringdata14[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_DiagramView_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_DiagramView_t qt_meta_stringdata_DiagramView = {
    {
        QT_MOC_LITERAL(0, 11),  // "DiagramView"
        QT_MOC_LITERAL(12, 13),  // "renderStarted"
        QT_MOC_LITERAL(26, 0),  // ""
        QT_MOC_LITERAL(27, 14),  // "renderComplete"
        QT_MOC_LITERAL(42, 12),  // "renderFailed"
        QT_MOC_LITERAL(55, 5),  // "title"
        QT_MOC_LITERAL(61, 7),  // "details"
        QT_MOC_LITERAL(69, 20),  // "onCurrentPageChanged"
        QT_MOC_LITERAL(90, 6),  // "pageId"
        QT_MOC_LITERAL(97, 16),  // "onDocumentLoaded"
        QT_MOC_LITERAL(114, 16),  // "onRenderComplete"
        QT_MOC_LITERAL(131, 7),  // "svgPath"
        QT_MOC_LITERAL(139, 13),  // "onRenderError"
        QT_MOC_LITERAL(153, 10),  // "errorTitle"
        QT_MOC_LITERAL(164, 12)   // "errorDetails"
    },
    "DiagramView",
    "renderStarted",
    "",
    "renderComplete",
    "renderFailed",
    "title",
    "details",
    "onCurrentPageChanged",
    "pageId",
    "onDocumentLoaded",
    "onRenderComplete",
    "svgPath",
    "onRenderError",
    "errorTitle",
    "errorDetails"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DiagramView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x06,    1 /* Public */,
       3,    0,   57,    2, 0x06,    2 /* Public */,
       4,    2,   58,    2, 0x06,    3 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       7,    1,   63,    2, 0x0a,    6 /* Public */,
       9,    0,   66,    2, 0x0a,    8 /* Public */,
      10,    1,   67,    2, 0x08,    9 /* Private */,
      12,    2,   70,    2, 0x08,   11 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   13,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject DiagramView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_DiagramView.offsetsAndSizes,
    qt_meta_data_DiagramView,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_DiagramView_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<DiagramView, std::true_type>,
        // method 'renderStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'renderComplete'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'renderFailed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onCurrentPageChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onDocumentLoaded'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRenderComplete'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onRenderError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void DiagramView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DiagramView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->renderStarted(); break;
        case 1: _t->renderComplete(); break;
        case 2: _t->renderFailed((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->onCurrentPageChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onDocumentLoaded(); break;
        case 5: _t->onRenderComplete((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onRenderError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DiagramView::*)();
            if (_t _q_method = &DiagramView::renderStarted; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DiagramView::*)();
            if (_t _q_method = &DiagramView::renderComplete; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DiagramView::*)(const QString & , const QString & );
            if (_t _q_method = &DiagramView::renderFailed; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject *DiagramView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DiagramView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DiagramView.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DiagramView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void DiagramView::renderStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DiagramView::renderComplete()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DiagramView::renderFailed(const QString & _t1, const QString & _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
