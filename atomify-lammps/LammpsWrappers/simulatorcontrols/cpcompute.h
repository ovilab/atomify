#ifndef COMPUTE_H
#define COMPUTE_H
#include "simulatorcontrol.h"
#include "datasource.h"
#include "../cpscalardata.h"

class CPCompute : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(bool isVector READ isVector WRITE setIsVector NOTIFY isVectorChanged)
    Q_PROPERTY(CPScalarData* scalarData READ scalarData WRITE setScalarData NOTIFY scalarDataChanged)
protected:
    void setValues(double time, QVector<double> values);
    virtual void updateCommand() override;
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;
    QString createCommandPrefix() override;
public:
    CPCompute(Qt3DCore::QNode *parent = 0);
    ~CPCompute();
    void copyData(LAMMPSController *lammpsController);
    bool existsInLammps(LAMMPSController *lammpsController) override;
    bool isVector() const;
    QString group() const;
    CPScalarData* scalarData() const;

signals:
    void isVectorChanged(bool isVector);
    void groupChanged(QString group);
    void scalarDataChanged(CPScalarData* scalarData);

public slots:
    void setIsVector(bool isVector);
    void setGroup(QString group);
    void setScalarData(CPScalarData* scalarData);

private:
    CPScalarData* m_scalarData = nullptr;
    bool m_isVector = false;
    double m_time = 0;
    QString m_group = "all";
};

#endif // COMPUTE_H
