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

void MyWorker::loadSimulation(QString simulationId) {
    auto simulation = m_simulations.find(simulationId);
    if(simulation == m_simulations.end()) {
        qDebug() << "Warning, tried to load simulation "+simulationId+" which does not exist in simulation list.";
        return;
    }
    m_currentSimulation = simulation.value();
    m_lammpsController.reset();
    m_lammpsController.loadScriptFromFile(m_currentSimulation->inputScriptFile());
}

MyWorker::MyWorker() {
    m_simulations = createSimulationObjects();
    loadSimulation("lennardjonesdiffusion");
    // loadSimulation("obstacle");
    m_sinceStart.start();
    m_elapsed.start();
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    MySimulator *mySimulator = qobject_cast<MySimulator*>(simulator);
    m_lammpsController.setSimulationSpeed(mySimulator->simulationSpeed());

    if(!mySimulator->m_simulationIdToLoad.isEmpty()) {
        loadSimulation(mySimulator->m_simulationIdToLoad);
        mySimulator->m_simulationIdToLoad.clear();
        mySimulator->setNewCameraPosition(m_currentSimulation->initialCameraPosition());
    }

    if(!mySimulator->m_scriptToRun.isEmpty()) {
        m_lammpsController.reset();
        m_lammpsController.runScript(mySimulator->m_scriptToRun);
        mySimulator->m_scriptToRun.clear();
    }

    if(mySimulator->lammpsOutput() != NULL) {
        m_lammpsController.setOutputParser(mySimulator->lammpsOutput());
    }

    m_lammpsController.setComputes(mySimulator->computes());
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    Spheres* spheres = qobject_cast<Spheres*>(renderableObject);
    LAMMPS *lammps = m_lammpsController.lammps();

    if(spheres) {
        QVector<QVector3D> &positions = spheres->positions();
        QVector<float> &scales = spheres->scales();
        QVector<QColor> &colors = spheres->colors();
        colors.resize(lammps->atom->natoms);

        positions.resize(lammps->atom->natoms);
        double position[3];
        QVector3D deltaPosition = m_currentSimulation->positionOffset();
        for(unsigned int i=0; i<lammps->atom->natoms; i++) {
            position[0] = lammps->atom->x[i][0];
            position[1] = lammps->atom->x[i][1];
            position[2] = lammps->atom->x[i][2];
            lammps->domain->remap(position);

            positions[i][0] = position[0] - lammps->domain->prd_half[0] + deltaPosition[0];
            positions[i][1] = position[1] - lammps->domain->prd_half[1] + deltaPosition[1];
            positions[i][2] = position[2] - lammps->domain->prd_half[2] + deltaPosition[2];
        }

        m_currentSimulation->scaleAndColorEvaluator()(colors, scales, lammps);

        return;
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

LammpsOutput *MySimulator::lammpsOutput() const
{
    return m_lammpsOutput;
}

MySimulator::MySimulator()
{

}

QVector3D MySimulator::newCameraPosition() const
{
    return m_newCameraPosition;
}

int MySimulator::simulationSpeed() const
{
    return m_simulationSpeed;
}

void MySimulator::addCompute(CPCompute *compute)
{
    qDebug() << "Adding compute to MySimulator";
    m_computes[compute->identifier()] = compute;
}

void MySimulator::loadSimulation(QString simulationId)
{
    m_simulationIdToLoad = simulationId;
}

void MySimulator::setNewCameraPosition(QVector3D arg)
{
    m_newCameraPosition = arg;
    emit newCameraPositionChanged(arg);
}

void MySimulator::setSimulationSpeed(int arg)
{
    if (m_simulationSpeed == arg)
        return;

    m_simulationSpeed = arg;
    emit simulationSpeedChanged(arg);
}

void MySimulator::setLammpsOutput(LammpsOutput *lammpsOutput)
{
    if (m_lammpsOutput == lammpsOutput)
        return;

    m_lammpsOutput = lammpsOutput;
    emit lammpsOutputChanged(lammpsOutput);
}

void MySimulator::runScript(QString script)
{
    m_scriptToRun = script;
}
