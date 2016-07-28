#ifndef CPVARIABLE_H
#define CPVARIABLE_H
#include "simulatorcontrol.h"

class CPVariable : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)

public:
    CPVariable();

    // SimulatorControl interface
protected:
    virtual void updateCommand();
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
    virtual QList<QString> resetCommands();

public:
    virtual bool existsInLammps(class LAMMPSController *lammpsController) override;
    virtual void update(LAMMPSController *lammpsController) override;
    double value() const;

public slots:
    void setValue(double value);

signals:
    void valueChanged(double value);

private:
    double m_value = 0;
};

#endif // CPVARIABLE_H
