#include "mysimulator.h"
#include "lammps/library.h"
#include "lammps/atom.h"
#include "lammps/domain.h"
#include "lammps/update.h"
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
#include <QStandardPaths>

using std::string;

void MyWorker::loadSimulation(QString simulationId) {
    auto simulation = m_simulations.find(simulationId);
    if(simulation == m_simulations.end()) {
        qDebug() << "Warning, tried to load simulation "+simulationId+" which does not exist in simulation list.";
        return;
    }
    m_currentSimulation = simulation.value();

    // Remove old LAMMPS simulator and prepare new, clean simulator
    if(lammps) {
        lammps_close((void*)lammps);
        lammps = 0;
    }
    lammps_open_no_mpi(0, 0, (void**)&lammps);

    m_currentSimulation->runLammpsScript(lammps);
}

MyWorker::MyWorker() {
    m_simulations = createSimulationObjects();

    loadSimulation("lennardjonesdiffusion");
}

void MyWorker::runCommands(const char *commands) {
    std::stringstream ss(commands);
    std::string to;

    if (commands != NULL)
    {
        while(std::getline(ss,to,'\n')){
            runCommand(to.c_str());
        }
    }
}

void MyWorker::runCommand(const char *command)
{
    if(lammps == 0) {
        qDebug() << "Warning, trying to run a LAMMPS command with no LAMMPS object. You need to load a simulation first.";
        qDebug() << "Command: " << command;
        return;
    }

    lammps_command((void*)lammps, (char*) command);
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    MySimulator *mySimulator = qobject_cast<MySimulator*>(simulator);
    m_discoMode = mySimulator->discoMode();
    m_simulationSpeed = mySimulator->simulationSpeed();

    if(!mySimulator->m_simulationIdToLoad.isEmpty()) {
        loadSimulation(mySimulator->m_simulationIdToLoad);
        mySimulator->m_simulationIdToLoad.clear();
        mySimulator->setNewCameraPosition(m_currentSimulation->initialCameraPosition());
    }
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    Spheres* spheres = qobject_cast<Spheres*>(renderableObject);
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

//        qDebug() << "Camera position: " << spheres->camera()->position();
//        qDebug() << "Camera view vector: " << spheres->camera()->viewVector();
//        qDebug() << "Camera view center: " << spheres->camera()->viewCenter();
//        qDebug() << "Camera upvector: " << spheres->camera()->upVector();

        return;
    }
}

void MyWorker::work()
{
    runCommand(QString("run %1 pre no post no").arg(m_simulationSpeed).toStdString().c_str());
}

MyWorker *MySimulator::createWorker()
{
    return new MyWorker();
}

MySimulator::MySimulator()
{

}

MySimulator::~MySimulator()
{

}

bool MySimulator::discoMode() const
{
    return m_discoMode;
}

int MySimulator::simulationSpeed() const
{
    return m_simulationSpeed;
}

QVector3D MySimulator::newCameraPosition() const
{
    return m_newCameraPosition;
}

void MySimulator::setDiscoMode(bool arg)
{
    if (m_discoMode == arg)
        return;

    m_discoMode = arg;
    emit discoModeChanged(arg);
}

void MySimulator::loadSimulation(QString simulationId)
{
    m_simulationIdToLoad = simulationId;
}

void MySimulator::setSimulationSpeed(int arg)
{
    if (m_simulationSpeed == arg)
        return;

    m_simulationSpeed = arg;
    emit simulationSpeedChanged(arg);
}

void MySimulator::setNewCameraPosition(QVector3D arg)
{
    m_newCameraPosition = arg;
    emit newCameraPositionChanged(arg);
}
