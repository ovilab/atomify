#ifndef COMPUTE_H
#define COMPUTE_H
#include <vector>
#include "simulatorcontrol.h"
#include "datasource.h"
#include "../../dataproviders/dataprovider.h"
#include <QVariantMap>
#include <mpi.h>
#include <compute.h>
#include <style_compute.h> // Includes all computes
using namespace LAMMPS_NS;

class CPCompute : public SimulatorControl
{
    Q_OBJECT
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(bool isVector READ isVector WRITE setIsVector NOTIFY isVectorChanged)
    Q_PROPERTY(bool isPerAtom READ isPerAtom WRITE setIsPerAtom NOTIFY isPerAtomChanged)
    Q_PROPERTY(int perAtomIndex READ perAtomIndex WRITE setPerAtomIndex NOTIFY perAtomIndexChanged)
    Q_PROPERTY(int numPerAtomValues READ numPerAtomValues WRITE setNumPerAtomValues NOTIFY numPerAtomValuesChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(bool hasScalarData READ hasScalarData WRITE setHasScalarData NOTIFY hasScalarDataChanged)
    Q_PROPERTY(float scalarValue READ scalarValue WRITE setScalarValue NOTIFY scalarValueChanged)
    Q_PROPERTY(QString xLabel READ xLabel WRITE setXLabel NOTIFY xLabelChanged)
    Q_PROPERTY(QString yLabel READ yLabel WRITE setYLabel NOTIFY yLabelChanged)

public:
    CPCompute(Qt3DCore::QNode *parent = 0);
    ~CPCompute();
    Q_INVOKABLE void clear();
    Q_INVOKABLE void exportToTextFile(QString fileName);
    Q_INVOKABLE void exportToPythonFile(QString fileName);
    Q_INVOKABLE void exportToMatlabFile(QString fileName);
    void copyData(LAMMPSController *lammpsController);
    bool existsInLammps(LAMMPSController *lammpsController) override;
    void computeInLAMMPS(LAMMPSController *lammpsController);
    bool isVector() const;
    QString group() const;
    int frequency() const;
    bool hasScalarData() const;
    float scalarValue() const;
    QString xLabel() const;
    QString yLabel() const;
    bool interactive() const;
    bool isPerAtom() const;
    bool hovered() const;
    void updateData1D();
    const std::vector<double> &atomData() const;
    int perAtomIndex() const;
    int numPerAtomValues() const;
    int groupBit() const;
signals:
    void isVectorChanged(bool isVector);
    void groupChanged(QString group);
    void frequencyChanged(int frequency);
    void hasScalarDataChanged(bool hasScalarData);
    void scalarValueChanged(float scalarValue);
    void xLabelChanged(QString xLabel);
    void yLabelChanged(QString yLabel);
    void interactiveChanged(bool interactive);
    void isPerAtomChanged(bool isPerAtom);
    void hoveredChanged(bool hovered);
    void perAtomIndexChanged(int perAtomIndex);
    void numPerAtomValuesChanged(int numPerAtomValues);

public slots:
    void setIsVector(bool isVector);
    void setGroup(QString group);
    void setFrequency(int frequency);
    void setHasScalarData(bool hasScalarData);
    void setScalarValue(float scalarValue);
    void setXLabel(QString xLabel);
    void setYLabel(QString yLabel);
    void setInteractive(bool interactive);
    void setIsPerAtom(bool isPerAtom);
    void setHovered(bool hovered);
    void setPerAtomIndex(int perAtomIndex);
    void setNumPerAtomValues(int numPerAtomValues);

private:
    bool copyData(ComputePressure *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeTemp *compute, LAMMPSController *lammpsController);
    bool copyData(ComputePE *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeKE *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeRDF *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeMSD *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeVACF *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeCOM *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeGyration *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeKEAtom *compute, LAMMPSController *lammpsController);
    bool copyData(ComputePropertyAtom *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeClusterAtom *compute, LAMMPSController *lammpsController);
    bool copyData(ComputeCNAAtom *compute, LAMMPSController *lammpsController);
    bool copyData(Compute *compute, LAMMPSController *lammpsController);

    bool m_isVector = false;
    double m_time = 0;
    QString m_group = "all";
    int m_frequency = 10;
    bool m_hasScalarData = false;
    float m_scalarValue = 0.0;
    std::vector<double> m_atomData;
    QString m_xLabel;
    QString m_yLabel;
    bool m_interactive = false;
    bool m_isPerAtom = false;
    bool m_hovered = false;
    int m_perAtomIndex = 0;
    int m_numPerAtomValues = 0;
    int m_groupBit = 0;
    bool validateStatus(Compute *compute, LAMMPS *lammps);
protected:
    virtual void updateCommand() override;
    QList<QString> enabledCommands() override;
    QList<QString> disableCommands() override;
    virtual QList<QString> resetCommands() override;
    QString createCommandPrefix() override;
    long m_lastUpdate = -1;
};

#endif // COMPUTE_H
