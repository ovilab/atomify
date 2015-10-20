#include "nvt.h"

NVT::NVT(QObject *parent) : QObject(parent)
{

}

double NVT::targetTemperature() const
{
    return m_targetTemperature;
}

AtomifySimulator *NVT::simulator() const
{
    return m_simulator;
}

bool NVT::enabled() const
{
    return m_enabled;
}

void NVT::setTargetTemperature(double targetTemperature)
{
    if (m_targetTemperature == targetTemperature)
        return;

    m_targetTemperature = targetTemperature;
    emit targetTemperatureChanged(targetTemperature);
}

void NVT::setSimulator(AtomifySimulator *simulator)
{
    if (m_simulator == simulator)
        return;

    m_simulator = simulator;
    emit simulatorChanged(simulator);
}

void NVT::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}

