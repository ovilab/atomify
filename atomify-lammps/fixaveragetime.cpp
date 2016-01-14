#include "fixaveragetime.h"
#include "lammpscontroller.h"
#include <fix_ave_time.h>
FixAverageTime::FixAverageTime()
{

}

void FixAverageTime::updateCommand()
{

}

QList<QString> FixAverageTime::enabledCommands()
{
    return {QString("fix %1 %2").arg(identifier()).arg(command())};
}

QList<QString> FixAverageTime::disableCommands()
{
    return {QString("unfix %1 ").arg(identifier())};
}

QList<QString> FixAverageTime::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), QString("fix %1 %2").arg(identifier()).arg(command()) };
}

bool FixAverageTime::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findFixIndex(identifier())>=0;
}
