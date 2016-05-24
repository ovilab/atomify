#include "cpvariable.h"
#include <variable.h>
#include "lammpscontroller.h"
CPVariable::CPVariable(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{

}



void CPVariable::updateCommand()
{
}

QList<QString> CPVariable::enabledCommands()
{
    return {QString("variable %1 %2").arg(identifier()).arg(command())};
}

QList<QString> CPVariable::disableCommands()
{
    return {QString("variable %1 delete").arg(identifier())};
}

QList<QString> CPVariable::resetCommands()
{
    return { QString("variable %1 delete").arg(identifier()), QString("variable %1 %2").arg(identifier()).arg(command()) };
}

bool CPVariable::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findVariableIndex(identifier())>=0;
}
