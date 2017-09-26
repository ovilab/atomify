#include "system.h"
#include "atoms.h"
#include "regions.h"
#include "groups.h"
#include "computes.h"
#include "fixes.h"
#include "variables.h"
#include "../mysimulator.h"
#include "modifiers/modifier.h"
#include "units.h"
#include "../performance.h"

#include <library.h>
#include <input.h>
#include <force.h>
#include <domain.h>
#include <atom.h>
#include <output.h>
#include <update.h>
#include <comm.h>
#include "LammpsWrappers/simulatorcontrols/simulatorcontrol.h"

using namespace LAMMPS_NS;

System::System(AtomifySimulator *simulator)
    : m_atoms(std::make_unique<Atoms>(simulator))
    , m_regions(std::make_unique<Regions>(simulator))
    , m_groups(std::make_unique<Groups>(simulator))
    , m_computes(std::make_unique<Computes>(simulator))
    , m_units(std::make_unique<Units>(simulator))
    , m_fixes(std::make_unique<Fixes>(simulator))
    , m_variables(std::make_unique<Variables>(simulator))
    , m_performance(std::make_unique<Performance>(simulator))
{
    m_transformationMatrix.setToIdentity();
}

System::~System()
{
}

void System::computeCellMatrix(Domain *domain) {
    domain->box_corners();
    QVector3D a(domain->corners[1][0], domain->corners[1][1], domain->corners[1][2]);
    QVector3D b(domain->corners[2][0], domain->corners[2][1], domain->corners[2][2]);
    QVector3D c(domain->corners[4][0], domain->corners[4][1], domain->corners[4][2]);
    QVector3D origo(domain->corners[0][0], domain->corners[0][1], domain->corners[0][2]);
    a *= m_atoms->globalScale();
    b *= m_atoms->globalScale();
    c *= m_atoms->globalScale();
    origo *= m_atoms->globalScale();

    a -= origo;
    b -= origo;
    c -= origo;
    float values[] = {
        a[0], b[0], c[0],
        a[1], b[1], c[1],
        a[2], b[2], c[2]
    };

    setCellMatrix(QMatrix3x3(values));
}

void System::updateCenter(Domain *domain)
{
    QVector3D origo(domain->corners[0][0], domain->corners[0][1], domain->corners[0][2]);
    QVector3D end(domain->corners[7][0], domain->corners[7][1], domain->corners[7][2]);
    QVector3D newCenter = origo + 0.5*(end-origo);
    if( !(newCenter - m_center).isNull()) {
        m_center = newCenter;
        emit centerChanged(m_center);
    }
}

void System::updateTransformationMatrix(Domain *domain)
{
    double *h = domain->h;
    float transformationMatrixValues[] = { // Must cast for clang
        float(h[0]*m_atoms->globalScale()), float(h[5]*m_atoms->globalScale()), float(h[4]*m_atoms->globalScale()), 0,
        0,    float(h[1]*m_atoms->globalScale()), float(h[3]*m_atoms->globalScale()), 0,
        0,    0,    float(h[2]*m_atoms->globalScale()), 0,
        0,    0,    0,    0
    };

    m_transformationMatrix = QMatrix4x4(transformationMatrixValues);
    emit transformationMatrixChanged(m_transformationMatrix);
}

void System::updateBoundaryStyle(Domain *domain)
{
    QString style;
    for (int idim = 0; idim < domain->dimension; idim++) {
        if(domain->boundary[idim][0] == 0) {
            style += "p ";
            continue;
        } else {
            // Loop over the two sides and append only one letter if they are the same, append both if they differ
            for (int iside = 0; iside < 2; iside++) {
                QString dimStyle = "";
                if(domain->boundary[idim][iside] == 1) dimStyle = "f";
                else if(domain->boundary[idim][iside] == 2) dimStyle = "s";
                else if(domain->boundary[idim][iside] == 3) dimStyle = "m";

                if(iside == 0) {
                    style += dimStyle;
                    continue;
                } else if(domain->boundary[idim][0] != domain->boundary[idim][1]) {
                    style += dimStyle;
                }
                style += " ";
            }
        }
    }
    setBoundaryStyle(style);
}

void System::updateSizeAndOrigin(Domain *domain)
{
    bool originDidChange = false;
    bool sizeDidChange = false;
    for(int i=0; i<3; i++) {
        if( fabs(m_origin[i] - domain->boxlo[i]) > 1e-4) {
            m_origin[i] = domain->boxlo[i];
            originDidChange  = true;
        }
        if( fabs(m_size[i] - domain->prd[i]) > 1e-4) {
            m_size[i] = domain->prd[i];
            sizeDidChange = true;
        }
    }

    if(originDidChange) {
        emit originChanged(m_origin);
        emit centerChanged(center());
    }

    if(sizeDidChange) {
        emit sizeChanged(m_size);
        emit centerChanged(center());
    }
}

void System::calculateCPURemain(LAMMPS *lammps)
{
    double value = lammps_get_thermo(lammps, "cpuremain");
    setCpuremain(value);
}

void System::calculateTimestepsPerSeconds(LAMMPS *lammps)
{
    if(m_currentTimestep % 10 == 0 && m_currentTimestep>0) {
        double value = lammps_get_thermo(lammps, "spcpu");
        if(value < 0) return;
        double oldValue = m_performance->timestepsPerSecond();
        value = 0.6*oldValue + 0.4*value; // low pass filter
        performance()->setTimestepsPerSecond(value);
    }
}

void System::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    if(!lammps) {
        reset();
        return;
    }
    setIsValid(true);

    if(lammps->input->lastLine()) {
        setLastCommand(QString::fromUtf8(lammps->input->lastLine()));
    }

    // 0 for unset, 1 for dynamics, 2 for min
    if(lammps->update->whichflag==0) {
        setState("");
    } else if(lammps->update->whichflag==1) {
        setState("Dynamics");
    } else if(lammps->update->whichflag==2) {
        setState("Minimizing");
    }

    Domain *domain = lammps->domain;
    Atom *atom = lammps->atom;
    Update *update = lammps->update;
    if(!domain || !atom || !update) return; // These may not be set in LAMMPS (they probably are, but an easy test).

    updateTransformationMatrix(domain);
    updateSizeAndOrigin(domain);
    computeCellMatrix(domain);
    updateBoundaryStyle(domain);
    setTriclinic(domain->triclinic);
    setPairStyle(QString(lammps->force->pair_style));
    m_performance->setThreads(lammps->comm->nthreads);
    updateCenter(domain);

    setNumberOfDangerousNeighborlistBuilds(lammps_get_thermo(lammps, "ndanger"));

    if(m_numberOfAtoms != atom->natoms) {
        m_numberOfAtoms = atom->natoms;
        emit numberOfAtomsChanged(m_numberOfAtoms);
    }


    if(m_numberOfAtomTypes != atom->ntypes) {
        m_numberOfAtomTypes = atom->ntypes;
        emit numberOfAtomTypesChanged(m_numberOfAtomTypes);
    }

    float currentTime = update->atime + update->dt*(update->ntimestep - update->atimestep);
    if(m_lastCommand.trimmed().startsWith("rerun")) {
        // Rerun command does have time, so we should probably use timestep value as time then?
        currentTime = update->ntimestep;
    }

    if(m_simulationTime != currentTime) {
        m_simulationTime = currentTime;
        emit simulationTimeChanged(m_simulationTime);
    }

    if(m_currentTimestep != update->ntimestep) {
        m_currentTimestep = update->ntimestep;
        emit currentTimestepChanged(m_currentTimestep);
    }

    for(QVariant modifier_ : m_atoms->modifiers()) {
        Modifier *modifier = modifier_.value<Modifier*>();
        modifier->setSystem(this);
    }
    m_volume = m_size[0]*m_size[1]*m_size[2];
    emit volumeChanged(m_volume);

    setDt(lammps->update->dt);
    setLammpsVersion(QString(LAMMPS_VERSION));

#ifdef MACAPPSTORE
    setMacAppStore(true);
#else
    setMacAppStore(false);
#endif

    calculateTimestepsPerSeconds(lammps);
    calculateCPURemain(lammps);

    m_regions->synchronize(lammpsController);
    m_groups->synchronize(lammpsController);
    m_atoms->synchronize(lammpsController);
    m_computes->synchronize(lammpsController);
    m_variables->synchronize(lammpsController);
    m_fixes->synchronize(lammpsController);
    m_performance->synchronize(lammpsController);
    m_units->synchronize(lammps);
}

QVector3D System::origin() const
{
    return m_origin;
}

QVector3D System::size() const
{
    return m_size;
}

int System::numberOfAtoms() const
{
    return m_numberOfAtoms;
}

float System::simulationTime() const
{
    return m_simulationTime;
}

int System::currentTimestep() const
{
    return m_currentTimestep;
}

Atoms *System::atoms() const
{
    return m_atoms.get();
}

Regions *System::regions() const
{
    return m_regions.get();
}

Groups *System::groups() const
{
    return m_groups.get();
}

Computes *System::computes() const
{
    return m_computes.get();
}

Units *System::units() const
{
    return m_units.get();
}

Fixes *System::fixes() const
{
    return m_fixes.get();
}

Variables *System::variables() const
{
    return m_variables.get();
}

int System::numberOfAtomTypes() const
{
    return m_numberOfAtomTypes;
}

void System::setCameraPosition(QVector3D cameraPosition)
{
    if (m_cameraPosition == cameraPosition)
        return;

    m_cameraPosition = cameraPosition;
    emit cameraPositionChanged(cameraPosition);
}

void System::reset()
{
    setIsValid(false);
    setTriclinic(false);
    m_fixes->reset();
    m_atoms->reset();
    m_groups->reset();
    m_computes->reset();
    m_regions->reset();
    m_variables->reset();
    m_units->reset();
    m_performance->reset();
    setDt(0);
    setCpuremain(0);
    setNumberOfDangerousNeighborlistBuilds(0);
    m_currentTimestep = 0;
    m_simulationTime = 0;
    m_size = QVector3D();
    m_origin = QVector3D();
    m_numberOfAtoms = 0;
    m_numberOfAtomTypes = 0;
    m_center = QVector3D();
    m_volume = 0;
    setBoundaryStyle("None");
    m_pairStyle = "";
    setLastCommand("");
    emit pairStyleChanged(m_pairStyle);
    emit currentTimestepChanged(m_currentTimestep);
    emit simulationTimeChanged(m_simulationTime);
    emit sizeChanged(m_size);
    emit centerChanged(m_center);
    emit originChanged(m_origin);
    emit numberOfAtomsChanged(m_numberOfAtoms);
    emit numberOfAtomTypesChanged(m_numberOfAtomTypes);
    emit volumeChanged(m_volume);
    m_performance->reset();
}

float System::volume() const
{
    return m_volume;
}

bool System::isValid() const
{
    return m_isValid;
}

QMatrix4x4 System::transformationMatrix() const
{
    return m_transformationMatrix;
}

QMatrix3x3 System::cellMatrix() const
{
    return m_cellMatrix;
}

QString System::boundaryStyle() const
{
    return m_boundaryStyle;
}

bool System::triclinic() const
{
    return m_triclinic;
}

Performance *System::performance() const
{
    return m_performance.get();
}

double System::cpuremain() const
{
    return m_cpuremain;
}

double System::dt() const
{
    return m_dt;
}

QString System::state() const
{
    return m_state;
}

QString System::lammpsVersion() const
{
    return m_lammpsVersion;
}

bool System::macAppStore() const
{
    return m_macAppStore;
}

QString System::pairStyle() const
{
    return m_pairStyle;
}

int System::numberOfTimesteps() const
{
    return m_numberOfTimesteps;
}

int System::numberOfDangerousNeighborlistBuilds() const
{
    return m_numberOfDangerousNeighborlistBuilds;
}

QString System::lastCommand() const
{
    return m_lastCommand;
}

QVector<SimulatorControl *> System::simulatorControls() const
{
    QVector<SimulatorControl*> controls = computes()->simulatorControls();
    controls.append(fixes()->simulatorControls());
    controls.append(variables()->simulatorControls());

    return controls;
}

void System::synchronizeQML(LAMMPSController *lammpsController)
{
    m_computes->synchronizeQML(lammpsController);
    m_variables->synchronizeQML(lammpsController);
    m_fixes->synchronizeQML(lammpsController);
    m_regions->synchronizeQML(lammpsController);
    m_groups->synchronizeQML(lammpsController);
}

void System::updateThreadOnDataObjects(QThread *thread)
{
    m_computes->updateThreadOnDataObjects(thread);
    m_fixes->updateThreadOnDataObjects(thread);
    m_variables->updateThreadOnDataObjects(thread);
}

QVector3D System::cameraPosition() const
{
    return m_cameraPosition;
}

QVector3D System::center() const
{
    return m_center;
}

void System::setIsValid(bool isValid)
{
    if (m_isValid == isValid)
        return;

    m_isValid = isValid;
    emit isValidChanged(isValid);
}

void System::setCellMatrix(QMatrix3x3 cellMatrix)
{
    if (m_cellMatrix == cellMatrix)
        return;

    m_cellMatrix = cellMatrix;
    emit cellMatrixChanged(cellMatrix);
}

void System::setBoundaryStyle(QString boundaryStyle)
{
    if (m_boundaryStyle == boundaryStyle)
        return;

    m_boundaryStyle = boundaryStyle;
    emit boundaryStyleChanged(boundaryStyle);
}

void System::setTriclinic(bool triclinic)
{
    if (m_triclinic == triclinic)
        return;

    m_triclinic = triclinic;
    emit triclinicChanged(triclinic);
}

void System::setCpuremain(double cpuremain)
{
    if (m_cpuremain == cpuremain)
        return;

    m_cpuremain = cpuremain;
    emit cpuremainChanged(cpuremain);
}

void System::setDt(double dt)
{
    if (m_dt == dt)
        return;

    m_dt = dt;
    emit dtChanged(dt);
}

void System::setState(QString state)
{
    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged(state);
}

void System::setLammpsVersion(QString lammpsVersion)
{
    if (m_lammpsVersion == lammpsVersion)
        return;

    m_lammpsVersion = lammpsVersion;
    emit lammpsVersionChanged(lammpsVersion);
}

void System::setMacAppStore(bool macAppStore)
{
    if (m_macAppStore == macAppStore)
        return;

    m_macAppStore = macAppStore;
    emit macAppStoreChanged(macAppStore);
}

void System::setPairStyle(QString pairStyle)
{
    if (m_pairStyle == pairStyle)
        return;

    m_pairStyle = pairStyle;
    emit pairStyleChanged(m_pairStyle);
}

void System::setNumberOfTimesteps(int numberOfTimesteps)
{
    if (m_numberOfTimesteps == numberOfTimesteps)
        return;

    m_numberOfTimesteps = numberOfTimesteps;
    emit numberOfTimestepsChanged(m_numberOfTimesteps);
}

void System::setNumberOfDangerousNeighborlistBuilds(int numberOfDangerousNeighborlistBuilds)
{
    if (m_numberOfDangerousNeighborlistBuilds == numberOfDangerousNeighborlistBuilds)
        return;

    m_numberOfDangerousNeighborlistBuilds = numberOfDangerousNeighborlistBuilds;
    emit numberOfDangerousNeighborlistBuildsChanged(m_numberOfDangerousNeighborlistBuilds);
}

void System::setLastCommand(QString lastCommand)
{
    if (m_lastCommand == lastCommand)
        return;

    m_lastCommand = lastCommand;
    emit lastCommandChanged(m_lastCommand);
}
