#include "simulatorcontrol.h"

SimulatorControl::SimulatorControl(QObject *parent) : QObject(parent)
{

}

bool SimulatorControl::dirty() const
{
    return m_dirty;
}

bool SimulatorControl::enabled() const
{
    return m_enabled;
}

void SimulatorControl::setDirty(bool dirty)
{
    if (m_dirty == dirty)
        return;

    m_dirty = dirty;
    emit dirtyChanged(dirty);
}

void SimulatorControl::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;
    setDirty(true);
    m_enabled = enabled;
    emit enabledChanged(enabled);
}

