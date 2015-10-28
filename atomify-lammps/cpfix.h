#ifndef CPFIX_H
#define CPFIX_H

#include "simulatorcontrol.h"
class LAMMPSController;
class CPFix : public SimulatorControl
{
    Q_OBJECT
protected:
    QString enabledCommand() override;
    QString disableCommand() override;

public:
    CPFix();
    void update(LAMMPSController *lammpsController) override;
    virtual bool existsInLammps(LAMMPSController *lammpsController);

signals:

public slots:

    // SimulatorControl interface
protected:
    virtual void updateCommand();
};

#endif // CPFIX_H
