#include "marchingcubessimulator.h"
#include <SimVis/MarchingCubes>
#include <SimVis/TriangleCollection>

#include <cmath>
MyWorker::MyWorker() {

}

MarchingCubes *MySimulator::marchingCubes() const
{
    return m_marchingCubes;
}

void MySimulator::setMarchingCubes(MarchingCubes *marchingCubes)
{
    if (m_marchingCubes == marchingCubes)
        return;

    m_marchingCubes = marchingCubes;
    emit marchingCubesChanged(marchingCubes);
}

SimulatorWorker *MySimulator::createWorker() {
    return new MyWorker();
}

void MyWorker::synchronizeSimulator(Simulator *simulator) {
    MySimulator* mySimulator = qobject_cast<MySimulator*>(simulator);
    if(mySimulator) {
        m_marchingCubesGenerator.synchronize(mySimulator->marchingCubes());
    }
}

void MyWorker::work() {
    // DO WORK
    m_marchingCubesGenerator.setScalarFieldEvaluator([](const QVector3D p) {
        return sin(pow(p.x(), p.y()) + 2*p.z()) + p.x()*p.y();
        // return p.length();
    });
    m_marchingCubesGenerator.setColorEvaluator([](const QVector3D point) {
        return QVector3D(1.0, 0.0, 0.0);
    });
    m_marchingCubesGenerator.generateSurface();
}

void MyWorker::synchronizeRenderer(Renderable *renderable)
{
    if(m_marchingCubesGenerator.validSurface()) {
        TriangleCollection* triangleCollection = qobject_cast<TriangleCollection*>(renderable);
        if(triangleCollection) {
            triangleCollection->vertices = m_marchingCubesGenerator.data();
            triangleCollection->m_trianglesBack = m_marchingCubesGenerator.trianglesBack();
            triangleCollection->m_trianglesFront = m_marchingCubesGenerator.trianglesFront();
            triangleCollection->dirty = true;
        }
    }

}
