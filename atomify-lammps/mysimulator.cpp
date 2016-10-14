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
#include "scripthandler.h"
#include "LammpsWrappers/atoms.h"
#include "LammpsWrappers/modifiers/modifiers.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/computes.h"
#include "LammpsWrappers/fixes.h"

using namespace std;

MyWorker::MyWorker() {
    m_sinceStart.start();
    m_elapsed.start();
    m_lammpsController.setWorker(this);
}

AtomifySimulator::AtomifySimulator() :
    m_scriptHandler(new ScriptHandler()),
    m_system(new System(this)),
    m_states(new States(this)),
    m_simulationSpeed(1)
{
    m_states->setupStates(*this);
}

void AtomifySimulator::clearSimulatorControls()
{
    for(QObject* child : children()) {
        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
        if(control) {
            control->setParent((QNode*) 0);
        }
    }
}

void AtomifySimulator::togglePaused()
{
    if(m_states->paused()->active()) {
        emit unPaused();
    } else if(m_states->parsing()->active() || m_states->continued()->active()) {
        emit paused();
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
    AtomifySimulator *atomifySimulator = qobject_cast<AtomifySimulator*>(simulator);

    QElapsedTimer t; t.start();
    // Sync properties from lammps controller and back
    atomifySimulator->scriptHandler()->setAtoms(atomifySimulator->system()->atoms()); //TODO: fix ownership
    m_lammpsController.setScriptHandler(atomifySimulator->scriptHandler());
    m_lammpsController.setSimulationSpeed(atomifySimulator->simulationSpeed()); // TODO: enable speed in script
    m_lammpsController.setSystem(atomifySimulator->system());
    m_lammpsController.states = atomifySimulator->states();

    States &states = *atomifySimulator->states();

    if(states.reset()->active()) {
        m_lammpsController.reset();
        atomifySimulator->system()->reset();
        atomifySimulator->setLammpsError(nullptr);
        emit atomifySimulator->didReset();
    }

    if(!m_lammpsController.lammps() && states.parsing()->active()) {
        m_lammpsController.start();
    }

    if(states.idle()->active()) {
        atomifySimulator->system()->synchronize(&m_lammpsController);
        atomifySimulator->system()->atoms()->updateData(atomifySimulator->system(), nullptr);
        return;
    }

    if(m_lammpsController.crashed() && !m_lammpsController.exceptionHandled()) {
        LammpsError *error = new LammpsError();
        error->create(m_lammpsController);
        atomifySimulator->setLammpsError(error);
        m_lammpsController.setExceptionHandled(true); // TODO: put in states
        emit atomifySimulator->crashed();
        return;
    }

    QMap<QString, SimulatorControl*> controls;
    for(QObject* child : atomifySimulator->children()) {
        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
        if(control) {
            if(!controls.contains(control->identifier())) {
                controls.insert(control->identifier(), control);
            }
        }
    }
    m_lammpsController.simulatorControls = controls; // This object is visible from the Computes class

    if(m_lammpsController.state.canProcessSimulatorControls) {
        foreach(SimulatorControl *control, controls) {
            control->update(&m_lammpsController);
        }
    }

    atomifySimulator->system()->synchronize(&m_lammpsController);
    atomifySimulator->system()->atoms()->synchronizeRenderer();

    if(!m_lammpsController.state.runCommandActive) {
        ScriptHandler *scriptHandler = atomifySimulator->m_scriptHandler;
        ScriptParser &scriptParser = scriptHandler->parser();
        ScriptCommand nextCommandObject = scriptHandler->nextCommand();

        QString nextCommand = nextCommandObject.command();
        if(nextCommandObject.type() == ScriptCommand::Type::NoCommand) {
            emit atomifySimulator->finished();
        }

        if(scriptParser.isEditorCommand(nextCommand) && scriptParser.isGUICommand(nextCommand)) {
            scriptHandler->parseGUICommand(nextCommand);
            m_lammpsController.state.nextCommand = ScriptCommand("", ScriptCommand::Type::SkipLammpsTick);
        } else {
            QString command = nextCommand;
            command = command.trimmed();
            command.remove(0,2);

            for(auto *simulatorControl : atomifySimulator->findChildren<SimulatorControl*>()) {
                simulatorControl->handleCommand(nextCommandObject.command());
            }
            m_lammpsController.state.nextCommand = nextCommandObject;
        }
    }
    // qDebug() << "Full synchronization spending " << t.elapsed() << " ms.";
}

void MyWorker::work()
{
    m_workCount += 1;
    bool didWork = m_lammpsController.tick();
    if(m_lammpsController.state.canProcessSimulatorControls) {
        m_lammpsController.system()->computes()->computeAll(&m_lammpsController);
        m_lammpsController.system()->atoms()->updateData(m_lammpsController.system(), m_lammpsController.lammps());
    }

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
