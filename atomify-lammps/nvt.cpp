#include "nvt.h"

NVT::NVT(QObject *parent) : QObject(parent)
{

}

double NVT::targetTemperature() const
{
    return m_targetTemperature;
}

MySimulator *NVT::simulator() const
{
    return m_simulator;
}

void NVT::setTargetTemperature(double targetTemperature)
{
    if (m_targetTemperature == targetTemperature)
        return;

    m_targetTemperature = targetTemperature;
    emit targetTemperatureChanged(targetTemperature);
}

void NVT::setSimulator(MySimulator *simulator)
{
    if (m_simulator == simulator)
        return;

    m_simulator = simulator;
    emit simulatorChanged(simulator);
}

