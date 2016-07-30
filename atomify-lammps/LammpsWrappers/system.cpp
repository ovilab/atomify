#include "system.h"
#include <domain.h>
#include <atom.h>
#include <update.h>
#include "atoms.h"
#include "regions.h"
#include "groups.h"
#include "computes.h"
#include "../mysimulator.h"
#include "modifiers/modifier.h"

using namespace LAMMPS_NS;

System::System(AtomifySimulator *simulator)
{
    setAtoms(new Atoms(simulator));
    setGroups(new Groups(simulator));
    setRegions(new Regions(simulator));
    setComputes(new Computes(simulator));
}

void System::synchronize(LAMMPSController *lammpsController)
{
    m_regions->synchronize(lammpsController);
    m_groups->synchronize(lammpsController);
    m_atoms->synchronize(lammpsController);
    m_computes->synchronize(lammpsController);

    LAMMPS *lammps = lammpsController->lammps();
    if(!lammps) {
        setIsValid(false);
        return;
    }
    setIsValid(true);

    Domain *domain = lammps->domain;
    Atom *atom = lammps->atom;
    Update *update = lammps->update;

    if(!domain || !atom || !update) return; // These may not be set in LAMMPS (they probably are, but an easy test).

    bool originDidChange = false;
    bool sizeDidChange = false;
    for(int i=0; i<3; i++) {
        if(m_origin[i] != domain->boxlo[i]) {
            m_origin[i] = domain->boxlo[i];
            originDidChange  = true;
        }
        if(m_size[i] != domain->prd[i]) {
            m_size[i] = domain->prd[i];
            sizeDidChange = true;
        }
    }

    if(originDidChange) {
        emit originChanged(m_origin);
        emit geometryChanged();
    }

    if(sizeDidChange) {
        emit sizeChanged(m_size);
        emit geometryChanged();
    }

    if(m_numberOfAtoms != atom->natoms) {
        m_numberOfAtoms = atom->natoms;
        emit numberOfAtomsChanged(m_numberOfAtoms);
    }

    if(m_numberOfAtomTypes != atom->ntypes) {
        m_numberOfAtomTypes = atom->ntypes;
        emit numberOfAtomTypesChanged(m_numberOfAtomTypes);
    }

    if(m_simulationTime != update->atime) {
        m_simulationTime = update->atime;
        emit simulationTimeChanged(m_simulationTime);
    }

    if(m_timesteps != update->atimestep) {
        m_timesteps = update->atimestep;
        emit timestepsChanged(m_timesteps);
    }

    for(QVariant modifier_ : m_atoms->modifiers()) {
        Modifier *modifier = modifier_.value<Modifier*>();
        modifier->setSystem(this);
    }
    m_volume = m_size[0]*m_size[1]*m_size[2];
    emit volumeChanged(m_volume);
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

int System::timesteps() const
{
    return m_timesteps;
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
    m_groups->reset();
    m_computes->reset();
    m_regions->reset();
    m_timesteps = 0;
    m_simulationTime = 0;
    m_size = QVector3D();
    m_origin = QVector3D();
    m_numberOfAtoms = 0;
    m_numberOfAtomTypes = 0;
    emit timestepsChanged(m_timesteps);
    emit simulationTimeChanged(m_simulationTime);
    emit sizeChanged(m_size);
    emit originChanged(m_origin);
    emit numberOfAtomsChanged(m_numberOfAtoms);
    emit numberOfAtomTypesChanged(m_numberOfAtomTypes);
    m_atoms->reset();
}

float System::volume() const
{
    return m_volume;
}

bool System::isValid() const
{
    return m_isValid;
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
