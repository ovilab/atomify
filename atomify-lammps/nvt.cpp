#include "nvt.h"
#include "lammpscontroller.h"
#include <QString>
#include <QRegularExpression>
#include <QDebug>

NVT::NVT(QObject *parent) : CPFix(parent)
{
    setIdentifier(QString("cp_nvt"));
    updateCommand();
}

double NVT::targetTemperature() const
{
    return m_targetTemperature;
}

void NVT::setTargetTemperature(double targetTemperature)
{
    if (m_targetTemperature == targetTemperature)
        return;

    m_targetTemperature = targetTemperature;
    emit targetTemperatureChanged(targetTemperature);
}

void NVT::setTemperatureDampening(double temperatureDampening)
{
    if (m_temperatureDampening == temperatureDampening)
        return;

    m_temperatureDampening = temperatureDampening;
    emit temperatureDampeningChanged(temperatureDampening);
}

double NVT::temperatureDampening() const
{
    return m_temperatureDampening;
}

void NVT::updateCommand()
{
    QString newCommand = QString("all nvt temp %1 %1 %2").arg(m_targetTemperature).arg(m_temperatureDampening);
    setCommand(newCommand);
}

QList<QString> NVT::enabledCommands()
{
    QString enableCommand = QString("fix %1 %2").arg(identifier()).arg(command());
    return {QString("#/disableAllEnsembleFixes"), enableCommand};
}

QList<QString> NVT::disableCommands()
{
    return { QString("unfix %1").arg(identifier()), QString("fix cp_nve all nve") };
}
