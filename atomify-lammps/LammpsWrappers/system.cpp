#include "system.h"
#include <domain.h>
#include <atom.h>
#include <update.h>
#include "../mysimulator.h"

using namespace LAMMPS_NS;

System::System(AtomifySimulator *simulator)
{
    setAtoms(new Atoms(simulator));
}

void System::synchronize(LAMMPS *lammps)
{
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

    if(m_simulationTime != update->atime) {
        m_simulationTime = update->atime;
        emit simulationTimeChanged(m_simulationTime);
    }

    if(m_timesteps != update->atimestep) {
        m_timesteps = update->atimestep;
        emit timestepsChanged(m_timesteps);
    }

    m_atoms->synchronize(lammps);
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

void System::setAtoms(Atoms *atoms)
{
    if (m_atoms == atoms)
        return;

    m_atoms = atoms;
    emit atomsChanged(atoms);
}
