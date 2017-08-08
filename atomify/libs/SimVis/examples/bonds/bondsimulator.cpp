#include "bondsimulator.h"
#include <QDebug>
#include <SimVis/SphereData>
#include <SimVis/BondData>

BondSimulator::BondSimulator(QNode *parent)
    : Simulator(parent)
    , m_sphereData(new SphereData(this))
    , m_bondData(new BondData(this))
{
}

double BondSimulator::dt() const
{
    return m_dt;
}

SphereData *BondSimulator::sphereData()
{
    return m_sphereData.data();
}

BondData *BondSimulator::bondData()
{
    return m_bondData.data();
}

float BondSimulator::radius1() const
{
    return m_radius1;
}

float BondSimulator::radius2() const
{
    return m_radius2;
}

void BondSimulator::setDt(double dt)
{
    if (m_dt == dt)
        return;

    m_dt = dt;
    emit dtChanged(dt);
}

void BondSimulator::setRadius1(float radius1)
{
    if (m_radius1 == radius1)
        return;

    m_radius1 = radius1;
    emit radius1Changed(radius1);
}

void BondSimulator::setRadius2(float radius2)
{
    if (m_radius2 == radius2)
        return;

    m_radius2 = radius2;
    emit radius2Changed(radius2);
}

SimulatorWorker *BondSimulator::createWorker()
{
    return new BondWorker();
}

BondWorker::BondWorker()
{
    SphereVBOData sphere1;
    sphere1.position = QVector3D(4, 0, 0);
    sphere1.radius = 1.0;
    sphere1.color = QVector3D(1.0, 0.0, 0.0);

    SphereVBOData sphere2;
    sphere2.position = QVector3D(-4, -2, 0);
    sphere2.radius = 1.0;
    sphere2.color = QVector3D(0.0, 1.0, 0.0);
    
    BondVBOData bond;
    bond.vertex1 = sphere1.position;
    bond.vertex2 = sphere2.position;
    bond.radius1 = 0.2;
    bond.radius2 = 0.2;
    bond.sphereRadius1 = sphere1.radius;
    bond.sphereRadius2 = sphere2.radius;

    m_bonds.push_back(bond);
    m_spheres.push_back(sphere1);
    m_spheres.push_back(sphere2);
}

void BondWorker::synchronizeSimulator(Simulator *simulator)
{
    BondSimulator *mySimulator = qobject_cast<BondSimulator*>(simulator);
    if(mySimulator) {
        // Synchronize data between QML thread and computing thread (MySimulator is on QML, MyWorker is computing thread).
        // This is for instance data from user through GUI (sliders, buttons, text fields etc)
        dt = mySimulator->dt();
        m_spheres[0].radius = mySimulator->radius1();
        m_spheres[1].radius = mySimulator->radius2();
        m_bonds[0].sphereRadius1 = mySimulator->radius1();
        m_bonds[0].sphereRadius2 = mySimulator->radius2();

        mySimulator->sphereData()->setData(m_spheres);
        mySimulator->bondData()->setData(m_bonds);
    }
}

void BondWorker::work()
{

}
