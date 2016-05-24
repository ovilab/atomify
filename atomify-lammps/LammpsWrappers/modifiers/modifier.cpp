#include "modifier.h"
#include "../system.h"

Modifier::Modifier(System *system) : m_system(system)
{

}

bool Modifier::enabled() const
{
    return m_enabled;
}

void Modifier::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}
