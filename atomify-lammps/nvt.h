#ifndef NVT_H
#define NVT_H

#include "simulatorcontrol.h"
#include "mysimulator.h"

class NVT : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(double targetTemperature READ targetTemperature WRITE setTargetTemperature NOTIFY targetTemperatureChanged)
private:
    double m_targetTemperature = 1;

public:
    explicit NVT(QObject *parent = 0);
    double targetTemperature() const;

signals:
    void targetTemperatureChanged(double targetTemperature);

public slots:
    void setTargetTemperature(double targetTemperature);
public:
    virtual void synchronizeLammps(LAMMPSController *lammpsController);
};

#endif // NVT_H
