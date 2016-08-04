#include "cpvariable.h"
#include "system.h"
#include <variable.h>
#include "lammpscontroller.h"
#include <library.h>
#include "data1d.h"
#include <QDebug>
CPVariable::CPVariable()
{
    m_data = new Data1D(this);
}

void CPVariable::updateCommand()
{

}

QList<QString> CPVariable::enabledCommands()
{
    return {QString("variable %1 %2").arg(identifier()).arg(command())};
}

QList<QString> CPVariable::disableCommands()
{
    return {QString("variable %1 delete").arg(identifier())};
}

QList<QString> CPVariable::resetCommands()
{
    return { QString("variable %1 delete").arg(identifier()), QString("variable %1 %2").arg(identifier()).arg(command()) };
}

bool CPVariable::existsInLammps(LAMMPSController *lammpsController)
{
    return lammpsController->findVariableIndex(identifier())>=0;
}


void CPVariable::update(LAMMPSController *lammpsController)
{
    QElapsedTimer t;
    t.start();

    SimulatorControl::update(lammpsController);
    if(!lammpsController->lammps())  {
        return;
    }
    if(!existsInLammps(lammpsController)) return;
    if(m_dependencies.size()) {
        for(const QVariant &variant : m_dependencies) {
            SimulatorControl *dependency = qvariant_cast<SimulatorControl*>(variant);
            if(dependency) {
                // We might need to invoke our dependencies before the variable
                dependency->update(lammpsController);
            }
        }
    }

    if( (lammpsController->system()->timesteps() - m_lastUpdated) > m_frequency) {
        m_lastUpdated = lammpsController->system()->timesteps();
        try {
            QByteArray identifier_ = identifier().toLocal8Bit();
            double *variableValue = (double*)lammps_extract_variable(lammpsController->lammps(), identifier_.data(), "all");
            m_data->add(lammpsController->system()->simulationTime(), *variableValue);
            lammps_free(variableValue);
        } catch (LammpsException error) {
            qDebug() << "Error: " << QString::fromStdString(error.error());
        }
    }

    // qDebug() << "Variable " << identifier() << " spent " << t.elapsed() << " ms to update.";
}

int CPVariable::frequency() const
{
    return m_frequency;
}

Data1D *CPVariable::data() const
{
    return m_data;
}

void CPVariable::setFrequency(int frequency)
{
    if (m_frequency == frequency)
        return;

    m_frequency = frequency;
    emit frequencyChanged(frequency);
}

void CPVariable::setData(Data1D *data)
{
    if (m_data == data)
        return;

    m_data = data;
    emit dataChanged(data);
}
