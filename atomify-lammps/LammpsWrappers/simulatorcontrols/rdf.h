#ifndef RDF_H
#define RDF_H

#include "cpcompute.h"
#include <QMap>
class RDF : public CPCompute
{
    Q_OBJECT
    Q_PROPERTY(QVariantList atomPairs READ atomPairs WRITE setAtomPairs NOTIFY atomPairsChanged)
    Q_PROPERTY(int numberOfBins READ numberOfBins WRITE setNumberOfBins NOTIFY numberOfBinsChanged)
    Q_PROPERTY(DataSource* dataSource1 READ dataSource1 WRITE setDataSource1 NOTIFY dataSource1Changed)
    Q_PROPERTY(DataSource* dataSource2 READ dataSource2 WRITE setDataSource2 NOTIFY dataSource2Changed)
    Q_PROPERTY(DataSource* dataSource3 READ dataSource3 WRITE setDataSource3 NOTIFY dataSource3Changed)
    Q_PROPERTY(DataSource* dataSource4 READ dataSource4 WRITE setDataSource4 NOTIFY dataSource4Changed)

public:
    RDF(Qt3DCore::QNode *parent = nullptr);

protected:
    DataSource* m_dataSource1 = nullptr;
    DataSource* m_dataSource2 = nullptr;
    DataSource* m_dataSource3 = nullptr;
    DataSource* m_dataSource4 = nullptr;
    QVariantList m_atomPairs;
    QString generatePairString();
    int m_numberOfBins = 100;
    int numberOfPairs();
    virtual void updateCommand() override;
    virtual QList<QString> enabledCommands() override;
    virtual QList<QString> resetCommands() override;
public:
    virtual void update(LAMMPSController *lammpsController) override;
    QVariantList atomPairs() const;
    int numberOfBins() const;

    DataSource* dataSource1() const;
    DataSource* dataSource2() const;
    DataSource* dataSource3() const;
    DataSource* dataSource4() const;

public slots:
    void setAtomPairs(QVariantList atomPairs);
    void setNumberOfBins(int numberOfBins);
    void setDataSource1(DataSource* dataSource1);
    void setDataSource2(DataSource* dataSource2);
    void setDataSource3(DataSource* dataSource3);
    void setDataSource4(DataSource* dataSource4);

signals:
    void atomPairsChanged(QVariantList atomPairs);
    void numberOfBinsChanged(int numberOfBins);
    void dataSource1Changed(DataSource* dataSource1);
    void dataSource2Changed(DataSource* dataSource2);
    void dataSource3Changed(DataSource* dataSource3);
    void dataSource4Changed(DataSource* dataSource4);

    // SimulatorControl interface
protected:
    virtual QList<QString> disableCommands();
};

#endif // RDF_H
