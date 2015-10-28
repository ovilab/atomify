#ifndef COMPUTE_H
#define COMPUTE_H
#include "simulatorcontrol.h"
class CPCompute : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(QList<double> values READ values NOTIFY valuesChanged)
    Q_PROPERTY(double value READ firstValue NOTIFY firstValueChanged)
    Q_PROPERTY(double firstValue READ firstValue NOTIFY firstValueChanged)
    Q_PROPERTY(double secondValue READ secondValue NOTIFY secondValueChanged)
    Q_PROPERTY(double thirdValue READ thirdValue NOTIFY thirdValueChanged)
    Q_PROPERTY(double fourthValue READ fourthValue NOTIFY fourthValueChanged)
    Q_PROPERTY(double time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(bool isVector READ isVector WRITE setIsVector NOTIFY isVectorChanged)

protected:
    bool m_isVector = false;
    double m_time = 0;
    QList<double> m_values;
    void setValues(double time, QVector<double> values);

    virtual void updateCommand() override;
    QString enabledCommand() override;
    QString disableCommand() override;
public:
    CPCompute();
    void update(LAMMPSController *lammpsController) override;
    bool existsInLammps(LAMMPSController *lammpsController) override;
    QList<double> values() const;
    double firstValue() const;
    double secondValue() const;
    double thirdValue() const;
    double fourthValue() const;
    double time() const;
    bool isVector() const;

signals:
    void valuesChanged(QList<double> values);
    void firstValueChanged(double firstValue);
    void secondValueChanged(double secondValue);
    void thirdValueChanged(double thirdValue);
    void fourthValueChanged(double fourthValue);
    void timeChanged(double time);
    void isVectorChanged(bool isVector);

public slots:
    void setTime(double time);
    void setIsVector(bool isVector);
};

#endif // COMPUTE_H
