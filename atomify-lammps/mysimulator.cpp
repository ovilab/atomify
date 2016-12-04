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
    m_lammpsController.system = atomifySimulator->system();
    m_lammpsController.paused = states.paused()->active();

    // If user pressed stop / restart, we should reset
    if(states.reset()->active()) {
        m_lammpsController.stop();
        atomifySimulator->system()->reset();
        emit atomifySimulator->didReset();
    }

    // If we don't have a LAMMPS object, but we have a new script (aka in parsing state), create LAMMPS object
    if(!m_lammpsController.lammps() && states.parsing()->active()) {
        m_lammpsController.start();
    }
}

void MyWorker::work()
{
    m_workCount += 1;
    m_lammpsController.locker = locker;
    m_lammpsController.tick();

    auto dt = m_elapsed.elapsed();
    double delta = 16 - dt;
    if(delta > 0) {
        QThread::currentThread()->msleep(delta);
    }
    m_elapsed.restart();
}

MyWorker *AtomifySimulator::createWorker()
{
    MyWorker *worker = new MyWorker();
    worker->locker = &m_workerMutex;
    return new MyWorker();
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

void AtomifySimulator::setSystem(System *system)
{
    if (m_system == system)
        return;

    m_system = system;
    emit systemChanged(system);
}

void AtomifySimulator::setStates(States *states)
{
    if (m_states == states)
        return;

    m_states = states;
    emit statesChanged(states);
}
