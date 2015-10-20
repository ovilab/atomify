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
    virtual void synchronizeLammps(LAMMPSController *lammpsController) = 0;
signals:

public slots:
};

#endif // SIMULATORCONTROL_H
