/****************************************************************************
** Meta object code from reading C++ file 'abstracttask.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../abstracttask.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'abstracttask.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12AbstractTaskE_t {};
} // unnamed namespace

template <> constexpr inline auto AbstractTask::qt_create_metaobjectdata<qt_meta_tag_ZN12AbstractTaskE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AbstractTask",
        "error",
        "",
        "Fountain::Body",
        "scene",
        "summary",
        "text",
        "taskFinished",
        "AbstractTask*",
        "task",
        "busyChanged",
        "promptChanged",
        "finished",
        "onOllamaError",
        "promptId",
        "onOllamaResponse",
        "busy",
        "prompt",
        "errorMessage",
        "sceneSummary"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'error'
        QtMocHelpers::SignalData<void(const QString &, const Fountain::Body &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 1 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'summary'
        QtMocHelpers::SignalData<void(const QString &, const Fountain::Body &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { 0x80000000 | 3, 4 },
        }}),
        // Signal 'taskFinished'
        QtMocHelpers::SignalData<void(AbstractTask *)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 8, 9 },
        }}),
        // Signal 'busyChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'promptChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'finished'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onOllamaError'
        QtMocHelpers::SlotData<void(int, const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::QString, 6 },
        }}),
        // Slot 'onOllamaResponse'
        QtMocHelpers::SlotData<void(int, const QString &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::QString, 6 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'busy'
        QtMocHelpers::PropertyData<bool>(16, QMetaType::Bool, QMC::DefaultPropertyFlags, 3),
        // property 'prompt'
        QtMocHelpers::PropertyData<QString>(17, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'errorMessage'
        QtMocHelpers::PropertyData<QString>(18, QMetaType::QString, QMC::DefaultPropertyFlags, 5),
        // property 'sceneSummary'
        QtMocHelpers::PropertyData<QString>(19, QMetaType::QString, QMC::DefaultPropertyFlags, 5),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AbstractTask, qt_meta_tag_ZN12AbstractTaskE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AbstractTask::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AbstractTaskE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AbstractTaskE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12AbstractTaskE_t>.metaTypes,
    nullptr
} };

void AbstractTask::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AbstractTask *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->error((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Fountain::Body>>(_a[2]))); break;
        case 1: _t->summary((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Fountain::Body>>(_a[2]))); break;
        case 2: _t->taskFinished((*reinterpret_cast<std::add_pointer_t<AbstractTask*>>(_a[1]))); break;
        case 3: _t->busyChanged(); break;
        case 4: _t->promptChanged(); break;
        case 5: _t->finished(); break;
        case 6: _t->onOllamaError((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->onOllamaResponse((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< AbstractTask* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AbstractTask::*)(const QString & , const Fountain::Body & )>(_a, &AbstractTask::error, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AbstractTask::*)(const QString & , const Fountain::Body & )>(_a, &AbstractTask::summary, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AbstractTask::*)(AbstractTask * )>(_a, &AbstractTask::taskFinished, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AbstractTask::*)()>(_a, &AbstractTask::busyChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AbstractTask::*)()>(_a, &AbstractTask::promptChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AbstractTask::*)()>(_a, &AbstractTask::finished, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isBusy(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->prompt(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->errorMessage(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->sceneSummary(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 1: _t->setPrompt(*reinterpret_cast<QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *AbstractTask::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AbstractTask::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12AbstractTaskE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AbstractTask::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void AbstractTask::error(const QString & _t1, const Fountain::Body & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void AbstractTask::summary(const QString & _t1, const Fountain::Body & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void AbstractTask::taskFinished(AbstractTask * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void AbstractTask::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AbstractTask::promptChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AbstractTask::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
