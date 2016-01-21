#include "mysimulator.h"
#include "simulatorcontrol.h"
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

using namespace std;

MyWorker::MyWorker() {
    m_sinceStart.start();
    m_elapsed.start();
    m_lammpsController.setWorker(this);
}

void AtomifySimulator::clearSimulatorControls()
{
    for(QQuickItem* child : childItems()) {
        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
        if(control) {
            control->setParentItem(nullptr);
        }
    }
}

float AtomifySimulator::cameraToSystemCenterDistance() const
{
    return m_cameraToSystemCenterDistance;
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    AtomifySimulator *mySimulator = qobject_cast<AtomifySimulator*>(simulator);
    if(mySimulator->scriptHandler() == nullptr) {
        return;
    }

    if(mySimulator->willReset()) {
        m_lammpsController.reset();
        mySimulator->lammpsState = m_lammpsController.state;
        mySimulator->setWillReset(false);
        mySimulator->scriptHandler()->setLammpsState(&mySimulator->lammpsState);
        emit mySimulator->lammpsDidReset();
    }

    if(mySimulator->atomStyle() != nullptr) {
        // Sync new atom styles from Simulator (QML) to Worker
        if(mySimulator->atomStyle()->dirty()) {
            m_atomStyle.setData(mySimulator->atomStyle()->data());
            m_atomStyle.setDirty(mySimulator->atomStyle()->dirty());
            mySimulator->atomStyle()->setDirty(false);
        }
    }

    // Sync values from QML and simulator
    m_lammpsController.setPaused(mySimulator->paused());
    m_lammpsController.setSimulationSpeed(mySimulator->simulationSpeed());
    QList<SimulatorControl*> controls;
    for(QQuickItem* child : mySimulator->childItems()) {
        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
        if(control) {
            controls.append(control);
        }
    }

//    for(QObject* child : mySimulator->children()) {
//        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
//        if(control) {
//            controls.append(control);
//        }
//    }

    m_lammpsController.simulatorControls = controls;
    m_lammpsController.state.staticSystem = mySimulator->lammpsState.staticSystem;

    // Sync properties from lammps controller
    mySimulator->setNumberOfTimesteps(m_lammpsController.numberOfTimesteps());
    mySimulator->setSimulationTime(m_lammpsController.simulationTime());
    mySimulator->setNumberOfAtoms(m_lammpsController.numberOfAtoms());
    mySimulator->setNumberOfAtomTypes(m_lammpsController.numberOfAtomTypes());
    mySimulator->setSystemSize(m_lammpsController.systemSize());
    mySimulator->setTimePerTimestep(m_lammpsController.timePerTimestep());
    qDebug() << "Time per timestep: " << m_lammpsController.timePerTimestep();
    mySimulator->setCameraToSystemCenterDistance(m_cameraToSystemCenterDistance);

    m_lammpsController.setScriptHandler(mySimulator->scriptHandler());

    if(m_lammpsController.crashed() && !m_lammpsController.currentException().isReported()) {
        qDebug() << "LAMMPS crashed";
        mySimulator->setLammpsError(QString(m_lammpsController.currentException().file().c_str()).trimmed());
        mySimulator->setLammpsErrorMessage(QString(m_lammpsController.currentException().error().c_str()).trimmed());
        m_lammpsController.currentException().setIsReported(true);

        emit mySimulator->errorInLammpsScript();
        return;
    }

    if(m_willPause) {
        m_lammpsController.setPaused(true);
        mySimulator->setPaused(true);
        m_willPause = false;
    }

    if(!m_lammpsController.state.runCommandActive) {
        ScriptHandler *scriptHandler = mySimulator->m_scriptHandler;
        ScriptParser &scriptParser = scriptHandler->parser();
        ScriptCommand nextCommandObject = scriptHandler->nextCommand();

        QString nextCommand = nextCommandObject.command();
        if(scriptParser.isEditorCommand(nextCommand) && scriptParser.isGUICommand(nextCommand)) {
            scriptHandler->parseGUICommand(nextCommand);
            m_lammpsController.state.nextCommand = ScriptCommand("", ScriptCommand::Type::SkipLammpsTick);
        } else {
            for(auto *simulatorControl : mySimulator->findChildren<SimulatorControl*>()) {
                simulatorControl->handleCommand(nextCommandObject.command());
            }
            m_lammpsController.state.nextCommand = nextCommandObject;
        }
    }
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    Spheres* spheres = qobject_cast<Spheres*>(renderableObject);
    LAMMPS *lammps = m_lammpsController.lammps();
    if(!lammps) return;
    //    if(!m_lammpsController.dataDirty() && !m_atomStyle.dirty()) return;
    m_lammpsController.setDataDirty(false);
    m_atomStyle.setDirty(false);
    if(spheres) {
        if(spheres->camera()) {
            QVector3D systemCenter(lammps->domain->prd_half[0], lammps->domain->prd_half[1], lammps->domain->prd_half[2]);
            m_cameraToSystemCenterDistance = (spheres->camera()->position() - systemCenter).length();
        }
        QVector<QVector3D> &positions = spheres->positions();
        QVector<float> &scales = spheres->scales();
        QVector<QColor> &colors = spheres->colors();

        colors.resize(lammps->atom->natoms);
        scales.resize(lammps->atom->natoms);
        positions.resize(lammps->atom->natoms);
        m_atomTypes.resize(lammps->atom->natoms);
        double position[3];
        QList<QObject *> atomStyleDataList = m_atomStyle.data();
        int numVisibleAtoms = 0;
        for(unsigned int i=0; i<lammps->atom->natoms; i++) {
            bool addAtom = true;
            int atomType = lammps->atom->type[i];

            if(atomType-1 < atomStyleDataList.size()) {
                // If not, we haven't added this atom to the list yet. Skip this atom type then
                AtomStyleData *atomStyleData = qobject_cast<AtomStyleData*>(atomStyleDataList[atomType-1]); // LAMMPS atom types start at 1
                if(!atomStyleData->visible()) addAtom = false;
            } else addAtom = false;

            if(addAtom) {
                position[0] = lammps->atom->x[i][0];
                position[1] = lammps->atom->x[i][1];
                position[2] = lammps->atom->x[i][2];
                lammps->domain->remap(position);

                positions[numVisibleAtoms][0] = position[0] - lammps->domain->boxlo[0] - lammps->domain->prd_half[0];
                positions[numVisibleAtoms][1] = position[1] - lammps->domain->boxlo[1] - lammps->domain->prd_half[1];
                positions[numVisibleAtoms][2] = position[2] - lammps->domain->boxlo[2] - lammps->domain->prd_half[2];
                m_atomTypes[numVisibleAtoms] = atomType;
                numVisibleAtoms++;
            }
        }

        colors.resize(numVisibleAtoms);
        scales.resize(numVisibleAtoms);
        positions.resize(numVisibleAtoms);
        m_atomTypes.resize(numVisibleAtoms);
        m_atomStyle.setColorsAndScales(colors, scales, m_atomTypes);
        spheres->setDirty(true);
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

double AtomifySimulator::simulationTime() const
{
    return m_simulationTime;
}

AtomStyle *AtomifySimulator::atomStyle() const
{
    return m_atomStyle;
}

int AtomifySimulator::numberOfAtoms() const
{
    return m_numberOfAtoms;
}

int AtomifySimulator::numberOfAtomTypes() const
{
    return m_numberOfAtomTypes;
}

QVector3D AtomifySimulator::systemSize() const
{
    return m_systemSize;
}

double AtomifySimulator::timePerTimestep() const
{
    return m_timePerTimestep;
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

int AtomifySimulator::numberOfTimesteps() const
{
    return m_numberOfTimesteps;
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

void AtomifySimulator::setPaused(bool paused)
{
    if (m_paused == paused)
        return;

    m_paused = paused;
    emit pausedChanged(paused);
}

void AtomifySimulator::setSimulationTime(double simulationTime)
{
    if (m_simulationTime == simulationTime)
        return;

    m_simulationTime = simulationTime;
    emit simulationTimeChanged(simulationTime);
}

void AtomifySimulator::setAtomStyle(AtomStyle *atomStyle)
{
    if (m_atomStyle == atomStyle)
        return;

    m_atomStyle = atomStyle;
    emit atomStyleChanged(atomStyle);
}

void AtomifySimulator::setNumberOfAtoms(int numberOfAtoms)
{
    if (m_numberOfAtoms == numberOfAtoms)
        return;

    m_numberOfAtoms = numberOfAtoms;
    emit numberOfAtomsChanged(numberOfAtoms);
}

void AtomifySimulator::setNumberOfAtomTypes(int numberOfAtomTypes)
{
    if (m_numberOfAtomTypes == numberOfAtomTypes)
        return;

    m_numberOfAtomTypes = numberOfAtomTypes;
    if(m_atomStyle) m_atomStyle->setMinimumSize(numberOfAtomTypes);
    emit numberOfAtomTypesChanged(numberOfAtomTypes);
}

void AtomifySimulator::setSystemSize(QVector3D systemSize)
{
    if (m_systemSize == systemSize)
        return;

    m_systemSize = systemSize;
    emit systemSizeChanged(systemSize);
}

void AtomifySimulator::setTimePerTimestep(double timePerTimestep)
{
    if (m_timePerTimestep == timePerTimestep)
        return;

    m_timePerTimestep = timePerTimestep;
    emit timePerTimestepChanged(timePerTimestep);
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

void AtomifySimulator::setNumberOfTimesteps(int numberOfTimesteps)
{
    if (m_numberOfTimesteps == numberOfTimesteps)
        return;

    m_numberOfTimesteps = numberOfTimesteps;
    emit numberOfTimestepsChanged(numberOfTimesteps);
}

void AtomifySimulator::setCameraToSystemCenterDistance(float cameraToSystemCenterDistance)
{
    if (m_cameraToSystemCenterDistance == cameraToSystemCenterDistance)
        return;

    m_cameraToSystemCenterDistance = cameraToSystemCenterDistance;
    emit cameraToSystemCenterDistanceChanged(cameraToSystemCenterDistance);
}
