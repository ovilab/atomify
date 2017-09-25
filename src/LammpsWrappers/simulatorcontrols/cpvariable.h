#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include <vector>
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
    Q_OBJECT
public:
    CPVariable(Qt3DCore::QNode *parent = 0);
    virtual bool existsInLammps(class LAMMPSController *lammpsController);
    void synchronize(class LAMMPSController *lammpsController);

public slots:

signals:

protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();
    QString createCommandPrefix();

private:

};

#endif // CPVARIABLE_H
