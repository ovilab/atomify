#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
    Q_OBJECT
public:
    CPVariable(Qt3DCore::QNode *parent = 0);

    // SimulatorControl interface
protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();
    QString createCommandPrefix();
public:
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
};

#endif // CPVARIABLE_H
