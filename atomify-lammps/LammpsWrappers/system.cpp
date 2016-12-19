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
