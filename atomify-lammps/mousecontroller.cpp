#include "mousecontroller.h"
#include <QApplication>
MouseController::MouseController(QObject *parent) : QObject(parent)
{

}

bool MouseController::fpsMode() const
{
    return m_fpsMode;
}

void MouseController::setFpsMode(bool fpsMode)
{
    if (m_fpsMode == fpsMode)
        return;

    m_fpsMode = fpsMode;
    emit fpsModeChanged(fpsMode);
}
