#ifndef SIMULATORCONTROL_H
#define SIMULATORCONTROL_H

#include <QObject>
class LAMMPSController;

class SimulatorControl : public QObject
{
    Q_OBJECT
private:

public:
    explicit SimulatorControl(QObject *parent = 0);
    virtual void processTick(LAMMPSController *lammpsController) {}
    virtual void refreshAfterCommand(LAMMPSController *lammpsController) {}
signals:

public slots:
};

#endif // SIMULATORCONTROL_H
