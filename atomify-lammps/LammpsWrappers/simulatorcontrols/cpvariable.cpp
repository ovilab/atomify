#include "cpvariable.h"
#include <variable.h>
#include <input.h>
#include "lammpscontroller.h"
CPVariable::CPVariable(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{

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
    Error *error = lammps->error;

    try {
        QByteArray bytes = identifier().toLocal8Bit();
        int ivar = variable->find(bytes.data());
        if (ivar < 0) return; // Didn't find it. Weird! TODO: handle this

        if (variable->equalstyle(ivar)) {
            double value = variable->compute_equal(ivar);
        }
    } catch(LAMMPSAbortException & ae) {
        error->set_last_error(ae.message.c_str(), ERROR_NORMAL);
    } catch(LAMMPSException & e) { \
        error->set_last_error(e.message.c_str(), ERROR_NORMAL);
    }

}
