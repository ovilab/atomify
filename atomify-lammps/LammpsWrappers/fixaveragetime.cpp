#include "fixaveragetime.h"
#include "lammpscontroller.h"
#include <fix_ave_time.h>

/* The Nevery, Nrepeat, and Nfreq arguments specify on what timesteps the input values will be used in order to contribute to the average. The final averaged quantities are generated on timesteps that are a mlutiple of Nfreq. The average is over Nrepeat quantities, computed in the preceding portion of the simulation every Nevery timesteps. Nfreq must be a multiple of Nevery and Nevery must be non-zero even if Nrepeat is 1. Also, the timesteps contributing to the average value cannot overlap, i.e. Nrepeat*Nevery can not exceed Nfreq.
 * */

FixAverageTime::FixAverageTime()
{

}

void FixAverageTime::updateCommand()
{
    setCommand(QString("fix %1 all ave/time %2 %3 %4 c_%5 file %6.txt").arg(identifier()).arg(m_nEvery).arg(m_nRepeat).arg(m_nFreq).arg(m_compute->identifier(), identifier()));
}

QList<QString> FixAverageTime::enabledCommands()
{
    return {QString("fix %1 all ave/time %2 %3 %4 c_%5 file %6.txt").arg(identifier()).arg(m_nEvery).arg(m_nRepeat).arg(m_nFreq).arg(m_compute->identifier(), identifier())};
}

QList<QString> FixAverageTime::disableCommands()
{
    return {QString("unfix %1 ").arg(identifier())};
}

QList<QString> FixAverageTime::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), QString("fix %1 all ave/time %2 %3 %4 c_%5 file %6.txt").arg(identifier()).arg(m_nEvery).arg(m_nRepeat).arg(m_nFreq).arg(m_compute->identifier(), identifier()) };
}

bool FixAverageTime::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findFixIndex(identifier())>=0;
}

int FixAverageTime::nEvery() const
{
    return m_nEvery;
}

int FixAverageTime::nRepeat() const
{
    return m_nRepeat;
}

int FixAverageTime::nFreq() const
{
    return m_nFreq;
}

CPCompute *FixAverageTime::compute() const
{
    return m_compute;
}

void FixAverageTime::setNEvery(int nEvery)
{
    if (m_nEvery == nEvery)
        return;

    m_nEvery = nEvery;
    emit nEveryChanged(nEvery);
}

void FixAverageTime::setNRepeat(int nRepeat)
{
    if (m_nRepeat == nRepeat)
        return;

    m_nRepeat = nRepeat;
    emit nRepeatChanged(nRepeat);
}

void FixAverageTime::setNFreq(int nFreq)
{
    if (m_nFreq == nFreq)
        return;

    m_nFreq = nFreq;
    emit nFreqChanged(nFreq);
}

void FixAverageTime::setCompute(CPCompute *compute)
{
    if (m_compute == compute)
        return;

    removeDependency(m_compute);
    m_compute = compute;
    addDependency(compute);

    emit computeChanged(compute);
}

void FixAverageTime::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);
    LAMMPS_NS::FixAveTime *lmp_fix = static_cast<LAMMPS_NS::FixAveTime *>(lammpsController->findFixByIdentifier(identifier()));

    if(lmp_fix != nullptr) {
//       QVector<double> newValues;

//       if(isVector()) {
//           qDebug() << "FixAverageTime::update: Fix ave/time vector mode is not supported in Atomify yet";
//       } else {
//           double value = lmp_fix->compute_scalar();
//           newValues.push_back(value);
//       }

//       setValues(lammpsController->simulationTime(), newValues);
    }
}
