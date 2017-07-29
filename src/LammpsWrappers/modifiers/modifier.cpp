#include "modifier.h"
#include "../system.h"

Modifier::Modifier()
{

}

bool Modifier::enabled() const
{
    return m_enabled;
}

void Modifier::setSystem(System *system)
{
    m_system = system;
}

void Modifier::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}
