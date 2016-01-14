#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
public:
    CPVariable();

    // SimulatorControl interface
protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();

public:
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
};

#endif // CPVARIABLE_H
