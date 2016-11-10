#include "mysimulator.h"
#include "states.h"
#include "LammpsWrappers/lammpserror.h"
#include "LammpsWrappers/simulatorcontrols/simulatorcontrol.h"
#include <library.h>
#include <atom.h>
#include <domain.h>
#include <compute_temp.h>
#include <update.h>
#include <modify.h>
#include <neighbor.h>
#include <neigh_list.h>
#include <string>
#include <sstream>
#include <SimVis/SphereData>
#include <QString>
#include <QQmlFile>
#include <QDir>
#include <iostream>
#include <fstream>
#include <memory>
#include <QStandardPaths>
#include "parser/scripthandler.h"
#include "parser/scriptcommand.h"
#include "LammpsWrappers/atoms.h"
#include "LammpsWrappers/modifiers/modifiers.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/computes.h"
#include "LammpsWrappers/fixes.h"
#include "LammpsWrappers/bonds.h"

using namespace std;

MyWorker::MyWorker() {
    m_sinceStart.start();
    m_elapsed.start();
}

AtomifySimulator::AtomifySimulator() :
    m_scriptHandler(new ScriptHandler()),
    m_system(new System(this)),
    m_states(new States(this)),
    m_simulationSpeed(1)
{
    m_states->setupStates(*this);
}

void AtomifySimulator::togglePaused()
{
    if(m_states->paused()->active()) {
        emit unPaused();
    } else if(m_states->parsing()->active() || m_states->continued()->active()) {
        emit paused();
    } else if(m_states->finished()->active()) {
        emit continued();
    }
}

System *AtomifySimulator::system() const
{
    return m_system;
}

LammpsError *AtomifySimulator::lammpsError() const
{
    return m_lammpsError;
}

States *AtomifySimulator::states() const
{
    return m_states;
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    QElapsedTimer t; t.start();

    AtomifySimulator *atomifySimulator = qobject_cast<AtomifySimulator*>(simulator);
    States &states = *atomifySimulator->states();

    // Sync properties from lammps controller and back
    m_lammpsController.setSystem(atomifySimulator->system());

    // If user pressed stop / restart, we should reset
    if(states.reset()->active()) {
        m_lammpsController.stop();
        m_lammpsController.commands.clear();
        atomifySimulator->system()->reset();
        atomifySimulator->setLammpsError(nullptr);
        emit atomifySimulator->didReset();
    }

    // If we don't have a LAMMPS object, but we have a new script (aka in parsing state), create LAMMPS object
    if(!m_lammpsController.lammps() && states.parsing()->active()) {
        m_lammpsController.start();
    }

    // If we're idling, we should synchronize visuals anyway
    if(states.idle()->active()) {
        // atomifySimulator->system()->synchronize(&m_lammpsController);
        return;
    }

    atomifySimulator->system()->atoms()->updateData(atomifySimulator->system(), nullptr);

    // If we crashed and haven't handled it yet, do it here

//    if(m_lammpsController.crashed() && !m_lammpsController.exceptionHandled()) {
//        LammpsError *error = new LammpsError();
//        error->create(m_lammpsController);
//        atomifySimulator->setLammpsError(error);
//        m_lammpsController.setExceptionHandled(true); // TODO: put in states
//        emit atomifySimulator->crashed();
//        return;
//    }

    // Synchronize visuals
    atomifySimulator->system()->synchronize(&m_lammpsController);
    atomifySimulator->system()->atoms()->synchronizeRenderer();

    ScriptHandler &handler = *atomifySimulator->scriptHandler();
    handler.didFinishPreviousCommands();

    m_lammpsController.commands = handler.nextCommands(m_lammpsController);
    for(ScriptCommand &command : handler.editorCommands()) {
        QString commandString = command.command();
        commandString.remove(0,2);

        handler.parser().atomColor(commandString, [&](int atomType, QColor color) {
            atomifySimulator->system()->atoms()->setAtomColor(atomType, color);
        });

        handler.parser().atomColorAndSize(commandString, [&](int atomType, QColor color, float size) {
            atomifySimulator->system()->atoms()->setAtomColorAndScale(atomType, color, size);
        });

        handler.parser().atomType(commandString, [&](int atomType, QString atomTypeName) {
            atomifySimulator->system()->atoms()->setAtomType(atomType, atomTypeName);
        });

        handler.parser().bond(commandString, [&](int atomType1, int atomType2, float bondLength) {
            Bonds *bonds = atomifySimulator->system()->atoms()->bonds();
            if(bonds->bondLengths().size() > std::max(atomType1, atomType2)) {
                bonds->bondLengths()[atomType1][atomType2] = bondLength;
                bonds->bondLengths()[atomType2][atomType1] = bondLength;
                bonds->setEnabled(true);
            }
        });
    }
    handler.editorCommands().clear();

    qDebug() << "Added new commands: ";
    for(ScriptCommand &cmd : m_lammpsController.commands) {
        qDebug() << "    " << cmd.command();
    }
    if(m_lammpsController.commands.size()==0) {
        emit atomifySimulator->finished();
    }
}

void MyWorker::work()
{
    m_workCount += 1;
    bool didWork = m_lammpsController.tick();
//    if(m_lammpsController.state.canProcessSimulatorControls) {
//        m_lammpsController.system()->computes()->computeAll(&m_lammpsController);
//        m_lammpsController.system()->atoms()->updateData(m_lammpsController.system(), m_lammpsController.lammps());
//    }

    auto dt = m_elapsed.elapsed();
    double delta = 16 - dt;
    if(delta > 0) {
        QThread::currentThread()->msleep(delta);
    }

    if(!didWork) {
        QThread::currentThread()->msleep(500);
    }
    m_elapsed.restart();
}

MyWorker *AtomifySimulator::createWorker()
{
    return new MyWorker();
}

ScriptHandler *AtomifySimulator::scriptHandler() const
{
    return m_scriptHandler;
}

int AtomifySimulator::simulationSpeed() const
{
    return m_simulationSpeed;
}

void AtomifySimulator::setSimulationSpeed(int arg)
{
    if (m_simulationSpeed == arg)
        return;

    m_simulationSpeed = arg;
    emit simulationSpeedChanged(arg);
}

void AtomifySimulator::setScriptHandler(ScriptHandler *scriptHandler)
{
    if (m_scriptHandler == scriptHandler)
        return;

    m_scriptHandler = scriptHandler;
    emit scriptHandlerChanged(scriptHandler);
}

void AtomifySimulator::setSystem(System *system)
{
    if (m_system == system)
        return;

    m_system = system;
    emit systemChanged(system);
}

void AtomifySimulator::setLammpsError(LammpsError *lammpsError)
{
    if (m_lammpsError == lammpsError)
        return;

    m_lammpsError = lammpsError;
    emit lammpsErrorChanged(lammpsError);
}

void AtomifySimulator::setStates(States *states)
{
    if (m_states == states)
        return;

    m_states = states;
    emit statesChanged(states);
}
