#include "mysimulator.h"
#include <library.h>
#include <atom.h>
#include <domain.h>
#include <update.h>
#include <modify.h>

#include <core/camera.h>
#include <string>
#include <sstream>
#include <SimVis/Spheres>
#include <SimVis/Points>
#include <QUrl>
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

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    MySimulator *mySimulator = qobject_cast<MySimulator*>(simulator);

    if(!mySimulator->m_scriptToRun.isEmpty()) {
        // We have a queued script, now run it
        m_lammpsController.reset();
        m_lammpsController.runScript(mySimulator->m_scriptToRun);
        mySimulator->m_scriptToRun.clear();
    }

    if(m_lammpsController.crashed() && !m_lammpsController.currentException().isReported()) {

        cout << "LAMMPS crashed and we picked it up :D" << endl;
        cout << "An error occured in " << m_lammpsController.currentException().file() << " on line " << m_lammpsController.currentException().line() << endl;
        cout << "Message: " << m_lammpsController.currentException().error() << endl;
        m_lammpsController.currentException().setIsReported(true);
    }

    if(mySimulator->atomStyle() != NULL) {
        // Sync new atom styles from Simulator (QML) to Worker
        m_atomStyle.setData(mySimulator->atomStyle()->data());
    }

    // Sync values from QML and simulator
    m_lammpsController.setComputes(mySimulator->computes());
    m_lammpsController.setPaused(mySimulator->paused());
    m_lammpsController.setSimulationSpeed(mySimulator->simulationSpeed());

    // Sync properties from lammps controller
    mySimulator->setSimulationTime(m_lammpsController.simulationTime());
    mySimulator->setNumberOfAtoms(m_lammpsController.numberOfAtoms());
    mySimulator->setNumberOfAtomTypes(m_lammpsController.numberOfAtomTypes());
    mySimulator->setSystemSize(m_lammpsController.systemSize());
    mySimulator->setLammpsOutput(&m_lammpsController.output);
    if(m_willPause) {
        m_lammpsController.setPaused(true);
        mySimulator->setPaused(true);
        m_willPause = false;
    }

    slice.distance = mySimulator->sliceDistance();
    slice.normal = mySimulator->sliceNormal();
    slice.width = mySimulator->sliceWidth();
    slice.enabled = mySimulator->sliceEnabled();
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    Spheres* spheres = qobject_cast<Spheres*>(renderableObject);
    Points* points = qobject_cast<Points*>(renderableObject);

    if(spheres) {
        LAMMPS *lammps = m_lammpsController.lammps();
        if(!lammps || !m_lammpsController.dataDirty()) return;
        m_lammpsController.setDataDirty(false);
        QVector<QVector3D> &positions = spheres->positions();
        QVector<float> &scales = spheres->scales();
        QVector<QColor> &colors = spheres->colors();
        colors.resize(lammps->atom->natoms);
        positions.resize(lammps->atom->natoms);
        m_atomTypes.resize(lammps->atom->natoms);

        double position[3];
        int visibleAtoms = 0;

        // Slice computation
        QVector3D normal = slice.normal.normalized();
        bool normalIsValid = !isnan(normal.length()); // If normal vector is 0, then this is not valid
        QVector3D planeOrigo = slice.distance*normal; // From origo (0,0,0), move along the normal a distance slice.distance

        for(unsigned int i=0; i<lammps->atom->natoms; i++) {
            position[0] = lammps->atom->x[i][0];
            position[1] = lammps->atom->x[i][1];
            position[2] = lammps->atom->x[i][2];
            lammps->domain->remap(position);

            QVector3D qPosition(position[0], position[1], position[2]);
            bool addAtom = true;
            if(slice.enabled && normalIsValid) {
                float distanceToPlane = abs(qPosition.distanceToPlane(planeOrigo, normal)); // Negative values may occu
                if(distanceToPlane > 0.5*slice.width) addAtom = false; // it is outside the slice
            }
            if(addAtom) {
                positions[visibleAtoms][0] = position[0] - lammps->domain->prd_half[0];
                positions[visibleAtoms][1] = position[1] - lammps->domain->prd_half[1];
                positions[visibleAtoms][2] = position[2] - lammps->domain->prd_half[2];
                m_atomTypes[visibleAtoms] = lammps->atom->type[i];
                visibleAtoms++;
            }
        }
        colors.resize(visibleAtoms);
        positions.resize(visibleAtoms);
        scales.resize(visibleAtoms);
        m_atomStyle.setColorsAndScales(colors, scales, m_atomTypes);
        spheres->setDirty(true);
    }


    if(points) {
        LAMMPS *lammps = m_lammpsController.lammps();
        if(!lammps || !m_lammpsController.dataDirty()) return;
        m_lammpsController.setDataDirty(false);

        QVector<QVector3D> &positions = points->positions();
        positions.resize(lammps->atom->natoms);

        double position[3];
        int visibleAtoms = 0;

        for(unsigned int i=0; i<lammps->atom->natoms; i++) {
            position[0] = lammps->atom->x[i][0];
            position[1] = lammps->atom->x[i][1];
            position[2] = lammps->atom->x[i][2];
            lammps->domain->remap(position);

            positions[visibleAtoms][0] = position[0] - lammps->domain->prd_half[0];
            positions[visibleAtoms][1] = position[1] - lammps->domain->prd_half[1];
            positions[visibleAtoms][2] = position[2] - lammps->domain->prd_half[2];
            visibleAtoms++;
        }
        positions.resize(visibleAtoms);
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


MyWorker *MySimulator::createWorker()
{
    return new MyWorker();
}
QMap<QString, CPCompute *> MySimulator::computes() const
{
    return m_computes;
}

void MySimulator::setComputes(const QMap<QString, CPCompute *> &computes)
{
    m_computes = computes;
}

void MySimulator::addCompute(CPCompute *compute)
{
    m_computes[compute->identifier()] = compute;
}

LammpsOutput *MySimulator::lammpsOutput() const
{
    return m_lammpsOutput;
}

bool MySimulator::paused() const
{
    return m_paused;
}

double MySimulator::simulationTime() const
{
    return m_simulationTime;
}

bool MySimulator::sliceEnabled() const
{
    return m_sliceEnabled;
}

double MySimulator::sliceDistance() const
{
    return m_sliceDistance;
}

QVector3D MySimulator::sliceNormal() const
{
    return m_sliceNormal;
}

double MySimulator::sliceWidth() const
{
    return m_sliceWidth;
}

AtomStyle *MySimulator::atomStyle() const
{
    return m_atomStyle;
}

int MySimulator::numberOfAtoms() const
{
    return m_numberOfAtoms;
}

int MySimulator::numberOfAtomTypes() const
{
    return m_numberOfAtomTypes;
}

QVector3D MySimulator::systemSize() const
{
    return m_systemSize;
}

void MySimulator::setLammpsOutput(LammpsOutput *lammpsOutput)
{
    if (m_lammpsOutput == lammpsOutput)
        return;

    m_lammpsOutput = lammpsOutput;
    emit lammpsOutputChanged(lammpsOutput);
}


int MySimulator::simulationSpeed() const
{
    return m_simulationSpeed;
}

void MySimulator::setSimulationSpeed(int arg)
{
    if (m_simulationSpeed == arg)
        return;

    m_simulationSpeed = arg;
    emit simulationSpeedChanged(arg);
}

void MySimulator::setPaused(bool paused)
{
    if (m_paused == paused)
        return;

    m_paused = paused;
    emit pausedChanged(paused);
}

void MySimulator::setSimulationTime(double simulationTime)
{
    if (m_simulationTime == simulationTime)
        return;

    m_simulationTime = simulationTime;
    emit simulationTimeChanged(simulationTime);
}

void MySimulator::setSliceEnabled(bool sliceEnabled)
{
    if (m_sliceEnabled == sliceEnabled)
        return;

    m_sliceEnabled = sliceEnabled;
    emit sliceEnabledChanged(sliceEnabled);
}

void MySimulator::setSliceDistance(double sliceDistance)
{
    if (m_sliceDistance == sliceDistance)
        return;

    m_sliceDistance = sliceDistance;
    emit sliceDistanceChanged(sliceDistance);
}

void MySimulator::setSliceNormal(QVector3D sliceNormal)
{
    if (m_sliceNormal == sliceNormal)
        return;
    m_sliceNormal = sliceNormal;
    emit sliceNormalChanged(sliceNormal);
}

void MySimulator::setSliceWidth(double sliceWidth)
{
    if (m_sliceWidth == sliceWidth)
        return;
    m_sliceWidth = sliceWidth;
    emit sliceWidthChanged(sliceWidth);
}

void MySimulator::setAtomStyle(AtomStyle *atomStyle)
{
    if (m_atomStyle == atomStyle)
        return;

    m_atomStyle = atomStyle;
    emit atomStyleChanged(atomStyle);
}

void MySimulator::setNumberOfAtoms(int numberOfAtoms)
{
    if (m_numberOfAtoms == numberOfAtoms)
        return;

    m_numberOfAtoms = numberOfAtoms;
    emit numberOfAtomsChanged(numberOfAtoms);
}

void MySimulator::setNumberOfAtomTypes(int numberOfAtomTypes)
{
    if (m_numberOfAtomTypes == numberOfAtomTypes)
        return;

    m_numberOfAtomTypes = numberOfAtomTypes;
    if(m_atomStyle) m_atomStyle->setMinimumSize(numberOfAtomTypes);
    emit numberOfAtomTypesChanged(numberOfAtomTypes);
}

void MySimulator::setSystemSize(QVector3D systemSize)
{
    if (m_systemSize == systemSize)
        return;

    m_systemSize = systemSize;
    emit systemSizeChanged(systemSize);
}

void MySimulator::runScript(QString script)
{
    // This is typically called from the QML thread.
    // We have to wait for synchronization before we actually load this script
    m_scriptToRun = script;
}
