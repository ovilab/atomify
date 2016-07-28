#include "cpvariable.h"
#include <variable.h>
#include "lammpscontroller.h"
#include <library.h>
CPVariable::CPVariable()
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


void CPVariable::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);
    if(!lammpsController->lammps())  {
        return;
    }
    QByteArray identifier_ = identifier().toLocal8Bit();
    double *variableValue = (double*)lammps_extract_variable(lammpsController->lammps(), identifier_.data(), "all");
    setValue(*variableValue);
    lammps_free(variableValue);
}

double CPVariable::value() const
{
    return m_value;
}

void CPVariable::setValue(double value)
{
    m_value = value;
    emit valueChanged(value);
}
