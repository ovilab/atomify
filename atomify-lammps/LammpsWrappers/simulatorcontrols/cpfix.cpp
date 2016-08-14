#include "cpfix.h"
#include "lammpscontroller.h"
#include "../system.h"

CPFix::CPFix(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{
}

CPFix::~CPFix() { }

void CPFix::copyData(LAMMPSController *lammpsController)
{
    // if(lammpsController->system()->timesteps() % m_frequency != 0) return;
    if(lastUpdate != -1 && (lammpsController->system()->timesteps()-lastUpdate) < m_frequency) return;
    // if(lammpsController->system()->timesteps() % m_frequency != 0) return;
    Fix *lmp_fix = lammpsController->findFixByIdentifier(identifier());
    if(lmp_fix == nullptr) return;
}

bool CPFix::interactive() const
{
    return m_interactive;
}

void CPFix::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);
}

QList<QString> CPFix::enabledCommands()
{
    return { fullCommand() };
}

QList<QString> CPFix::disableCommands()
{
    return {QString("unfix %1 ").arg(identifier())};
}

void CPFix::updateCommand()
{

}

bool CPFix::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->fixExists(identifier());
}

QString CPFix::group() const
{
    return m_group;
}

int CPFix::frequency() const
{
    return m_frequency;
}

void CPFix::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(group);
}

void CPFix::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(frequency);
}

void CPFix::setInteractive(bool interactive)
{
    if (m_interactive == interactive)
        return;

    m_interactive = interactive;
    emit interactiveChanged(interactive);
}


QList<QString> CPFix::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), fullCommand() };
}

QString CPFix::createCommandPrefix()
{
    return QString("fix %1 %2 ").arg(identifier()).arg(group());
}
