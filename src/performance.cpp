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

void Performance::reset()
{
    setThreads(1);
    setMemoryAtomify(0);
    setMemoryLAMMPS(0);
    setTimestepsPerSecond(0);
    setAtomTimestepsPerSecond(0);
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

double Performance::timestepsPerSecond() const
{
    return m_timestepsPerSecond;
}

int Performance::threads() const
{
    return m_threads;
}

double Performance::atomTimestepsPerSecond() const
{
  return m_atomTimestepsPerSecond;
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

void Performance::setTimestepsPerSecond(double timestepsPerSecond)
{
    if (m_timestepsPerSecond == timestepsPerSecond)
        return;

    m_timestepsPerSecond = timestepsPerSecond;
    emit timestepsPerSecondChanged(timestepsPerSecond);
}

void Performance::setThreads(int threads)
{
    if (m_threads == threads)
        return;

    m_threads = threads;
    emit threadsChanged(m_threads);
}

void Performance::setAtomTimestepsPerSecond(double atomTimestepsPerSecond)
{
  if (qFuzzyCompare(m_atomTimestepsPerSecond, atomTimestepsPerSecond))
    return;

  m_atomTimestepsPerSecond = atomTimestepsPerSecond;
  emit atomTimestepsPerSecondChanged(m_atomTimestepsPerSecond);
}
