#include "cpfix.h"
#include "lammpscontroller.h"

CPFix::CPFix(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{
}

CPFix::~CPFix() { }

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
    return lammpsController->findFixIndex(identifier())>=0;
}

QString CPFix::group() const
{
    return m_group;
}

void CPFix::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(group);
}


QList<QString> CPFix::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), fullCommand() };
}

QString CPFix::createCommandPrefix()
{
    return QString("fix %1 %2 ").arg(identifier()).arg(group());
}
