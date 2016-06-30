#ifndef COMPUTE_H
#define COMPUTE_H
#include "simulatorcontrol.h"
#include "datasource.h"
#include "../../dataproviders/dataprovider.h"

#include <QVariantMap>

class CPCompute : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(bool isVector READ isVector WRITE setIsVector NOTIFY isVectorChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(bool hasScalarData READ hasScalarData WRITE setHasScalarData NOTIFY hasScalarDataChanged)
    Q_PROPERTY(float scalarValue READ scalarValue WRITE setScalarValue NOTIFY scalarValueChanged)
    Q_PROPERTY(int num1DData READ num1DData WRITE setNum1DData NOTIFY num1DDataChanged)
    Q_PROPERTY(QVariantMap data1D READ data1D WRITE setData1D NOTIFY data1DChanged)
protected:
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
    int frequency() const;
    bool hasScalarData() const;
    float scalarValue() const;
    int num1DData() const;
    QVariantMap data1D() const;

signals:
    void isVectorChanged(bool isVector);
    void groupChanged(QString group);
    void frequencyChanged(int frequency);
    void hasScalarDataChanged(bool hasScalarData);
    void scalarValueChanged(float scalarValue);
    void num1DDataChanged(int num1DData);
    void data1DChanged(QVariantMap data1D);

public slots:
    void setIsVector(bool isVector);
    void setGroup(QString group);
    void setFrequency(int frequency);
    void setHasScalarData(bool hasScalarData);
    void setScalarValue(float scalarValue);
    void setNum1DData(int num1DData);
    void setData1D(QVariantMap data1D);

private:
    bool m_isVector = false;
    double m_time = 0;
    QString m_group = "all";
    int m_frequency = 2;
    bool m_hasScalarData = false;
    float m_scalarValue = 0.0;
    int m_num1DData = 0;
    QVariantMap m_data1D;
    QMap<QString, CP1DData*> m_data1DRaw;
};

#endif // COMPUTE_H
