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
    explicit CPFix(QQuickItem *parent = 0);
    ~CPFix() { }
    void update(LAMMPSController *lammpsController) override;
    virtual bool existsInLammps(LAMMPSController *lammpsController);

signals:

public slots:

};

#endif // CPFIX_H
