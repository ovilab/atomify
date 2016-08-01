#ifndef FIXHISTOGRAM_H
#define FIXHISTOGRAM_H
#include "cpcompute.h"
#include "datasource.h"
class FixHistogram : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(int nEvery READ nEvery WRITE setNEvery NOTIFY nEveryChanged)
    Q_PROPERTY(int nRepeat READ nRepeat WRITE setNRepeat NOTIFY nRepeatChanged)
    Q_PROPERTY(int nFreq READ nFreq WRITE setNFreq NOTIFY nFreqChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(float max READ max WRITE setMax NOTIFY maxChanged)
    Q_PROPERTY(float min READ min WRITE setMin NOTIFY minChanged)
    Q_PROPERTY(int numberOfBins READ numberOfBins WRITE setNumberOfBins NOTIFY numberOfBinsChanged)
    Q_PROPERTY(CPCompute* compute READ compute WRITE setCompute NOTIFY computeChanged)
    Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY valuesChanged)
    Q_PROPERTY(DataSource* dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
private:
    DataSource* m_dataSource = nullptr;
    CPCompute* m_compute = nullptr;
    QVariantList m_values;
    int m_nEvery = 1;
    int m_nRepeat = 10;
    int m_nFreq = 10;
    float m_max = 0;
    float m_min = 10;
    int m_numberOfBins = 100;
    QString m_group = "all";

public:
    FixHistogram(Qt3DCore::QNode *parent = 0);
    virtual bool existsInLammps(LAMMPSController *lammpsController) override;
    virtual void update(LAMMPSController *lammpsController) override;
    int nEvery() const;
    int nRepeat() const;
    int nFreq() const;
    CPCompute* compute() const;
    float max() const;
    float min() const;
    int numberOfBins() const;
    QVariantList values() const;
    DataSource* dataSource() const;
    QString group() const;

protected:
    virtual void updateCommand() override;
    virtual QList<QString> enabledCommands() override;
    virtual QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;
    QString createCommandPrefix();
public slots:
    void setNEvery(int nEvery);
    void setNRepeat(int nRepeat);
    void setNFreq(int nFreq);
    void setCompute(CPCompute* compute);
    void setMax(float max);
    void setMin(float min);
    void setNumberOfBins(int numberOfBins);
    void setValues(QVariantList values);
    void setDataSource(DataSource* dataSource);
    void setGroup(QString group);

signals:
    void nEveryChanged(int nEvery);
    void nRepeatChanged(int nRepeat);
    void nFreqChanged(int nFreq);
    void computeChanged(CPCompute* compute);
    void maxChanged(float max);
    void minChanged(float min);
    void numberOfBinsChanged(int numberOfBins);
    void valuesChanged(QVariantList values);
    void dataSourceChanged(DataSource* dataSource);
    void groupChanged(QString group);
};

#endif // FIXHISTOGRAM_H
