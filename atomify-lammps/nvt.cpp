#include "nvt.h"
#include "lammpscontroller.h"
#include <fix_nvt.h>
#include <QString>

class FixNHHack : public FixNH {
public:
    double target() { return t_target; }
    void setTargets(double targetValue) {
        t_target = targetValue;
        t_start = targetValue;
        t_stop = targetValue;
    }
};

NVT::NVT(QObject *parent) : SimulatorControl(parent)
{

}

double NVT::targetTemperature() const
{
    return m_targetTemperature;
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
    m_dirty = true;
    emit targetTemperatureChanged(targetTemperature);
}

void NVT::setEnabled(bool enabled)
{
    qDebug() << "Setting enabled: " << enabled << " (m_enabled: " << m_enabled << ")";

    if (m_enabled == enabled)
        return;
    m_dirty = true;
    m_enabled = enabled;
    emit enabledChanged(enabled);
}

void NVT::setDirty(bool dirty)
{
    qDebug() << "Setting dirty: " << dirty << " (m_dirty: " << m_dirty << ")";
    if (m_dirty == dirty)
        return;

    m_dirty = dirty;
    emit dirtyChanged(dirty);
}


void NVT::synchronizeLammps(LAMMPSController *lammpsController)
{
    FixNVT *fix = lammpsController->findFixByType<FixNVT>();
    if(fix) {
        qDebug() << "Found fix";
        FixNH *fixNH = dynamic_cast<FixNH*>(fix);
        FixNHHack *fixHack = reinterpret_cast<FixNHHack*>(fixNH);
        if(fixHack) {
            qDebug() << "Found fix hack";
            if(m_dirty) {
                qDebug() << "I am dirty";
                m_dirty = false;

                if(!m_enabled) {
                    QList<QString> disableCommands;
                    disableCommands.push_back(QString("unfix %1").arg(fix->id));
                    disableCommands.push_back("fix nve all nve");
                    qDebug() << "Commands: " << disableCommands;
                    lammpsController->scriptHandler()->addCommandsToTop(disableCommands, CommandInfo(CommandInfo::Type::SingleCommand));
                    return;
                }

                if(m_targetTemperature != fixHack->target()) {
                    fixHack->setTargets(m_targetTemperature);
                }
            } else {
                qDebug() << "I am not dirty";
                if(m_targetTemperature != fixHack->target()) {
                    qDebug() << "temp changed";
                    m_targetTemperature = fixHack->target();
                    emit targetTemperatureChanged(m_targetTemperature);
                }
                if(!m_enabled) {
                    qDebug() << "enabled changed";
                    m_enabled = true;
                    emit enabledChanged(m_enabled);
                }
            }
        }
    } else {
        if(m_dirty && m_enabled) {
            lammpsController->disableAllEnsembleFixes();
            QString command = QString("fix nvt all nvt temp %1 %1 1.0").arg(m_targetTemperature);
            lammpsController->scriptHandler()->addCommandToTop(command, CommandInfo(CommandInfo::Type::SingleCommand));
        }
    }

    m_dirty = false;
}

bool NVT::dirty() const
{
    return m_dirty;
}
