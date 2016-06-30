#include "cpcompute.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include "cpdata.h"
#include "../system.h"
#include <QDebug>
CPCompute::CPCompute(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{

}

CPCompute::~CPCompute() { }

void CPCompute::copyData(LAMMPSController *lammpsController)
{
    if(lammpsController->system()->timesteps() % m_frequency != 0) return;

    LAMMPS_NS::Compute *lmp_compute = lammpsController->findComputeByIdentifier(identifier());
    if(lmp_compute != nullptr) {
        if(lmp_compute->scalar_flag == 1) {
            double value = lmp_compute->compute_scalar();
            setHasScalarData(true);
            setScalarValue(value);
            CP1DData *scalarData = m_data1DRaw["scalar"];
            scalarData->add(lammpsController->system()->simulationTime(), value);
        }
        if(lmp_compute->vector_flag == 1) {

        }
        if(lmp_compute->array_flag == 1) {

        }
    }
}

void CPCompute::updateCommand()
{
    // For standard computes, command doesn't change
}

QList<QString> CPCompute::enabledCommands()
{
    return { fullCommand() };
}

QList<QString> CPCompute::disableCommands()
{
    return {QString("uncompute %1").arg(identifier())};
}

bool CPCompute::existsInLammps(LAMMPSController *lammpsController)
{
    LAMMPS_NS::Compute *compute = lammpsController->findComputeByIdentifier(identifier());
    return compute!=nullptr;
}

bool CPCompute::isVector() const
{
    return m_isVector;
}

QString CPCompute::group() const
{
    return m_group;
}

int CPCompute::frequency() const
{
    return m_frequency;
}

bool CPCompute::hasScalarData() const
{
    return m_hasScalarData;
}

float CPCompute::scalarValue() const
{
    return m_scalarValue;
}

int CPCompute::num1DData() const
{
    return m_num1DData;
}

QVariantMap CPCompute::data1D() const
{
    return m_data1D;
}

void CPCompute::setIsVector(bool isVector)
{
    if (m_isVector == isVector)
        return;

    m_isVector = isVector;
    emit isVectorChanged(isVector);
}

void CPCompute::setGroup(QString group)
{
    if (m_group == group)
        return;

    m_group = group;
    emit groupChanged(group);
}

void CPCompute::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(frequency);
}

void CPCompute::setHasScalarData(bool hasScalarData)
{
    if (m_hasScalarData == hasScalarData)
        return;

    m_hasScalarData = hasScalarData;
    if(m_hasScalarData) {
        m_data1DRaw["scalar"] = new CP1DData(this);
        m_data1D.insert(QString("scalar"), QVariant::fromValue<CP1DData*>(m_data1DRaw["scalar"]));
    }
    emit hasScalarDataChanged(hasScalarData);
}

void CPCompute::setScalarValue(float scalarValue)
{
    if (m_scalarValue == scalarValue)
        return;

    m_scalarValue = scalarValue;
    emit scalarValueChanged(scalarValue);
}

void CPCompute::setNum1DData(int num1DData)
{
    if (m_num1DData == num1DData)
        return;

    m_num1DData = num1DData;
    emit num1DDataChanged(num1DData);
}

void CPCompute::setData1D(QVariantMap data1D)
{
    if (m_data1D == data1D)
        return;

    m_data1D = data1D;
    emit data1DChanged(data1D);
}

QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), fullCommand() };
}

QString CPCompute::createCommandPrefix()
{
    return QString("compute %1 %2 ").arg(identifier()).arg(group());
}
