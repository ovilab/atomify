#include "mousemover.h"

#include <QApplication>
#include <QCursor>
#include <QQuickItem>
#include <QQuickWindow>

MouseMover::MouseMover(QObject* parent)
    : QObject(parent)
    , m_window(0)
{
}

MouseMover::~MouseMover()
{
}

bool MouseMover::showCursor() const
{
    return m_showCursor;
}

QWindow* MouseMover::window() const
{
    return m_window;
}

void MouseMover::move(int x, int y)
{
    if (!m_window) {
        qWarning() << "Warning: Haven't set window on MouseMover";
        return;
    } else {
        QPoint globalPoint = m_window->mapToGlobal(QPoint(x, y));
        QCursor::setPos(globalPoint);
    }
}

void MouseMover::setShowCursor(bool arg)
{
    if (m_showCursor != arg) {
        m_showCursor = arg;
        if (m_showCursor) {
            QApplication::setOverrideCursor(QCursor());
            QApplication::changeOverrideCursor(QCursor());
        } else {
            QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
            QApplication::changeOverrideCursor(QCursor(Qt::BlankCursor));
        }
        emit showCursorChanged(arg);
    }
}

void MouseMover::setWindow(QWindow* window)
{
    if (m_window == window)
        return;

    m_window = window;
    emit windowChanged(window);
}
