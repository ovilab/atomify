#include "runcommand.h"

RunCommand::RunCommand(int start, int stop) {
    startTimestep = start;
    stopTimestep = stop;
}

QString RunCommand::nextCommand(unsigned long currentTimestep, int simulationSpeed, bool preRunNeeded)
{
    int finalSimulationSpeed = simulationSpeed;
    if(currentTimestep + simulationSpeed >= stopTimestep) {
        // If simulationSpeed is 10, but there is only 8 timesteps left until endTimestep,
        // cap simulation speed on 8 so we land on endTimestep.
        finalSimulationSpeed = stopTimestep - currentTimestep;
        finished = true;
    }

    QString preYesNoString = preRunNeeded ? "yes" : "no";
    QString command = QString("run %1 pre %2 post no start %3 stop %4").arg(finalSimulationSpeed).arg(preYesNoString).arg(startTimestep).arg(stopTimestep);
    return command;
}
