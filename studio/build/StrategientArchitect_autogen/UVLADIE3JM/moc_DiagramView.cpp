/****************************************************************************
** Meta object code from reading C++ file 'DiagramView.hpp'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/DiagramView.hpp"
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
    uint offsetsAndSizes[40];
    char stringdata0[12];
    char stringdata1[14];
    char stringdata2[1];
    char stringdata3[15];
    char stringdata4[13];
    char stringdata5[6];
    char stringdata6[8];
    char stringdata7[13];
    char stringdata8[7];
    char stringdata9[18];
    char stringdata10[14];
    char stringdata11[21];
    char stringdata12[7];
    char stringdata13[17];
    char stringdata14[17];
    char stringdata15[8];
    char stringdata16[14];
    char stringdata17[11];
    char stringdata18[13];
    char stringdata19[21];
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
        QT_MOC_LITERAL(69, 12),  // "nodeSelected"
        QT_MOC_LITERAL(82, 6),  // "nodeId"
        QT_MOC_LITERAL(89, 17),  // "nodeDoubleClicked"
        QT_MOC_LITERAL(107, 13),  // "layoutChanged"
        QT_MOC_LITERAL(121, 20),  // "onCurrentPageChanged"
        QT_MOC_LITERAL(142, 6),  // "pageId"
        QT_MOC_LITERAL(149, 16),  // "onDocumentLoaded"
        QT_MOC_LITERAL(166, 16),  // "onRenderComplete"
        QT_MOC_LITERAL(183, 7),  // "svgPath"
        QT_MOC_LITERAL(191, 13),  // "onRenderError"
        QT_MOC_LITERAL(205, 10),  // "errorTitle"
        QT_MOC_LITERAL(216, 12),  // "errorDetails"
        QT_MOC_LITERAL(229, 20)   // "onSceneLayoutChanged"
    },
    "DiagramView",
    "renderStarted",
    "",
    "renderComplete",
    "renderFailed",
    "title",
    "details",
    "nodeSelected",
    "nodeId",
    "nodeDoubleClicked",
    "layoutChanged",
    "onCurrentPageChanged",
    "pageId",
    "onDocumentLoaded",
    "onRenderComplete",
    "svgPath",
    "onRenderError",
    "errorTitle",
    "errorDetails",
    "onSceneLayoutChanged"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_DiagramView[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   80,    2, 0x06,    1 /* Public */,
       3,    0,   81,    2, 0x06,    2 /* Public */,
       4,    2,   82,    2, 0x06,    3 /* Public */,
       7,    1,   87,    2, 0x06,    6 /* Public */,
       9,    1,   90,    2, 0x06,    8 /* Public */,
      10,    0,   93,    2, 0x06,   10 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      11,    1,   94,    2, 0x0a,   11 /* Public */,
      13,    0,   97,    2, 0x0a,   13 /* Public */,
      14,    1,   98,    2, 0x08,   14 /* Private */,
      16,    2,  101,    2, 0x08,   16 /* Private */,
      19,    0,  106,    2, 0x08,   19 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   17,   18,
    QMetaType::Void,

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
        // method 'nodeSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'nodeDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'layoutChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
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
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onSceneLayoutChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
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
        case 3: _t->nodeSelected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->nodeDoubleClicked((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->layoutChanged(); break;
        case 6: _t->onCurrentPageChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onDocumentLoaded(); break;
        case 8: _t->onRenderComplete((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onRenderError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->onSceneLayoutChanged(); break;
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
        {
            using _t = void (DiagramView::*)(const QString & );
            if (_t _q_method = &DiagramView::nodeSelected; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (DiagramView::*)(const QString & );
            if (_t _q_method = &DiagramView::nodeDoubleClicked; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (DiagramView::*)();
            if (_t _q_method = &DiagramView::layoutChanged; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 5;
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
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
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

// SIGNAL 3
void DiagramView::nodeSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void DiagramView::nodeDoubleClicked(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void DiagramView::layoutChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
