/*
 *  SPDX-FileCopyrightText: 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_tablet_debugger.h"

#include <QEvent>
#include <QMessageBox>
#include <QApplication>

#include <kis_debug.h>
#include <kis_config.h>
#include <KisPortingUtils.h>

#include <QGlobalStatic>

#include <klocalizedstring.h>

Q_GLOBAL_STATIC(KisTabletDebugger, s_instance)


inline QString button(const QWheelEvent &ev) {
    Q_UNUSED(ev);
    return "-";
}

template <class T>
QString button(const T &ev) {
    return QString::number(ev.button());
}

template <class T>
QString buttons(const T &ev) {
    return QString::number(ev.buttons());
}

template <class Event>
    void dumpBaseParams(QTextStream &s, const Event &ev, const QString &prefix)
{
    s << qSetFieldWidth(5)  << Qt::left << prefix << Qt::reset << " ";
    s << qSetFieldWidth(17) << Qt::left << KisTabletDebugger::exTypeToString(ev.type()) << Qt::reset;
}

template <class Event>
    void dumpMouseRelatedParams(QTextStream &s, const Event &ev)
{
    s << "btn: " << button(ev) << " ";
    s << "btns: " << buttons(ev) << " ";
    s << "pos: " << qSetFieldWidth(4) << ev.x() << qSetFieldWidth(0) << "," << qSetFieldWidth(4) << ev.y() << qSetFieldWidth(0) << " ";
    s << "gpos: "  << qSetFieldWidth(4) << ev.globalX() << qSetFieldWidth(0) << "," << qSetFieldWidth(4) << ev.globalY() << qSetFieldWidth(0) << " ";
}

template <>
    void dumpMouseRelatedParams(QTextStream &s, const QWheelEvent &ev)
{
    s << "btn: " << button(ev) << " ";
    s << "btns: " << buttons(ev) << " ";
    s << "pos: " << qSetFieldWidth(4) << ev.position().x() << qSetFieldWidth(0) << "," << qSetFieldWidth(4) << ev.position().y() << qSetFieldWidth(0) << " ";
    s << "gpos: "  << qSetFieldWidth(4) << ev.globalPosition().x() << qSetFieldWidth(0) << "," << qSetFieldWidth(4) << ev.globalPosition().y() << qSetFieldWidth(0) << " ";
}

QString KisTabletDebugger::exTypeToString(QEvent::Type type) {
    return
        type == QEvent::TabletEnterProximity ? "TabletEnterProximity" :
        type == QEvent::TabletLeaveProximity ? "TabletLeaveProximity" :
        type == QEvent::Enter ? "Enter" :
        type == QEvent::Leave ? "Leave" :
        type == QEvent::FocusIn ? "FocusIn" :
        type == QEvent::FocusOut ? "FocusOut" :
        type == QEvent::Wheel ? "Wheel" :
        type == QEvent::KeyPress ? "KeyPress" :
        type == QEvent::KeyRelease ? "KeyRelease" :
        type == QEvent::ShortcutOverride ? "ShortcutOverride" :
        type == QMouseEvent::MouseButtonPress ? "MouseButtonPress" :
        type == QMouseEvent::MouseButtonRelease ? "MouseButtonRelease" :
        type == QMouseEvent::MouseButtonDblClick ? "MouseButtonDblClick" :
        type == QMouseEvent::MouseMove ? "MouseMove" :
        type == QTabletEvent::TabletMove ? "TabletMove" :
        type == QTabletEvent::TabletPress ? "TabletPress" :
        type == QTabletEvent::TabletRelease ? "TabletRelease" :
        type == QTouchEvent::TouchBegin ? "TouchBegin" :
        type == QTouchEvent::TouchUpdate ? "TouchUpdate" :
        type == QTouchEvent::TouchEnd ? "TouchEnd" :
        type == QTouchEvent::TouchCancel ? "TouchCancel" :
        "unknown";
}


KisTabletDebugger::KisTabletDebugger()
    : m_debugEnabled(false)
{
    KisConfig cfg(true);
    m_shouldEatDriverShortcuts = cfg.shouldEatDriverShortcuts();
}

KisTabletDebugger* KisTabletDebugger::instance()
{
    return s_instance;
}

void KisTabletDebugger::toggleDebugging()
{
    m_debugEnabled = !m_debugEnabled;
    QMessageBox::information(qApp->activeWindow(), i18nc("@title:window", "Krita"), m_debugEnabled ?
                             i18n("Tablet Event Logging Enabled") :
                             i18n("Tablet Event Logging Disabled"));
    if (m_debugEnabled) {
        dbgTablet << "vvvvvvvvvvvvvvvvvvvvvvv START TABLET EVENT LOG vvvvvvvvvvvvvvvvvvvvvvv";
    }
    else {
        dbgTablet << "^^^^^^^^^^^^^^^^^^^^^^^ END TABLET EVENT LOG ^^^^^^^^^^^^^^^^^^^^^^^";
    }
}

bool KisTabletDebugger::debugEnabled() const
{
    return m_debugEnabled;
}

bool KisTabletDebugger::initializationDebugEnabled() const
{
    // FIXME: make configurable!
    return true;
}

bool KisTabletDebugger::debugRawTabletValues() const
{
    // FIXME: make configurable!
    return m_debugEnabled;
}

bool KisTabletDebugger::shouldEatDriverShortcuts() const
{
    return m_shouldEatDriverShortcuts;
}

QString KisTabletDebugger::eventToString(const QMouseEvent &ev, const QString &prefix)
{
    QString string;
    QTextStream s(&string);

    dumpBaseParams(s, ev, prefix);
    dumpMouseRelatedParams(s, ev);
    s << "hires: " << qSetFieldWidth(8) << ev.screenPos().x() << qSetFieldWidth(0) << "," << qSetFieldWidth(8) << ev.screenPos().y() << qSetFieldWidth(0) << " ";
    s << "Source:" << ev.source();

    return string;
}

QString KisTabletDebugger::eventToString(const QKeyEvent &ev, const QString &prefix)
{
    QString string;
    QTextStream s(&string);
    KisPortingUtils::setUtf8OnStream(s);

    dumpBaseParams(s, ev, prefix);

    s << "key: 0x" << Qt::hex << ev.key() << Qt::reset << " ";
    s << "mod: 0x" << Qt::hex << ev.modifiers() << Qt::reset << " ";
    s << "text: " << (ev.text().isEmpty() ? "none" : ev.text()) << " ";
    s << "autorepeat: " << bool(ev.isAutoRepeat());

    return string;
}

QString KisTabletDebugger::eventToString(const QWheelEvent &ev, const QString &prefix)
{
    QString string;
    QTextStream s(&string);
    KisPortingUtils::setUtf8OnStream(s);

    dumpBaseParams(s, ev, prefix);
    dumpMouseRelatedParams(s, ev);

    s << "delta: x: " << ev.angleDelta().x() << " y: " << ev.angleDelta().y() << " ";

    return string;
}

QString KisTabletDebugger::eventToString(const QTouchEvent &ev, const QString &prefix)
{
    QString string;
    QTextStream s(&string);
    KisPortingUtils::setUtf8OnStream(s);

    dumpBaseParams(s, ev, prefix);
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    s << (ev.device()->type() ? "TouchPad" : "TouchScreen") << " ";
#else
    if (ev.deviceType() == QInputDevice::DeviceType::TouchPad) {
        s << "Touchpad";
    }
    else if (ev.deviceType() == QInputDevice::DeviceType::TouchScreen) {
        s << "TouchScreen";
    }
    s << " ";
#endif
    for (const auto& touchpoint: ev.touchPoints()) {
        s << "id: " << touchpoint.id() << " ";
        s << "hires: " << qSetFieldWidth(8) << touchpoint.screenPos().x() << qSetFieldWidth(0) << "," << qSetFieldWidth(8) << touchpoint.screenPos().y() << qSetFieldWidth(0) << " ";
        s << "prs: " << touchpoint.pressure() << " ";
        s << "rot: "<< touchpoint.rotation() << " ";
        s << "state: 0x" << Qt::hex << touchpoint.state() << "; ";
        s << Qt::dec;
    }

    return string;
}

QString KisTabletDebugger::eventToString(const QEvent &ev, const QString &prefix)
{
    QString string;
    QTextStream s(&string);
    KisPortingUtils::setUtf8OnStream(s);

    dumpBaseParams(s, ev, prefix);

    return string;
}

template <class Event>
    QString tabletEventToString(const Event &ev, const QString &prefix)
{
    QString string;
    QTextStream s(&string);
    KisPortingUtils::setUtf8OnStream(s);

    dumpBaseParams(s, ev, prefix);
    dumpMouseRelatedParams(s, ev);

    s << "hires: " << qSetFieldWidth(8) << ev.globalPosF().x() << qSetFieldWidth(0) << "," << qSetFieldWidth(8) << ev.globalPosF().y() << qSetFieldWidth(0) << " ";
    s << "prs: " << qSetFieldWidth(4) << Qt::fixed << ev.pressure() << Qt::reset << " ";


    s << KisTabletDebugger::tabletDeviceToString(ev) << " ";
    s << KisTabletDebugger::pointerTypeToString(ev) << " ";

    s << "id: " << ev.uniqueId() << " ";

    s << "xTilt: " << ev.xTilt() << " ";
    s << "yTilt: " << ev.yTilt() << " ";
    s << "rot: " << ev.rotation() << " ";
    s << "z: " << ev.z() << " ";
    s << "tp: " << ev.tangentialPressure() << " ";

    return string;
}

QString KisTabletDebugger::eventToString(const QTabletEvent &ev, const QString &prefix)
{
    return tabletEventToString(ev, prefix);
}

QString KisTabletDebugger::tabletDeviceToString(const QTabletEvent &event)
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    return
        event.deviceType() == QTabletEvent::NoDevice ? "NoDevice" :
        event.deviceType() == QTabletEvent::Puck ? "Puck" :
        event.deviceType() == QTabletEvent::Stylus ? "Stylus" :
        event.deviceType() == QTabletEvent::Airbrush ? "Airbrush" :
        event.deviceType() == QTabletEvent::FourDMouse ? "FourDMouse" :
        event.deviceType() == QTabletEvent::XFreeEraser ? "XFreeEraser" :
        event.deviceType() == QTabletEvent::RotationStylus ? "RotationStylus" :
        "Unknown";
#else
    QInputDevice::DeviceType deviceType = event.deviceType();

    return
        deviceType == QInputDevice::DeviceType::Puck ? "Puck" :
        deviceType == QInputDevice::DeviceType::Stylus ? "Stylus" :
        deviceType == QInputDevice::DeviceType::Airbrush ? "Airbrush" :
        "Unknown";
#endif
}

QString KisTabletDebugger::pointerTypeToString(const QTabletEvent &event) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    return
        event.pointerType() == QTabletEvent::UnknownPointer ? "UnknownPointer" :
        event.pointerType() == QTabletEvent::Pen ? "Pen" :
        event.pointerType() == QTabletEvent::Cursor ? "Cursor" :
        event.pointerType() == QTabletEvent::Eraser ? "Eraser" :
        "Unknown";
#else
    QPointingDevice::PointerType pointerType = event.pointerType();
    return
        pointerType == QPointingDevice::PointerType::Pen ? "Pen" :
        pointerType == QPointingDevice::PointerType::Cursor ? "Cursor" :
        pointerType == QPointingDevice::PointerType::Eraser ? "Eraser" :
        "Unknown";
#endif

}

