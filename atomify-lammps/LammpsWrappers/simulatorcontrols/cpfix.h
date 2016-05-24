#ifndef CPFIX_H
#define CPFIX_H

#include "simulatorcontrol.h"
class LAMMPSController;
class CPFix : public SimulatorControl
{
    Q_OBJECT
protected:
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual void updateCommand() override;
    virtual QList<QString> resetCommands() override;

public:
    CPFix(Qt3DCore::QNode *parent = 0);
    ~CPFix() { }
    void update(LAMMPSController *lammpsController) override;
    virtual bool existsInLammps(LAMMPSController *lammpsController);

signals:

public slots:
private:
    QT3D_CLONEABLE(CPFix)
};

#endif // CPFIX_H
