#include "cpvariable.h"
#include <variable.h>
#include <input.h>
#include <library.h>
#include "lammpscontroller.h"
#include "../../dataproviders/data1d.h"
#include "../system.h"
using namespace LAMMPS_NS;

CPVariable::CPVariable(QQuickItem *parent) : SimulatorControl(parent)
{
    setType("Variable");
}

void CPVariable::updateCommand()
{
}

QList<QString> CPVariable::enabledCommands()
{
    return { fullCommand() };
}

QList<QString> CPVariable::disableCommands()
{
    return {QString("variable %1 delete").arg(identifier())};
}

QList<QString> CPVariable::resetCommands()
{
    return { QString("variable %1 delete").arg(identifier()), QString("variable %1 %2").arg(identifier()).arg(command()) };
}

QString CPVariable::createCommandPrefix()
{
    return QString("variable %1 ").arg(identifier());
}

bool CPVariable::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findVariableIndex(identifier())>=0;
}

void CPVariable::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    Variable *variable = lammps->input->variable;

    QByteArray bytes = identifier().toLocal8Bit();
    int ivar = variable->find(bytes.data());
    if (ivar < 0) return; // Didn't find it. Weird! TODO: handle this
    if (variable->equalstyle(ivar)) {
        Data1D *data = ensureExists("scalar", true);
        double value = variable->compute_equal(ivar);
        double time = lammpsController->system->simulationTime();
        data->add(time, value, true);
        setHasScalarData(true);
        setScalarValue(value);
        setInteractive(true);
    }

    if (variable->atomstyle(ivar)) {
        setHasHistogram(true);
        setInteractive(true);
        setIsPerAtom(true);
        Data1D *data = ensureExists("histogram", true);

        if(!windowVisible() && !hovered()) return;
        m_atomData.resize(lammpsController->system->numberOfAtoms());
        double *vector = &m_atomData.front();
        variable->compute_atom(ivar,0 /* group index for all */,vector,1,0);
        if(windowVisible()) {
            data->createHistogram(m_atomData);
        }
    }
}
