#include "system.h"
#include <domain.h>
#include <atom.h>
#include <update.h>
#include "atoms.h"
#include "regions.h"
#include "groups.h"
#include "../mysimulator.h"
#include "modifiers/modifier.h"

using namespace LAMMPS_NS;

System::System(AtomifySimulator *simulator)
{
    setAtoms(new Atoms(simulator));
    setGroups(new Groups(simulator));
    setRegions(new Regions(simulator));
}

void System::synchronize(LAMMPS *lammps)
{
    if(!lammps) return;

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

    if(originDidChange) emit originChanged(m_origin);
    if(sizeDidChange) emit sizeChanged(m_size);

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

    m_groups->synchronize(lammps);
    m_atoms->synchronize(lammps);
    for(QVariant modifier_ : m_atoms->modifiers()) {
        Modifier *modifier = modifier_.value<Modifier*>();
        modifier->setSystem(this);
    }
//    QElapsedTimer t;
//    t.start();
//    m_atoms->atomData().neighborList.build(m_atoms->atomData().positions, this);
//    qDebug() << "Cell list built in " << t.elapsed() << " ms.";
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

void System::setNumberOfAtomTypes(int numberOfAtomTypes)
{
    if (m_numberOfAtomTypes == numberOfAtomTypes)
        return;

    m_numberOfAtomTypes = numberOfAtomTypes;
    emit numberOfAtomTypesChanged(numberOfAtomTypes);
}
