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
}

void AtomifySimulator::addSimulatorControl(SimulatorControl *simulatorControl)
{
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

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    AtomifySimulator *atomifySimulator = qobject_cast<AtomifySimulator*>(simulator);

    // Sync properties from lammps controller and back
    atomifySimulator->scriptHandler()->setAtoms(atomifySimulator->system()->atoms());
    m_lammpsController.setScriptHandler(atomifySimulator->scriptHandler());
    m_lammpsController.setPaused(atomifySimulator->paused());
    m_lammpsController.setSimulationSpeed(atomifySimulator->simulationSpeed());
    m_lammpsController.setSystem(atomifySimulator->system());
    m_lammpsController.state.staticSystem = atomifySimulator->lammpsState.staticSystem;
    m_atoms = atomifySimulator->system()->atoms();

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
        atomifySimulator->clearSimulatorControls();
        atomifySimulator->system()->reset();
        atomifySimulator->setLammpsError(nullptr);
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

//    for(QObject* child : mySimulator->children()) {
//        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
//        if(control) {
//            if(!controls.contains(control)) {
//                controls.append(control);
//            }
//        }
//    }

//    QMap<QString, SimulatorControl*> controls;
//    for(QObject* child : mySimulator->children()) {
//        SimulatorControl* control = qobject_cast<SimulatorControl*>(child);
//        if(control) {
//            if(!controls.contains(control->identifier())) {
//                controls.insert(control->identifier(), control);
//            }
//        }
//    }
//    m_lammpsController.simulatorControls = controls; // This object is visible from the Computes class
    if(m_lammpsController.state.canProcessSimulatorControls) {
        foreach(SimulatorControl *control, mySimulator->m_simulatorControls) {
            control->update(&m_lammpsController);
        }
    }

        foreach(SimulatorControl *control, controls) {
            control->update(&m_lammpsController);
        }
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

}

void MyWorker::synchronizeSpheres(Spheres *spheres) {
    spheres->setData(m_atoms->spheresData());
    spheres->setDirty(true);
//    spheres
//    LAMMPS *lammps = m_lammpsController.lammps();
//    if(spheres->camera()) {
//        m_cameraToSystemCenterDistance = (spheres->camera()->position() - m_lammpsController.systemCenter()).length();
//    }
//    QVector<QVector3D> &positions = spheres->positions();
//    QVector<float> &scales = spheres->scales();
//    QVector<QColor> &colors = spheres->colors();

//    if(m_addPeriodicCopies) {
//        // Each atom will have 27 copies
//        positions.resize(27*lammps->atom->natoms);
//        m_atomTypes.resize(27*lammps->atom->natoms);
//    } else {
//        positions.resize(lammps->atom->natoms);
//        m_atomTypes.resize(lammps->atom->natoms);
//    }
//    double position[3];
//    QList<QObject *> atomStyleDataList = m_atomStyle.data();
//    int numVisibleAtoms = 0;
//    for(unsigned int i=0; i<lammps->atom->natoms; i++) {
//        bool addAtom = true;
//        int atomType = lammps->atom->type[i];

//        if(atomType-1 < atomStyleDataList.size()) {
//            // If not, we haven't added this atom to the list yet. Skip this atom type then
//            AtomStyleData *atomStyleData = qobject_cast<AtomStyleData*>(atomStyleDataList[atomType-1]); // LAMMPS atom types start at 1
//            if(!atomStyleData->visible()) addAtom = false;
//        } else addAtom = false;

//        if(addAtom) {
//            position[0] = lammps->atom->x[i][0];
//            position[1] = lammps->atom->x[i][1];
//            position[2] = lammps->atom->x[i][2];
//            lammps->domain->remap(position);
//            position[0] -= lammps->domain->boxlo[0] + lammps->domain->prd_half[0];
//            position[1] -= lammps->domain->boxlo[1] + lammps->domain->prd_half[1];
//            position[2] -= lammps->domain->boxlo[2] + lammps->domain->prd_half[2];

//            if(spheres->camera() && m_addPeriodicCopies) {
//                QVector3D systemSize = m_lammpsController.systemSize();
//                for(int dx=-1; dx<=1; dx++) {
//                    for(int dy=-1; dy<=1; dy++) {
//                        for(int dz=-1; dz<=1; dz++) {
//                            QVector3D pos = QVector3D(position[0], position[1], position[2]) + QVector3D(dx*systemSize[0], dy*systemSize[1], dz*systemSize[2]);
//                            QVector3D cameraToPos = pos - spheres->camera()->position();
//                            if(QVector3D::dotProduct(cameraToPos, spheres->camera()->viewVector()) < 0) continue;

//                            positions[numVisibleAtoms][0] = pos[0];
//                            positions[numVisibleAtoms][1] = pos[1];
//                            positions[numVisibleAtoms][2] = pos[2];
//                            m_atomTypes[numVisibleAtoms] = atomType;
//                            numVisibleAtoms++;
//                        }
//                    }
//                }
//            } else {
//                positions[numVisibleAtoms][0] = position[0];
//                positions[numVisibleAtoms][1] = position[1];
//                positions[numVisibleAtoms][2] = position[2];
//                m_atomTypes[numVisibleAtoms] = atomType;
//                numVisibleAtoms++;
//            }
//        }
//    }
//    colors.resize(numVisibleAtoms);
//    scales.resize(numVisibleAtoms);
//    positions.resize(numVisibleAtoms);
//    m_atomTypes.resize(numVisibleAtoms);
//    m_atomStyle.setColorsAndScales(colors, scales, m_atomTypes);
//    spheres->setDirty(true);
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
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
