#include "spheresimulator.h"
#include <QDebug>
#include <SimVis/SphereData>
NoiseSimulator::NoiseSimulator(QNode *parent)
    : Simulator(parent)
    , m_sphereData(new SphereData(this))
{
}

double NoiseSimulator::dt() const
{
    return m_dt;
}

void NoiseSimulator::setDt(double dt)
{
    if (m_dt == dt)
        return;

    m_dt = dt;
    emit dtChanged(dt);
}

SimulatorWorker *NoiseSimulator::createWorker()
{
    return new NoiseWorker();
}

NoiseWorker::NoiseWorker()
{
    m_positions.resize(5000);
    m_velocities.resize(m_positions.size());
    double size = 10;
    for(int i=0; i<m_positions.size(); i++) {
        float x = ((2.0*rand() / double(RAND_MAX))-1.0)*size;
        float y = ((2.0*rand() / double(RAND_MAX))-1.0)*size;
        float z = ((2.0*rand() / double(RAND_MAX))-1.0)*size;
        float vx = ((2.0*rand() / double(RAND_MAX))-1.0);
        float vy = ((2.0*rand() / double(RAND_MAX))-1.0);
        float vz = ((2.0*rand() / double(RAND_MAX))-1.0);
        QVector3D pos(x,y,z);
        m_positions[i] = pos + pos.normalized() * 5;
        m_velocities[i] = QVector3D(vx,vy,vz);
    }
}

void NoiseWorker::synchronizeSimulator(Simulator *simulator)
{
    NoiseSimulator *mySimulator = qobject_cast<NoiseSimulator*>(simulator);
    if(mySimulator) {
        // Synchronize data between QML thread and computing thread (MySimulator is on QML, MyWorker is computing thread).
        // This is for instance data from user through GUI (sliders, buttons, text fields etc)
        dt = mySimulator->dt();

        mySimulator->sphereData()->setPositions(m_positions);
    }
}

void NoiseWorker::work()
{
    for(int i=0; i<m_positions.size(); i++) {
        float ax = ((2.0*rand() / double(RAND_MAX))-1.0);
        float ay = ((2.0*rand() / double(RAND_MAX))-1.0);
        float az = ((2.0*rand() / double(RAND_MAX))-1.0);
        m_velocities[i][0] += ax*dt;
        m_velocities[i][1] += ay*dt;
        m_velocities[i][2] += az*dt;

        m_positions[i] += m_velocities[i]*dt;
    }
}
