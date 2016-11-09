#ifndef RUNCOMMAND_H
#define RUNCOMMAND_H

#include <QString>

struct RunCommand
{
    RunCommand() { }
    unsigned long startTimestep = 0;
    unsigned long endTimestep = 0;
    bool finished = false;
    QString nextCommand(unsigned long currentTimestep, int simulationSpeed, bool preRunNeeded);
};

#endif // RUNCOMMAND_H
