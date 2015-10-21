#ifndef NVT_H
#define NVT_H

#include "simulatorcontrol.h"
#include "mysimulator.h"

class NVT : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(double targetTemperature READ targetTemperature WRITE setTargetTemperature NOTIFY targetTemperatureChanged)
    Q_PROPERTY(double temperatureDampening READ temperatureDampening WRITE setTemperatureDampening NOTIFY temperatureDampeningChanged)
private:
    double m_targetTemperature = 1;
    double m_temperatureDampening = 1.0;

public:
    explicit NVT(QObject *parent = 0);
    double targetTemperature() const;

signals:
    void targetTemperatureChanged(double targetTemperature);
    void temperatureDampeningChanged(double temperatureDampening);

public slots:
void setTargetTemperature(double targetTemperature);
void setTemperatureDampening(double temperatureDampening);

public:
virtual void synchronizeLammps(LAMMPSController *lammpsController);
double temperatureDampening() const;
};

#endif // NVT_H
