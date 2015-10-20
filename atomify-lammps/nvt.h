#ifndef NVT_H
#define NVT_H

#include "simulatorcontrol.h"
#include "mysimulator.h"

class NVT : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(double targetTemperature READ targetTemperature WRITE setTargetTemperature NOTIFY targetTemperatureChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool dirty READ dirty WRITE setDirty NOTIFY dirtyChanged)
private:
    double m_targetTemperature = 1;
    bool m_enabled = false;
    bool m_dirty = false;

public:
    explicit NVT(QObject *parent = 0);
    double targetTemperature() const;
    bool enabled() const;

signals:
    void targetTemperatureChanged(double targetTemperature);
    void enabledChanged(bool enabled);
    void dirtyChanged(bool dirty);

public slots:
    void setTargetTemperature(double targetTemperature);
    void setEnabled(bool enabled);
    // SimulatorControl interface
    void setDirty(bool dirty);
public:

    virtual void processTick(LAMMPSController *lammpsController) override;
    virtual void refreshAfterCommand(LAMMPSController *lammpsController);
    bool dirty() const;
};

#endif // NVT_H
