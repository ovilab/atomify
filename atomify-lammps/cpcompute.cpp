#include "cpcompute.h"
#include "lammpscontroller.h"
#include "mysimulator.h"

CPCompute::CPCompute(QObject *parent) : SimulatorControl(parent)
{

}

void CPCompute::setValues(double time, QVector<double> values)
{
    m_time = time;
    m_values.clear();
    m_values = QList<double>::fromVector(values);
    if(m_values.size()>0) {
       emit valuesChanged(m_values);
       emit firstValueChanged(m_values.at(0));
       if(values.size() > 1) secondValueChanged(m_values.at(1));
       if(values.size() > 2) thirdValueChanged(m_values.at(2));
       if(values.size() > 3) fourthValueChanged(m_values.at(3));
    }
}

void CPCompute::updateCommand()
{
    // For standard computes, command doesn't change
}

void CPCompute::update(LAMMPSController *lammpsController)
{
    SimulatorControl::update(lammpsController);
    LAMMPS_NS::Compute *lmp_compute = lammpsController->findComputeByIdentifier(identifier());
    if(lmp_compute != nullptr) {
       QVector<double> newValues;

       if(isVector()) {
           lmp_compute->compute_vector();
           double *values = lmp_compute->vector;
           int numValues = lmp_compute->size_vector;

           for(int i=0; i<numValues; i++) {
               newValues.push_back(values[i]);
           }
       } else {
           double value = lmp_compute->compute_scalar();
           newValues.push_back(value);
       }

       setValues(lammpsController->simulationTime(), newValues);
    }
}

QList<QString> CPCompute::enabledCommands()
{
    return { QString("compute %1 %2").arg(identifier()).arg(command()) };
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

QList<double> CPCompute::values() const
{
    return m_values;
}

double CPCompute::firstValue() const
{
    if(m_values.size()<1) return NAN;
    return m_values.at(0);
}

double CPCompute::secondValue() const
{
    if(m_values.size()<2) return NAN;
    return m_values.at(1);
}

double CPCompute::thirdValue() const
{
    if(m_values.size()<3) return NAN;
    return m_values.at(2);
}

double CPCompute::fourthValue() const
{
    if(m_values.size()<4) return NAN;
    return m_values.at(3);
}

double CPCompute::time() const
{
    return m_time;
}

bool CPCompute::isVector() const
{
    return m_isVector;
}

void CPCompute::setTime(double time)
{
    if (m_time == time)
        return;

    m_time = time;
    emit timeChanged(time);
}

void CPCompute::setIsVector(bool isVector)
{
    if (m_isVector == isVector)
        return;

    m_isVector = isVector;
    emit isVectorChanged(isVector);
}


QList<QString> CPCompute::resetCommands()
{
    return { QString("uncompute %1").arg(identifier()), QString("compute %1 %2").arg(identifier()).arg(command()) };
}
