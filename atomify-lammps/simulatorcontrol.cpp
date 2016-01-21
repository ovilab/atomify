#include "simulatorcontrol.h"
#include "lammpscontroller.h"
#include "scripthandler.h"
#include "mysimulator.h"

SimulatorControl::SimulatorControl(QQuickItem *parent) : QQuickItem(parent)
{

}

SimulatorControl::~SimulatorControl()
{
    m_dependencies.clear();
}

bool SimulatorControl::addToLammps(LAMMPSController *lammpsController) {
    for(const QVariant &variant : m_dependencies) {
        SimulatorControl *dependency = qvariant_cast<SimulatorControl*>(variant);
        if(dependency && !dependency->existsInLammps(lammpsController)) {
            // We found one dependency that is not added to LAMMPS, abort this mission
            return false;
        }
    }

    ScriptHandler *scriptHandler = lammpsController->scriptHandler();
    scriptHandler->addCommandsToTop(enabledCommands(), ScriptCommand::Type::SingleCommand);
    return true;
}

void SimulatorControl::addDependency(SimulatorControl *control)
{
    bool found = false;
    for(QVariant &variant : m_dependencies) {
        SimulatorControl *thisControl = variant.value<SimulatorControl*>();
        if(thisControl==control) {
            found = true;
            break;
        }
    }

    if(!found) {
        m_dependencies.push_back(QVariant::fromValue(control));
    }
}

void SimulatorControl::removeDependency(SimulatorControl *control)
{
    for(QVariant &variant : m_dependencies) {
        SimulatorControl *thisControl = variant.value<SimulatorControl*>();
        if(thisControl==control) {
            m_dependencies.removeOne(variant);
            break;
        }
    }
}

bool SimulatorControl::dependenciesValid(LAMMPSController *lammpsController)
{
    bool valid = true;
    for(const QVariant &variant : m_dependencies) {
        SimulatorControl *dependency = qvariant_cast<SimulatorControl*>(variant);
        if(dependency) {
            // Check if the dependency is not in lammps or one of its dependencies are not in lammps
            if(!dependency->existsInLammps(lammpsController) || !dependency->dependenciesValid(lammpsController)) {
                valid = false;
            }
        }
    }

    return valid;
}

void SimulatorControl::update(LAMMPSController *lammpsController)
{
    if(!lammpsController->scriptHandler()) {
        return;
    }

    bool exists = existsInLammps(lammpsController);
    if(!exists && m_enabled) {
        // We should exist, so let's try to add.
        // Whatever happens, just return. We aren't ready to compute any values yet anyway.
        addToLammps(lammpsController);
        return;
    }

    if(exists && !m_enabled || !dependenciesValid(lammpsController)) {
        // We should not exist, but we do. Now remove from lammps
        lammpsController->scriptHandler()->addCommandsToTop(disableCommands(), ScriptCommand::Type::SingleCommand);
    }

    if(exists) {
        QString currentCommand = command();
        updateCommand();
        if(currentCommand!=command()) {

            lammpsController->scriptHandler()->addCommandsToTop(resetCommands(), ScriptCommand::Type::SingleCommand);
        }
    }
}

bool SimulatorControl::enabled() const
{
    return m_enabled;
}

QString SimulatorControl::identifier() const
{
    return m_commandPrefix+m_identifier;
}

QString SimulatorControl::command() const
{
    return m_command;
}

QVariantList SimulatorControl::dependencies() const
{
    return m_dependencies;
}

void SimulatorControl::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;
    m_enabled = enabled;
    emit enabledChanged(enabled);
}

void SimulatorControl::setIdentifier(QString identifier)
{
    if (m_identifier == identifier)
        return;

    m_identifier = identifier;
    emit identifierChanged(identifier);
}

void SimulatorControl::setCommand(QString command)
{
    if (m_command == command)
        return;

    m_command = command;
    emit commandChanged(command);
}

void SimulatorControl::setDependencies(QVariantList dependencies)
{
    if (m_dependencies == dependencies)
        return;

    m_dependencies = dependencies;
    emit dependenciesChanged(dependencies);
}
