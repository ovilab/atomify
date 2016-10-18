#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
    Q_OBJECT
public:
    CPVariable(Qt3DCore::QNode *parent = 0);
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
    void synchronize(class LAMMPSController *lammpsController);

    // SimulatorControl interface
protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();
    QString createCommandPrefix();
};

#endif // CPVARIABLE_H
