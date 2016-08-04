#include "mysimulator.h"
#include "LammpsWrappers/simulatorcontrol.h"
#include <library.h>
#include <atom.h>
#include <domain.h>
#include <compute_temp.h>
#include <update.h>
#include <modify.h>
#include <neighbor.h>
#include <neigh_list.h>
#include <core/camera.h>
#include <string>
#include <sstream>
#include <SimVis/Spheres>
#include <SimVis/Lines>
#include <SimVis/Cylinders>
#include <SimVis/Points>
#include <QString>
#include <QQmlFile>
#include <QDir>
#include <iostream>
#include <fstream>
#include <memory>
#include <QStandardPaths>
#include <renderables/bonds/bonds.h>
#include "LammpsWrappers/system.h"

using namespace std;

MyWorker::MyWorker() {
    m_sinceStart.start();
    m_elapsed.start();
    m_lammpsController.setWorker(this);
}

void AtomifySimulator::clearSimulatorControls()
{
    //    for(QQuickItem* child : childItems()) {
    //        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
    //        if(control) {
    //            control->setParentItem(nullptr);
    //        }
    //    }
    m_simulatorControls.clear();
    qDebug() << "Clear simulator controls";
}

void AtomifySimulator::addSimulatorControl(SimulatorControl *simulatorControl)
{
    qDebug() << "Add simulator control with command" << simulatorControl->command();
    m_simulatorControls.append(simulatorControl);
}

System *AtomifySimulator::system() const
{
    return m_system;
}

LammpsError *AtomifySimulator::lammpsError() const
{
    return m_lammpsError;
}

bool AtomifySimulator::hasExecutedRunCommand() const
{
    return m_hasExecutedRunCommand;
}
bool AtomifySimulator::running() const
{
    return m_running;
}

int AtomifySimulator::currentRunStep() const
{
    return m_currentRunStep;
}

int AtomifySimulator::runStepCount() const
{
    return m_runStepCount;
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    AtomifySimulator *atomifySimulator = qobject_cast<AtomifySimulator*>(simulator);

    m_running = atomifySimulator->m_running;
    if(!atomifySimulator->m_running) {
        return;
    }

    // Sync properties from lammps controller and back
    atomifySimulator->scriptHandler()->setAtoms(atomifySimulator->system()->atoms());
    m_lammpsController.setScriptHandler(atomifySimulator->scriptHandler());
    m_lammpsController.setPaused(atomifySimulator->paused());
    m_lammpsController.setSimulationSpeed(atomifySimulator->simulationSpeed());
    m_lammpsController.setSystem(atomifySimulator->system());
    m_lammpsController.state.staticSystem = atomifySimulator->lammpsState.staticSystem;
    m_atoms = atomifySimulator->system()->atoms();
    atomifySimulator->setHasExecutedRunCommand(m_lammpsController.state.hasExecutedRunCommand);
    atomifySimulator->setCurrentRunStep(m_lammpsController.state.runCommandCurrent - m_lammpsController.state.runCommandStart);
    atomifySimulator->setRunStepCount(m_lammpsController.state.runCommandEnd - m_lammpsController.state.runCommandStart);

    if(m_willPause) {
        m_lammpsController.setPaused(true);
        atomifySimulator->setPaused(true);
        m_willPause = false;
    }

    if(atomifySimulator->willReset()) {
        m_lammpsController.reset();
        atomifySimulator->lammpsState = m_lammpsController.state;
        atomifySimulator->setWillReset(false);
        atomifySimulator->scriptHandler()->setLammpsState(&atomifySimulator->lammpsState);
        atomifySimulator->system()->reset();
        atomifySimulator->setLammpsError(nullptr);
        m_lammpsController.setSimulationSpeed(1); // Will update simulator later
        atomifySimulator->setHasExecutedRunCommand(false);
        emit atomifySimulator->lammpsDidReset();
    }

    if(!m_lammpsController.lammps()) {
        atomifySimulator->system()->synchronize(nullptr);
        atomifySimulator->system()->atoms()->updateData(atomifySimulator->system(), nullptr);
        return;
    }

    if(m_lammpsController.crashed() && !m_lammpsController.currentException().isReported()) {
        LammpsError *error = new LammpsError(atomifySimulator);
        error->setMessage(QString(m_lammpsController.currentException().error().c_str()).trimmed());
        error->setCommand(m_lammpsController.state.nextCommand.command());
        error->setScriptFile(m_lammpsController.state.nextCommand.filename());
        error->setLine(m_lammpsController.state.nextCommand.line());
        atomifySimulator->setLammpsError(error);
        m_lammpsController.currentException().setIsReported(true);

        emit atomifySimulator->errorInLammpsScript();
        return;
    }

    if(m_lammpsController.state.canProcessSimulatorControls) {
        foreach(SimulatorControl *control, atomifySimulator->m_simulatorControls) {
            control->update(&m_lammpsController);
        }
    }

    if(!m_lammpsController.state.runCommandActive) {
        ScriptHandler *scriptHandler = atomifySimulator->m_scriptHandler;
        ScriptParser &scriptParser = scriptHandler->parser();
        ScriptCommand nextCommandObject = scriptHandler->nextCommand();

        QString nextCommand = nextCommandObject.command();
        if(scriptParser.isEditorCommand(nextCommand)) {
            bool handled = scriptHandler->parseGUICommand(nextCommand);
            if(!handled) {
                handled = scriptHandler->parseLammpsCommand(nextCommand, &m_lammpsController);
                atomifySimulator->setSimulationSpeed(m_lammpsController.simulationSpeed()); // A command might have changed it
            }
            if(!handled) {
                qDebug() << "Error, editor command " << nextCommand << " was not handled";
                exit(1);
            }
            m_lammpsController.state.nextCommand = ScriptCommand("", ScriptCommand::Type::SkipLammpsTick);
        } else {
            QString command = nextCommand;
            command = command.trimmed();
            command.remove(0,2);

            if(scriptHandler->parser().isSimulationSpeed(command)) {
                int speed = scriptHandler->parser().simulationSpeed(command);
                if(speed > 0) atomifySimulator->setSimulationSpeed(speed);
                m_lammpsController.state.nextCommand = ScriptCommand("", ScriptCommand::Type::SkipLammpsTick);

            } else {
                for(auto *simulatorControl : atomifySimulator->findChildren<SimulatorControl*>()) {
                    simulatorControl->handleCommand(nextCommandObject.command());
                }
                m_lammpsController.state.nextCommand = nextCommandObject;
            }
        }
    }
}

void MyWorker::synchronizeBonds(Bonds *bonds) {
    bonds->setData(m_atoms->bondsData());
    bonds->setDirty(true);
}

void MyWorker::synchronizeSpheres(Spheres *spheres) {
    spheres->setData(m_atoms->spheresData());
    spheres->setDirty(true);
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    if(!m_running) {
        return;
    }
    LAMMPS *lammps = m_lammpsController.lammps();
    if(!lammps) return;

    Spheres *spheres = qobject_cast<Spheres*>(renderableObject);
    if(spheres) {
        synchronizeSpheres(spheres);
    }
    Bonds *bonds = qobject_cast<Bonds*>(renderableObject);
    if(bonds) {
        synchronizeBonds(bonds);
    }

}

void MyWorker::work()
{
    if(!m_running) {
        return;
    }
    m_lammpsController.tick();
    auto dt = m_elapsed.elapsed();
    double delta = 16 - dt;
    if(delta > 0) {
        QThread::currentThread()->msleep(delta);
    }
    m_elapsed.restart();
    if(m_lammpsController.lammps() && m_lammpsController.system()) {
        m_lammpsController.system()->synchronize(m_lammpsController.lammps());
        m_lammpsController.system()->atoms()->updateData(m_lammpsController.system(), m_lammpsController.lammps());
    }
}
bool MyWorker::willPause() const
{
    return m_willPause;
}

void MyWorker::setWillPause(bool willPause)
{
    m_willPause = willPause;
}


MyWorker *AtomifySimulator::createWorker()
{
    return new MyWorker();
}

void AtomifySimulator::setCurrentRunStep(int currentRunStep)
{
    if (m_currentRunStep == currentRunStep)
        return;

    m_currentRunStep = currentRunStep;
    emit currentRunStepChanged(currentRunStep);
}

void AtomifySimulator::setRunStepCount(int runStepCount)
{
    if (m_runStepCount == runStepCount)
        return;

    m_runStepCount = runStepCount;
    emit runStepCountChanged(runStepCount);
}

bool AtomifySimulator::paused() const
{
    return m_paused;
}

ScriptHandler *AtomifySimulator::scriptHandler() const
{
    return m_scriptHandler;
}

bool AtomifySimulator::willReset() const
{
    return m_willReset;
}

AtomifySimulator::AtomifySimulator() :
    m_system(new System(this)),
    m_scriptHandler(new ScriptHandler())
{

}

AtomifySimulator::~AtomifySimulator() { }

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

void AtomifySimulator::setPaused(bool paused)
{
    if (m_paused == paused)
        return;

    m_paused = paused;
    emit pausedChanged(paused);
}

void AtomifySimulator::setScriptHandler(ScriptHandler *scriptHandler)
{
    if (m_scriptHandler == scriptHandler)
        return;

    m_scriptHandler = scriptHandler;
    emit scriptHandlerChanged(scriptHandler);
}

void AtomifySimulator::setWillReset(bool willReset)
{
    if (m_willReset == willReset)
        return;

    m_willReset = willReset;
    emit willResetChanged(willReset);
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

void AtomifySimulator::setHasExecutedRunCommand(bool hasExecutedRunCommand)
{
    if (m_hasExecutedRunCommand == hasExecutedRunCommand)
        return;

    m_hasExecutedRunCommand = hasExecutedRunCommand;
    emit hasExecutedRunCommandChanged(hasExecutedRunCommand);
}

void AtomifySimulator::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(running);
}
