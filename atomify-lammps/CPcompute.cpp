#include "CPcompute.h"
#include "mysimulator.h"
#include "lammpscontroller.h"
CPCompute::CPCompute(QObject *parent) : QObject(parent)
{

}

QString CPCompute::identifier() const
{
    return m_identifier;
}

QString CPCompute::command() const
{
    return m_command;
}

MySimulator *CPCompute::simulator() const
{
    return m_simulator;
}

QStringList CPCompute::dependencies() const
{
    return m_dependencies;
}

void CPCompute::setIdentifier(QString identifier)
{
    if (m_identifier == identifier)
        return;

    m_identifier = identifier;
    emit identifierChanged(identifier);
}

void CPCompute::setCommand(QString command)
{
    if (m_command == command)
        return;

    m_command = command;
    emit commandChanged(command);
}

void CPCompute::setSimulator(MySimulator *simulator)
{
    if (m_simulator == simulator)
        return;

    m_simulator = simulator;
    simulator->addCompute(this);
    emit simulatorChanged(simulator);
}

void CPCompute::setDependencies(QStringList dependencies)
{
    if (m_dependencies == dependencies)
        return;

    m_dependencies = dependencies;
    emit dependenciesChanged(dependencies);
}
