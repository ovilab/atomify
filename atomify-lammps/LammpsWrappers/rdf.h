#ifndef RDF_H
#define RDF_H

#include "CPCompute.h"
#include "QMLPlot/linegraph.h"
#include <QMap>
class RDF : public CPCompute
{
    Q_OBJECT
    Q_PROPERTY(QVariantList atomPairs READ atomPairs WRITE setAtomPairs NOTIFY atomPairsChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(int numberOfBins READ numberOfBins WRITE setNumberOfBins NOTIFY numberOfBinsChanged)
    Q_PROPERTY(LineGraphDataSource* dataSource1 READ dataSource1 WRITE setDataSource1 NOTIFY dataSource1Changed)
    Q_PROPERTY(LineGraphDataSource* dataSource2 READ dataSource2 WRITE setDataSource2 NOTIFY dataSource2Changed)
    Q_PROPERTY(LineGraphDataSource* dataSource3 READ dataSource3 WRITE setDataSource3 NOTIFY dataSource3Changed)
    Q_PROPERTY(LineGraphDataSource* dataSource4 READ dataSource4 WRITE setDataSource4 NOTIFY dataSource4Changed)

public:
    RDF();

protected:
    LineGraphDataSource* m_dataSource1 = nullptr;
    LineGraphDataSource* m_dataSource2 = nullptr;
    LineGraphDataSource* m_dataSource3 = nullptr;
    LineGraphDataSource* m_dataSource4 = nullptr;
    QVariantList m_atomPairs;
    QString m_group = "all";
    QString generatePairString();
    int m_numberOfBins = 100;
    int numberOfPairs();
    virtual void updateCommand() override;
    virtual QList<QString> enabledCommands() override;
    virtual QList<QString> resetCommands() override;

public:
    virtual void update(LAMMPSController *lammpsController) override;
    QVariantList atomPairs() const;
    QString group() const;
    int numberOfBins() const;

    LineGraphDataSource* dataSource1() const;
    LineGraphDataSource* dataSource2() const;
    LineGraphDataSource* dataSource3() const;
    LineGraphDataSource* dataSource4() const;

public slots:
    void setAtomPairs(QVariantList atomPairs);
    void setGroup(QString group);
    void setNumberOfBins(int numberOfBins);
    void setDataSource1(LineGraphDataSource* dataSource1);
    void setDataSource2(LineGraphDataSource* dataSource2);
    void setDataSource3(LineGraphDataSource* dataSource3);
    void setDataSource4(LineGraphDataSource* dataSource4);

signals:
    void atomPairsChanged(QVariantList atomPairs);
    void groupChanged(QString group);
    void numberOfBinsChanged(int numberOfBins);
    void dataSource1Changed(LineGraphDataSource* dataSource1);
    void dataSource2Changed(LineGraphDataSource* dataSource2);
    void dataSource3Changed(LineGraphDataSource* dataSource3);
    void dataSource4Changed(LineGraphDataSource* dataSource4);

    // SimulatorControl interface
protected:
    virtual QList<QString> disableCommands();
};

#endif // RDF_H
