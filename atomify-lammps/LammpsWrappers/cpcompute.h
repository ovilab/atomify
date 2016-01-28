#ifndef COMPUTE_H
#define COMPUTE_H
#include "simulatorcontrol.h"
#include "QMLPlot/linegraph.h"
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
    Q_PROPERTY(LineGraphDataSource* dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)

protected:
    bool m_isVector = false;
    double m_time = 0;
    QList<double> m_values;
    void setValues(double time, QVector<double> values);
    LineGraphDataSource* m_dataSource = nullptr;

    virtual void updateCommand() override;
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;

public:
    explicit CPCompute(QQuickItem *parent = 0);
    ~CPCompute() { }
    void update(LAMMPSController *lammpsController) override;
    bool existsInLammps(LAMMPSController *lammpsController) override;
    QList<double> values() const;
    double firstValue() const;
    double secondValue() const;
    double thirdValue() const;
    double fourthValue() const;
    double time() const;
    bool isVector() const;
    LineGraphDataSource* dataSource() const;

signals:
    void valuesChanged(QList<double> values);
    void firstValueChanged(double firstValue);
    void secondValueChanged(double secondValue);
    void thirdValueChanged(double thirdValue);
    void fourthValueChanged(double fourthValue);
    void timeChanged(double time);
    void isVectorChanged(bool isVector);
    void dataSourceChanged(LineGraphDataSource* dataSource);

public slots:
    void setTime(double time);
    void setIsVector(bool isVector);
    void setDataSource(LineGraphDataSource* dataSource);
};

#endif // COMPUTE_H
