#include "system.h"
#include <domain.h>
#include <atom.h>
#include <update.h>
#include "atoms.h"
#include "regions.h"
#include "groups.h"
#include "computes.h"
#include "fixes.h"
#include "variables.h"
#include "../mysimulator.h"
#include "modifiers/modifier.h"
#include "units.h"

using namespace LAMMPS_NS;

System::System(AtomifySimulator *simulator)
{
    setAtoms(new Atoms(simulator));
    setGroups(new Groups(simulator));
    setRegions(new Regions(simulator));
    setComputes(new Computes(simulator));
    setUnits(new Units(simulator));
    setFixes(new Fixes(simulator));
    setVariables(new Variables(simulator));
}

void System::updateCorners(Domain *domain)
{
//    corners[0][0] = lo[0]; corners[0][1] = lo[1]; corners[0][2] = lo[2];
//    lamda2x(corners[0],corners[0]);
//    corners[1][0] = hi[0]; corners[1][1] = lo[1]; corners[1][2] = lo[2];
//    lamda2x(corners[1],corners[1]);
//    corners[2][0] = lo[0]; corners[2][1] = hi[1]; corners[2][2] = lo[2];
//    lamda2x(corners[2],corners[2]);
//    corners[3][0] = hi[0]; corners[3][1] = hi[1]; corners[3][2] = lo[2];
//    lamda2x(corners[3],corners[3]);
//    corners[4][0] = lo[0]; corners[4][1] = lo[1]; corners[4][2] = hi[2];
//    lamda2x(corners[4],corners[4]);
//    corners[5][0] = hi[0]; corners[5][1] = lo[1]; corners[5][2] = hi[2];
//    lamda2x(corners[5],corners[5]);
//    corners[6][0] = lo[0]; corners[6][1] = hi[1]; corners[6][2] = hi[2];
//    lamda2x(corners[6],corners[6]);
//    corners[7][0] = hi[0]; corners[7][1] = hi[1]; corners[7][2] = subhi_lamda[2];
//    lamda2x(corners[7],corners[7]);


//    qDebug() << "lo = " << domain->boxlo_lamda[0] << ", " << domain->boxlo_lamda[1] << ", " << domain->boxlo_lamda[2];
//    qDebug() << "hi = " << domain->boxhi_lamda[0] << ", " << domain->boxhi_lamda[1] << ", " << domain->boxhi_lamda[2];
//    double lo[3];
//    lo[0] = domain->boxlo_lamda[0];
//    lo[1] = domain->boxlo_lamda[1];
//    lo[2] = domain->boxlo_lamda[2];

//    double hi[3];
//    hi[0] = domain->boxhi_lamda[0];
//    hi[1] = domain->boxhi_lamda[1];
//    hi[2] = domain->boxhi_lamda[2];

//    domain->lamda2x(lo, lo);
//    domain->lamda2x(hi, hi);

//    qDebug() << "lo1 = " << lo[0] << ", " << lo[1] << ", " << lo[2];
//    qDebug() << "hi1 = " << hi[0] << ", " << hi[1] << ", " << hi[2];

    double *h = domain->h;
    float transformationMatrixValues[] = {
        h[0], h[5], h[4],
        0,    h[1], h[3],
        0,    0,    h[2]
    };
    m_transformationMatrix = QMatrix3x3(transformationMatrixValues);
    emit transformationMatrixChanged(m_transformationMatrix);

//    domain->box_corners();
//    m_corners.resize(8);
//    bool cornersDidChanged = false;
//    for(int i=0; i<8; i++) {
//        for(int a=0; a<3; a++) {
//            cornersDidChanged = cornersDidChanged || (fabs(m_corners[i][a]-domain->corners[i][a]) > 1e-5);
//            m_corners[i][a] = domain->corners[i][a];
//        }
//    }
//    if(cornersDidChanged) emit cornersChanged(m_corners);
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

void System::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    if(!lammps) {
        reset();
        return;
    }
    setIsValid(true);

    m_regions->synchronize(lammpsController);
    m_groups->synchronize(lammpsController);
    m_atoms->synchronize(lammpsController);
    m_computes->synchronize(lammpsController);
    m_variables->synchronize(lammpsController);
    m_fixes->synchronize(lammpsController);

    Domain *domain = lammps->domain;
    Atom *atom = lammps->atom;
    Update *update = lammps->update;
    if(!domain || !atom || !update) return; // These may not be set in LAMMPS (they probably are, but an easy test).

    updateCorners(domain);
    updateSizeAndOrigin(domain);

    if(m_numberOfAtoms != atom->natoms) {
        m_numberOfAtoms = atom->natoms;
        emit numberOfAtomsChanged(m_numberOfAtoms);
    }

    if(m_numberOfAtomTypes != atom->ntypes) {
        m_numberOfAtomTypes = atom->ntypes;
        emit numberOfAtomTypesChanged(m_numberOfAtomTypes);
    }

    float currentTime = update->atime + update->dt*(update->ntimestep - update->atimestep);
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
    return m_atoms;
}

Regions *System::regions() const
{
    return m_regions;
}

Groups *System::groups() const
{
    return m_groups;
}

int System::numberOfAtomTypes() const
{
    return m_numberOfAtomTypes;
}

void System::reset()
{
    setIsValid(false);
    m_fixes->reset();
    m_atoms->reset();
    m_groups->reset();
    m_computes->reset();
    m_regions->reset();
    m_variables->reset();
    m_currentTimestep = 0;
    m_simulationTime = 0;
    m_size = QVector3D();
    m_origin = QVector3D();
    m_numberOfAtoms = 0;
    m_numberOfAtomTypes = 0;
    emit currentTimestepChanged(m_currentTimestep);
    emit simulationTimeChanged(m_simulationTime);
    emit sizeChanged(m_size);
    emit originChanged(m_origin);
    emit numberOfAtomsChanged(m_numberOfAtoms);
    emit numberOfAtomTypesChanged(m_numberOfAtomTypes);
}

float System::volume() const
{
    return m_volume;
}

bool System::isValid() const
{
    return m_isValid;
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
}

QVector3D System::cameraPosition() const
{
    return m_cameraPosition;
}

QVector3D System::center() const
{
    return m_origin+0.5*m_size;
}

Variables *System::variables() const
{
    return m_variables;
}

Units *System::units() const
{
    return m_units;
}

Fixes *System::fixes() const
{
    return m_fixes;
}

Computes *System::computes() const
{
    return m_computes;
}

void System::setAtoms(Atoms *atoms)
{
    if (m_atoms == atoms)
        return;

    m_atoms = atoms;
    emit atomsChanged(atoms);
}

void System::setRegions(Regions *regions)
{
    if (m_regions == regions)
        return;

    m_regions = regions;
    emit regionsChanged(regions);
}

void System::setGroups(Groups *groups)
{
    if (m_groups == groups)
        return;

    m_groups = groups;
    emit groupsChanged(groups);
}

void System::setIsValid(bool isValid)
{
    if (m_isValid == isValid)
        return;

    m_isValid = isValid;
    emit isValidChanged(isValid);
}

void System::setComputes(Computes *computes)
{
    if (m_computes == computes)
        return;

    m_computes = computes;
    emit computesChanged(computes);
}

void System::setCameraPosition(QVector3D cameraPosition)
{
    if (m_cameraPosition == cameraPosition)
        return;

    m_cameraPosition = cameraPosition;
    emit cameraPositionChanged(cameraPosition);
}

void System::setUnits(Units *units)
{
    if (m_units == units)
        return;

    m_units = units;
    emit unitsChanged(units);
}

void System::setFixes(Fixes *fixes)
{
    if (m_fixes == fixes)
        return;

    m_fixes = fixes;
    emit fixesChanged(fixes);
}

void System::setVariables(Variables *variables)
{
    if (m_variables == variables)
        return;

    m_variables = variables;
    emit variablesChanged(variables);
}
