#include "mysimulator.h"
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

void AtomifySimulator::clearSimulatorControls()
{
    for(QObject* child : children()) {
        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
        if(control) {
            control->setParent((QNode*) 0);
        }
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

bool AtomifySimulator::automaticallyRun() const
{
    return m_automaticallyRun;
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    QElapsedTimer t;
    t.start();
    AtomifySimulator *atomifySimulator = qobject_cast<AtomifySimulator*>(simulator);

    // Sync properties from lammps controller and back
    atomifySimulator->scriptHandler()->setAtoms(atomifySimulator->system()->atoms());
    m_lammpsController.setScriptHandler(atomifySimulator->scriptHandler());
    m_lammpsController.setPaused(atomifySimulator->paused());
    m_lammpsController.setSimulationSpeed(atomifySimulator->simulationSpeed());
    m_lammpsController.setSystem(atomifySimulator->system());
    m_lammpsController.state.automaticallyRun = atomifySimulator->automaticallyRun();
    m_lammpsController.state.staticSystem = atomifySimulator->lammpsState.staticSystem;

    if(m_willPause) {
        m_lammpsController.setPaused(true);
        atomifySimulator->setPaused(true);
        m_willPause = false;
    }

    if(atomifySimulator->willReset()) {
        m_lammpsController.reset();
        m_lammpsController.state.automaticallyRun = atomifySimulator->automaticallyRun();
        atomifySimulator->lammpsState = m_lammpsController.state;
        atomifySimulator->setWillReset(false);
        atomifySimulator->scriptHandler()->setLammpsState(&atomifySimulator->lammpsState);
        // atomifySimulator->clearSimulatorControls();
        atomifySimulator->system()->reset();
        atomifySimulator->setLammpsError(nullptr);
        emit atomifySimulator->lammpsDidReset();
    }

    if(!m_lammpsController.lammps()) {
        atomifySimulator->system()->synchronize(&m_lammpsController);
        atomifySimulator->system()->atoms()->updateData(atomifySimulator->system(), nullptr);
        return;
    }

    if(m_lammpsController.crashed() && !m_lammpsController.exceptionHandled()) {
        LammpsError *error = new LammpsError(atomifySimulator);

        error->setMessage(QString::fromStdString(m_lammpsController.currentException().message).trimmed());
        error->setCommand(m_lammpsController.state.nextCommand.command());
        error->setScriptFile(m_lammpsController.state.nextCommand.filename());
        error->setScriptPath(m_lammpsController.state.nextCommand.path());
        error->setLine(m_lammpsController.state.nextCommand.line());
        atomifySimulator->setLammpsError(error);
        m_lammpsController.setExceptionHandled(true);

        emit atomifySimulator->errorInLammpsScript();
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
    // atomifySimulator->system()->atoms()->updateData(atomifySimulator->system(), m_lammpsController.lammps());

    if(m_lammpsController.crashed()) return;

    if(!m_lammpsController.state.runCommandActive) {
        ScriptHandler *scriptHandler = atomifySimulator->m_scriptHandler;
        ScriptParser &scriptParser = scriptHandler->parser();
        ScriptCommand nextCommandObject = scriptHandler->nextCommand();

        QString nextCommand = nextCommandObject.command();
        if(scriptParser.isEditorCommand(nextCommand) && scriptParser.isGUICommand(nextCommand)) {
            scriptHandler->parseGUICommand(nextCommand);
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
    // qDebug() << "Full synchronization spending " << t.elapsed() << " ms.";
}

void MyWorker::work()
{
    // qDebug() << "Doing work " << m_workCount;
    m_workCount += 1;
    m_lammpsController.tick();
    if(m_lammpsController.state.canProcessSimulatorControls) {
        m_lammpsController.system()->computes()->computeAll(&m_lammpsController);
        m_lammpsController.system()->atoms()->updateData(m_lammpsController.system(), m_lammpsController.lammps());
    }
    auto dt = m_elapsed.elapsed();
    double delta = 16 - dt;
    if(delta > 0) {
        QThread::currentThread()->msleep(delta);
    }
    if(!m_lammpsController.lammps() || m_lammpsController.state.paused || m_lammpsController.state.crashed) {
        QThread::currentThread()->msleep(500);
    }
    m_elapsed.restart();
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
    m_scriptHandler(new ScriptHandler()),
    m_system(new System(this))
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

void AtomifySimulator::setAutomaticallyRun(bool automaticallyRun)
{
    if (m_automaticallyRun == automaticallyRun)
        return;

    m_automaticallyRun = automaticallyRun;
    emit automaticallyRunChanged(automaticallyRun);
}
