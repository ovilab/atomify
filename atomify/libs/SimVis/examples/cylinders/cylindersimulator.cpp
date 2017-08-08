#include "cylindersimulator.h"
#include <QDebug>

CylinderSimulator::CylinderSimulator(QNode *parent)
    : Simulator(parent)
    , m_cylinderData(new CylinderData(this))
{

}

SimulatorWorker *CylinderSimulator::createWorker()
{
    return new CylinderWorker();
}

CylinderWorker::CylinderWorker()
{
    m_cylinders.resize(100);
    double spread = 30.0;
    for(CylinderVBOData &cylinder : m_cylinders) {
        float x = ((2.0*rand() / double(RAND_MAX))-1.0)*spread;
        float y = ((2.0*rand() / double(RAND_MAX))-1.0)*spread;
        float z = ((2.0*rand() / double(RAND_MAX))-1.0)*spread;

        QVector3D direction(((2.0*rand() / double(RAND_MAX))-1.0),
                               ((2.0*rand() / double(RAND_MAX))-1.0),
                               ((2.0*rand() / double(RAND_MAX))-1.0));
        QVector3D position(x, y, z);
        double length = 1.0 + 8.0*rand() / double(RAND_MAX);

        cylinder.radius1 = 1.0 + rand() / double(RAND_MAX);
        cylinder.radius2 = 1.0 + rand() / double(RAND_MAX);
        cylinder.vertex1 = position + direction * length * 0.5;
        cylinder.vertex2 = position - direction * length * 0.5;
    }
}

void CylinderWorker::synchronizeSimulator(Simulator *simulator)
{
    CylinderSimulator *cylinderSimulator = qobject_cast<CylinderSimulator*>(simulator);
    if(cylinderSimulator) {
        // Synchronize data between QML thread and computing thread (CylinderSimulator is on QML, CylinderWorker is computing thread).
        // This is for instance data from user through GUI (sliders, buttons, text fields etc)
        cylinderSimulator->cylinderData()->setData(m_cylinders);
    }
}

void CylinderWorker::work()
{
}
