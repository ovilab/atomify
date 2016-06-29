#include "cpcompute.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include "cpdata.h"
#include "../system.h"
#include <QDebug>
CPCompute::CPCompute(Qt3DCore::QNode *parent) : SimulatorControl(parent)
{
    m_scalarData = new CPScalarData();
}

CPCompute::~CPCompute() { }

void CPCompute::copyData(LAMMPSController *lammpsController)
{
    LAMMPS_NS::Compute *lmp_compute = lammpsController->findComputeByIdentifier(identifier());
    if(lmp_compute != nullptr) {
        if(lmp_compute->scalar_flag == 1) {
            double value = lmp_compute->compute_scalar();
            if(!m_scalarData) m_scalarData = new CPScalarData();
            m_scalarData->add(lammpsController->system()->simulationTime(), value);
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

CPScalarData *CPCompute::scalarData() const
{
    return m_scalarData;
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

void CPCompute::setScalarData(CPScalarData *scalarData)
{
    if (m_scalarData == scalarData)
        return;

    m_scalarData = scalarData;
    emit scalarDataChanged(scalarData);
}

QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), fullCommand() };
}

QString CPCompute::createCommandPrefix()
{
    return QString("compute %1 %2 ").arg(identifier()).arg(group());
}
