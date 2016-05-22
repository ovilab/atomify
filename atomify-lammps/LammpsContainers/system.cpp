#include "system.h"
#include <domain.h>
#include <atom.h>

using namespace LAMMPS_NS;

System::System()
{

}

void System::synchronize(LAMMPS *lammps)
{
    Domain *domain = lammps->domain;
    Atom *atom = lammps->atom;
    if(!domain || !atom) return; // These may not be set in LAMMPS (they probably are, but an easy test).

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
    if(m_numberOfAtoms != lammps->atom->natoms) {
        m_numberOfAtoms = lammps->atom->natoms;
        emit numberOfAtomsChanged(m_numberOfAtoms);
    }
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
