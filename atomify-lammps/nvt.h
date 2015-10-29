#ifndef NVT_H
#define NVT_H

#include "cpfix.h"
#include "mysimulator.h"

class NVT : public CPFix
{
    Q_OBJECT
    Q_PROPERTY(double targetTemperature READ targetTemperature WRITE setTargetTemperature NOTIFY targetTemperatureChanged)
    Q_PROPERTY(double temperatureDampening READ temperatureDampening WRITE setTemperatureDampening NOTIFY temperatureDampeningChanged)
private:
    double m_targetTemperature = 1.0;
    double m_temperatureDampening = 1.0;

public:
    explicit NVT(QObject *parent = 0);
    ~NVT() { }
    double targetTemperature() const;
    double temperatureDampening() const;

signals:
    void targetTemperatureChanged(double targetTemperature);
    void temperatureDampeningChanged(double temperatureDampening);

public slots:
    void setTargetTemperature(double targetTemperature);
    void setTemperatureDampening(double temperatureDampening);

    // SimulatorControl interface
protected:
    virtual void updateCommand() override;
    virtual QList<QString> enabledCommands();
    virtual QList<QString> disableCommands();
};

#endif // NVT_H
