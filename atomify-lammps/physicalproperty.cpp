#include "physicalproperty.h"
#include "lammps/modify.h"
#include "lammps/compute.h"
#include "lammps/update.h"
#include "mysimulator.h"
#include "lammpscontroller.h"
#include <cassert>

PhysicalProperty::PhysicalProperty(QObject *parent) : QObject(parent)
{

}

MySimulator *PhysicalProperty::simulator() const
{
    return m_simulator;
}

bool PhysicalProperty::enabled() const
{
    return m_enabled;
}

QString PhysicalProperty::command() const
{
    return m_command;
}

void PhysicalProperty::setSimulator(MySimulator *simulator)
{
    if (m_simulator == simulator)
        return;

    m_simulator = simulator;

    emit simulatorChanged(simulator);
}

void PhysicalProperty::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}

void PhysicalProperty::setCommand(QString command)
{
    if (m_command == command)
        return;

    m_command = command;
    emit commandChanged(command);
}

void PhysicalProperty::setIdentifier(QString identifier)
{
    if (m_identifier == identifier)
        return;

    m_identifier = identifier;
    emit identifierChanged(identifier);
}

void PhysicalProperty::setDependencies(QStringList dependencies)
{
    if (m_dependencies == dependencies)
        return;

    m_dependencies = dependencies;
    emit dependenciesChanged(dependencies);
}
bool PhysicalProperty::valuesDirty() const
{
    return m_valuesDirty;
}

void PhysicalProperty::setValuesDirty(bool valuesDirty)
{
    m_valuesDirty = valuesDirty;
}


QString PhysicalProperty::identifier() const
{
    return m_identifier;
}

void PhysicalProperty::addToLAMMPS(LAMMPSController *lammpsController)
{
    for(QString &dependency : m_dependencies) {
        //First see if all dependencies are found
        int computeid = lammpsController->lammps()->modify->find_compute(dependency.toStdString().c_str());
        if(computeid < 0) return; // This dependency isn't added yet
    }
    lammpsController->runCommand(m_command);

    int icompute = lammpsController->lammps()->modify->find_compute(m_identifier.toStdString().c_str());
    assert(icompute >= 0 && "Tried to create a compute but didn't work.");
    m_compute = lammpsController->lammps()->modify->compute[icompute];
    m_lammpsController = lammpsController;
    m_valuesDirty = false;
}
#include "lammps/input.h"
#include "lammps/variable.h"
#include <iostream>
using namespace std;

void PhysicalProperty::compute()
{
    if(m_compute == NULL) return;

    if(m_type==Scalar) {
        // if (m_compute->invoked_scalar != m_lammpsController->lammps()->update->ntimestep)
        m_compute->compute_scalar();
    } else if(m_type==Vector) m_compute->compute_vector();
    else if(m_type==Array) m_compute->compute_array();
    else if(m_type==PerAtom) m_compute->compute_peratom();
    else if(m_type==Local) m_compute->compute_local();
    m_valuesDirty = true;
}

double PhysicalProperty::scalar() const
{
    if(m_compute == NULL) return 0;
    else return m_compute->scalar;
}

QStringList PhysicalProperty::dependencies() const
{
    return m_dependencies;
}



