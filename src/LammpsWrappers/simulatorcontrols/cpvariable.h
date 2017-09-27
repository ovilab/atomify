#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include <vector>
#include "simulatorcontrol.h"
class CPVariable : public SimulatorControl
{
    Q_OBJECT
public:
    CPVariable(QQuickItem *parent = 0);
    virtual bool existsInLammps(class LAMMPSController *lammpsController) override;
    void synchronize(class LAMMPSController *lammpsController);

public slots:

signals:

protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands() override;
    virtual QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;
    virtual QString createCommandPrefix() override;

private:

};

#endif // CPVARIABLE_H
