#include "fixaveragetime.h"
#include "lammpscontroller.h"
#include <fix_ave_time.h>
FixAverageTime::FixAverageTime()
{

}

void FixAverageTime::updateCommand()
{

}

QList<QString> FixAverageTime::enabledCommands()
{
    return {QString("fix %1 %2").arg(identifier()).arg(command())};
}

QList<QString> FixAverageTime::disableCommands()
{
    return {QString("unfix %1 ").arg(identifier())};
}

QList<QString> FixAverageTime::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), QString("fix %1 %2").arg(identifier()).arg(command()) };
}

bool FixAverageTime::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findFixIndex(identifier())>=0;
}

int FixAverageTime::NEvery() const
{
    return m_NEvery;
}

int FixAverageTime::NRepeat() const
{
    return m_NRepeat;
}

int FixAverageTime::NFreq() const
{
    return m_NFreq;
}

void FixAverageTime::setNEvery(int NEvery)
{
    if (m_NEvery == NEvery)
        return;

    m_NEvery = NEvery;
    emit NEveryChanged(NEvery);
}

void FixAverageTime::setNRepeat(int NRepeat)
{
    if (m_NRepeat == NRepeat)
        return;

    m_NRepeat = NRepeat;
    emit NRepeatChanged(NRepeat);
}

void FixAverageTime::setNFreq(int NFreq)
{
    if (m_NFreq == NFreq)
        return;

    m_NFreq = NFreq;
    emit NFreqChanged(NFreq);
}
