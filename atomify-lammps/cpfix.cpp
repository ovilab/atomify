#include "cpfix.h"
#include "lammpscontroller.h"

CPFix::CPFix(QQuickItem *parent) : SimulatorControl(parent)
{
}

void CPFix::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);
}

QList<QString> CPFix::enabledCommands()
{
    return {QString("fix %1 %2").arg(identifier()).arg(command())};
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


QList<QString> CPFix::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), QString("fix %1 %2").arg(identifier()).arg(command()) };
}
