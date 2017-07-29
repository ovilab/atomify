#include "fixhistogram.h"
#include "../lammpscontroller.h"
#include <fix_ave_histo.h>

FixHistogram::FixHistogram()
{

}


void FixHistogram::updateCommand()
{
    setCommand(QString("fix %1 all ave/histo %2 %3 %4 %5 %6 %7 c_%8 file /projects/%1.txt ave running").arg(identifier()).arg(m_nEvery).arg(m_nRepeat).arg(m_nFreq).arg(m_min).arg(m_max).arg(m_numberOfBins).arg(m_compute->identifier()));
}

QList<QString> FixHistogram::enabledCommands()
{
    return {QString("fix %1 all ave/histo %2 %3 %4 %5 %6 %7 c_%8 file /projects/%1.txt ave running").arg(identifier()).arg(m_nEvery).arg(m_nRepeat).arg(m_nFreq).arg(m_min).arg(m_max).arg(m_numberOfBins).arg(m_compute->identifier())};
}

QList<QString> FixHistogram::disableCommands()
{
    return {QString("unfix %1 ").arg(identifier())};
}

QList<QString> FixHistogram::resetCommands()
{
    return { QString("unfix %1").arg(identifier()), QString("fix %1 all ave/histo %2 %3 %4 %5 %6 %7 c_%8 file /projects/%1.txt ave running").arg(identifier()).arg(m_nEvery).arg(m_nRepeat).arg(m_nFreq).arg(m_min).arg(m_max).arg(m_numberOfBins).arg(m_compute->identifier()) };
}

bool FixHistogram::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findFixIndex(identifier())>=0;
}

void FixHistogram::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);
    LAMMPS_NS::FixAveHisto *lmp_fix = static_cast<LAMMPS_NS::FixAveHisto *>(lammpsController->findFixByIdentifier(identifier()));

    if(lmp_fix != nullptr) {
        m_values.clear();
        m_values.reserve(m_numberOfBins);
        for(int n=0; n<m_numberOfBins; n++) {
            m_values.push_back(QVariant::fromValue<double>(lmp_fix->compute_array(n,2)));
        }

        if(m_dataSource) {
            QVector<QPointF> data;
            data.reserve(m_numberOfBins);
            float deltaX = (m_max - m_min) / m_numberOfBins;
            for(int n=0; n<m_numberOfBins; n++) {
                double x = m_min + n*deltaX;
                double y = m_values[n].value<double>();
                if( !(y!=y)) data.push_back(QPointF(x,y)); // y!=y is the test for isNaN
            }
            m_dataSource->setPoints(data);
        }

        emit valuesChanged(m_values);
    }
}

int FixHistogram::nEvery() const
{
    return m_nEvery;
}

int FixHistogram::nRepeat() const
{
    return m_nRepeat;
}

int FixHistogram::nFreq() const
{
    return m_nFreq;
}

CPCompute *FixHistogram::compute() const
{
    return m_compute;
}

float FixHistogram::max() const
{
    return m_max;
}

float FixHistogram::min() const
{
    return m_min;
}

int FixHistogram::numberOfBins() const
{
    return m_numberOfBins;
}

QVariantList FixHistogram::values() const
{
    return m_values;
}

DataSource *FixHistogram::dataSource() const
{
    return m_dataSource;
}

void FixHistogram::setNEvery(int nEvery)
{
    if (m_nEvery == nEvery)
        return;

    m_nEvery = nEvery;
    emit nEveryChanged(nEvery);
}

void FixHistogram::setNRepeat(int nRepeat)
{
    if (m_nRepeat == nRepeat)
        return;

    m_nRepeat = nRepeat;
    emit nRepeatChanged(nRepeat);
}

void FixHistogram::setNFreq(int nFreq)
{
    if (m_nFreq == nFreq)
        return;

    m_nFreq = nFreq;
    emit nFreqChanged(nFreq);
}

void FixHistogram::setCompute(CPCompute *compute)
{
    if (m_compute == compute)
        return;

    removeDependency(m_compute);
    m_compute = compute;
    addDependency(compute);

    emit computeChanged(compute);
}

void FixHistogram::setMax(float max)
{
    if (m_max == max)
        return;

    m_max = max;
    emit maxChanged(max);
}

void FixHistogram::setMin(float min)
{
    if (m_min == min)
        return;

    m_min = min;
    emit minChanged(min);
}

void FixHistogram::setNumberOfBins(int numberOfBins)
{
    if (m_numberOfBins == numberOfBins)
        return;

    m_numberOfBins = numberOfBins;
    emit numberOfBinsChanged(numberOfBins);
}

void FixHistogram::setValues(QVariantList values)
{
    if (m_values == values)
        return;

    m_values = values;
    emit valuesChanged(values);
}

void FixHistogram::setDataSource(DataSource *dataSource)
{
    if (m_dataSource == dataSource)
        return;

    m_dataSource = dataSource;
    emit dataSourceChanged(dataSource);
}
