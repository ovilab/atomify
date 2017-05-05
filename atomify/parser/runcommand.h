#ifndef RUNCOMMAND_H
#define RUNCOMMAND_H

#include <QString>

struct RunCommand
{
    RunCommand(int start, int stop);
    unsigned long startTimestep = 0;
    unsigned long stopTimestep = 0;
    bool finished = false;
    QString nextCommand(unsigned long currentTimestep, int simulationSpeed, bool preRunNeeded);
};

#endif // RUNCOMMAND_H
