#ifndef NVT_H
#define NVT_H

#include <QObject>
#include "mysimulator.h"

class NVT : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double targetTemperature READ targetTemperature WRITE setTargetTemperature NOTIFY targetTemperatureChanged)
    Q_PROPERTY(MySimulator* simulator READ simulator WRITE setSimulator NOTIFY simulatorChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
private:
    double m_targetTemperature = 1;
    MySimulator* m_simulator = NULL;
    bool m_enabled = false;

public:
    explicit NVT(QObject *parent = 0);
    double targetTemperature() const;
    MySimulator* simulator() const;
    bool enabled() const;

signals:

    void targetTemperatureChanged(double targetTemperature);
    void simulatorChanged(MySimulator* simulator);
    void enabledChanged(bool enabled);

public slots:
    void setTargetTemperature(double targetTemperature);
    void setSimulator(MySimulator* simulator);
    void setEnabled(bool enabled);
};

#endif // NVT_H
