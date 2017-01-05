#include "lammpscontroller.h"
#include "performance.h"
#include <atom.h>
#include <neighbor.h>
#include <comm.h>
#include <update.h>
#include <force.h>
#include <modify.h>
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/atoms.h"

using namespace LAMMPS_NS;
Performance::Performance(QObject *parent) : QObject(parent)
{

}

void Performance::synchronize(LAMMPSController *controller)
{
    LAMMPS *lammps = controller->lammps();
    bigint bytes = 0;
    bytes += lammps->atom->memory_usage();
    bytes += lammps->neighbor->memory_usage();
    bytes += lammps->comm->memory_usage();
    bytes += lammps->update->memory_usage();
    bytes += lammps->force->memory_usage();
    bytes += lammps->modify->memory_usage();
    setMemoryLAMMPS(bytes);
    setMemoryAtomify(controller->system->atoms()->memoryUsage());
}

long Performance::memoryLAMMPS() const
{
    return m_memoryLAMMPS;
}

long Performance::memoryAtomify() const
{
    return m_memoryAtomify;
}

double Performance::effectiveSimulationSpeed() const
{
    return m_effectiveSimulationSpeed;
}

void Performance::setMemoryLAMMPS(long memoryLAMMPS)
{
    if (m_memoryLAMMPS == memoryLAMMPS)
        return;

    m_memoryLAMMPS = memoryLAMMPS;
    emit memoryLAMMPSChanged(memoryLAMMPS);
}

void Performance::setMemoryAtomify(long memoryAtomify)
{
    if (m_memoryAtomify == memoryAtomify)
        return;

    m_memoryAtomify = memoryAtomify;
    emit memoryAtomifyChanged(memoryAtomify);
}

void Performance::setEffectiveSimulationSpeed(double effectiveSimulationSpeed)
{
    if (m_effectiveSimulationSpeed == effectiveSimulationSpeed)
        return;

    m_effectiveSimulationSpeed = effectiveSimulationSpeed;
    emit effectiveSimulationSpeedChanged(effectiveSimulationSpeed);
}
