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

public:
    CPCompute(QObject *parent = 0);
    ~CPCompute();
    void copyData(LAMMPSController *lammpsController);
    virtual bool existsInLammps(LAMMPSController *lammpsController) override;
    void computeInLAMMPS(LAMMPSController *lammpsController);

signals:

public slots:

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
    bool validateStatus(Compute *compute, LAMMPS *lammps);
};

#endif // COMPUTE_H
