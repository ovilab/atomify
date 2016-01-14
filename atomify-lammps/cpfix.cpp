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
    // TODO: Use findFixIndex>=0 ?
    LAMMPS_NS::Fix *fix = lammpsController->findFixByIdentifier(identifier());
    return fix!=nullptr;
}


QList<QString> CPFix::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), QString("fix %1 %2").arg(identifier()).arg(command()) };
}
