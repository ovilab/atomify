#include "cpfix.h"
#include "lammpscontroller.h"

CPFix::CPFix()
{

}

void CPFix::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);

}


QString CPFix::enabledCommand()
{
    return QString("fix %1 %2").arg(identifier()).arg(command());
}

QString CPFix::disableCommand()
{
    return QString("unfix %1 ").arg(identifier());
}


void CPFix::updateCommand()
{
}

bool CPFix::existsInLammps(LAMMPSController *lammpsController)
{
    LAMMPS_NS::Fix *fix = lammpsController->findFixByIdentifier(identifier());
    return fix!=nullptr;
}
