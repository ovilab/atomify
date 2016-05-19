#include "atoms.h"
#include <atom.h>
#include <domain.h>
#include "modifiers/modifiers.h"
using namespace LAMMPS_NS;

Atoms::Atoms()
{
    m_sphereData = new SphereData();
    m_cylinderData = new CylinderData();
}

void Atoms::synchronize(LAMMPS *lammps)
{
    if(!lammps) return;
    Atom *atom = lammps->atom;
    Domain *domain = lammps->domain;
    int *types = lammps->atom->type;

    int numberOfAtoms = atom->natoms;
    if(m_atomData.positions.size() != numberOfAtoms) {
        m_atomData.positions.resize(numberOfAtoms);
    }
    if(m_atomData.atomTypes.size() != numberOfAtoms) {
        m_atomData.atomTypes.resize(numberOfAtoms);
    }

    for(int i=0; i<numberOfAtoms; i++) {
        m_atomData.atomTypes[i] = types[i];

        double position[3];
        position[0] = atom->x[i][0];
        position[1] = atom->x[i][1];
        position[2] = atom->x[i][2];
        domain->remap(position); // remap into system boundaries with PBC
        m_atomData.positions[i][0] = position[0];
        m_atomData.positions[i][1] = position[1];
        m_atomData.positions[i][2] = position[2];
    }
}

void Atoms::updateData()
{
    AtomData atomData = m_atomData;
    // TODO: Sort modifiers on priority first
    for(Modifier *modifier : m_modifiers) {
         modifier->apply(atomData);
    }

    generateSphereData(atomData);
}

void Atoms::generateSphereData(AtomData &atomData) {

    m_sphereData->setData(atomData.positions, atomData.colors, atomData.radii);
}

void Atoms::generateCylinderData(AtomData &atomData) {

}

SphereData *Atoms::sphereData() const
{
    return m_sphereData;
}

CylinderData *Atoms::cylinderData() const
{
    return m_cylinderData;
}

void Atoms::setSphereData(SphereData *sphereData)
{
    if (m_sphereData == sphereData)
        return;

    m_sphereData = sphereData;
    emit sphereDataChanged(sphereData);
}

void Atoms::setCylinderData(CylinderData *cylinderData)
{
    if (m_cylinderData == cylinderData)
        return;

    m_cylinderData = cylinderData;
    emit cylinderDataChanged(cylinderData);
}
