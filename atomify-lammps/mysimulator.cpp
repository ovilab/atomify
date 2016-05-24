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

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    AtomifySimulator *atomifySimulator = qobject_cast<AtomifySimulator*>(simulator);

    if(atomifySimulator->willReset()) {
        m_lammpsController.reset();
        atomifySimulator->lammpsState = m_lammpsController.state;
        atomifySimulator->setWillReset(false);
        atomifySimulator->scriptHandler()->setLammpsState(&atomifySimulator->lammpsState);
        atomifySimulator->clearSimulatorControls();
        emit atomifySimulator->lammpsDidReset();
    }

    atomifySimulator->scriptHandler()->setAtoms(atomifySimulator->system()->atoms());
    atomifySimulator->system()->synchronize(m_lammpsController.lammps());
    atomifySimulator->system()->atoms()->updateData();

    // Sync values from QML and simulator
    m_lammpsController.setPaused(atomifySimulator->paused());
    m_lammpsController.setSimulationSpeed(atomifySimulator->simulationSpeed());
    m_lammpsController.setSystem(atomifySimulator->system());
    QList<SimulatorControl*> controls;

    // TODO review if this can be removed
    //    for(QQuickItem* child : mySimulator->childItems()) {
    //        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
    //        if(control) {
    //            controls.append(control);
    //        }
    //    }
    for(QObject* child : atomifySimulator->children()) {
        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
        if(control) {
            if(!controls.contains(control)) {
                controls.append(control);
            }
        }
    }

    m_lammpsController.simulatorControls = controls;
    m_lammpsController.state.staticSystem = atomifySimulator->lammpsState.staticSystem;

    // Sync properties from lammps controller
    m_lammpsController.setScriptHandler(atomifySimulator->scriptHandler());

    if(m_lammpsController.crashed() && !m_lammpsController.currentException().isReported()) {
        qDebug() << "LAMMPS crashed";
        atomifySimulator->setLammpsError(QString(m_lammpsController.currentException().file().c_str()).trimmed());
        atomifySimulator->setLammpsErrorMessage(QString(m_lammpsController.currentException().error().c_str()).trimmed());
        m_lammpsController.currentException().setIsReported(true);

        emit atomifySimulator->errorInLammpsScript();
        return;
    }

    if(m_willPause) {
        m_lammpsController.setPaused(true);
        atomifySimulator->setPaused(true);
        m_willPause = false;
    }

    if(!m_lammpsController.state.runCommandActive) {
        ScriptHandler *scriptHandler = atomifySimulator->m_scriptHandler;
        ScriptParser &scriptParser = scriptHandler->parser();
        ScriptCommand nextCommandObject = scriptHandler->nextCommand();

        QString nextCommand = nextCommandObject.command();
        if(scriptParser.isEditorCommand(nextCommand) && scriptParser.isGUICommand(nextCommand)) {
            scriptHandler->parseGUICommand(nextCommand);
            m_lammpsController.state.nextCommand = ScriptCommand("", ScriptCommand::Type::SkipLammpsTick);
        } else {
            for(auto *simulatorControl : atomifySimulator->findChildren<SimulatorControl*>()) {
                simulatorControl->handleCommand(nextCommandObject.command());
            }
            m_lammpsController.state.nextCommand = nextCommandObject;
        }
    }
}

void MyWorker::work()
{
    m_lammpsController.tick();
    auto dt = m_elapsed.elapsed();
    double delta = 16 - dt;
    if(delta > 0) {
        QThread::currentThread()->msleep(delta);
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

QString AtomifySimulator::lammpsError() const
{
    return m_lammpsError;
}

QString AtomifySimulator::lammpsErrorMessage() const
{
    return m_lammpsErrorMessage;
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
    // m_system->atoms()->modifiers().append(new ColorModifier(m_system));
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

void AtomifySimulator::setLammpsError(QString lammpsError)
{
    if (m_lammpsError == lammpsError)
        return;

    m_lammpsError = lammpsError;
    emit lammpsErrorChanged(lammpsError);
}

void AtomifySimulator::setLammpsErrorMessage(QString lammpsErrorMessage)
{
    if (m_lammpsErrorMessage == lammpsErrorMessage)
        return;

    m_lammpsErrorMessage = lammpsErrorMessage;
    emit lammpsErrorMessageChanged(lammpsErrorMessage);
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
