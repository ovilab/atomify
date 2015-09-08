#include "mysimulator.h"
#include "lammps/library.h"
#include "lammps/atom.h"
#include "lammps/domain.h"
#include "lammps/update.h"
#include "lammps/modify.h"
#include "physicalproperty.h"

#include <core/camera.h>
#include <string>
#include <sstream>
#include <SimVis/Spheres>
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

    if(mySimulator->atomStyle() != NULL) {
        m_atomStyle.setData(mySimulator->atomStyle()->data());
    }

    mySimulator->setSimulationTime(m_lammpsController.simulationTime());
    mySimulator->setLammpsOutput(&m_lammpsController.output);
    m_lammpsController.setComputes(mySimulator->computes());
    m_lammpsController.setPaused(mySimulator->paused());
    m_lammpsController.setSimulationSpeed(mySimulator->simulationSpeed());

    slice.distance = mySimulator->sliceDistance();
    slice.normal = mySimulator->sliceNormal();
    slice.width = mySimulator->sliceWidth();
    slice.enabled = mySimulator->sliceEnabled();
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    Spheres* spheres = qobject_cast<Spheres*>(renderableObject);
    LAMMPS *lammps = m_lammpsController.lammps();
    if(!lammps) return;

    if(spheres) {
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

void MySimulator::runScript(QString script)
{
    // This is typically called from the QML thread.
    // We have to wait for synchronization before we actually load this script
    m_scriptToRun = script;
}
