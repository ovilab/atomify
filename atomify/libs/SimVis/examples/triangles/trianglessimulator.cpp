#include "trianglessimulator.h"
#include <SimVis/TriangleCollection>
#include <QDebug>
MySimulator::MySimulator()
{

}

SimulatorWorker *MySimulator::createWorker()
{
    return new MyWorker();
}

void MyWorker::synchronizeSimulator(Simulator *simulator)
{
    Q_UNUSED(simulator)
}

void MyWorker::synchronizeRenderer(Renderable *renderableObject)
{
    TriangleCollection* triangleCollection = qobject_cast<TriangleCollection*>(renderableObject);
    if(triangleCollection) {
        triangleCollection->vertices.resize(vertices.size());
        triangleCollection->dirty = true;
        for(int i=0; i<vertices.size()/3; i++) {
            QVector3D &p1 = vertices[3*i+0];
            QVector3D &p2 = vertices[3*i+1];
            QVector3D &p3 = vertices[3*i+2];
            QVector3D normal = QVector3D::crossProduct((p1-p2), (p1-p3)).normalized();
            QVector3D color(1.0, 1.0, 1.0);
            for(int j=0; j<3; j++) {
                triangleCollection->vertices[3*i+j].vertex = vertices[3*i+j];
                triangleCollection->vertices[3*i+j].color = color;
                triangleCollection->vertices[3*i+j].normal = normal;
            }
        }
        return;
    }
}

void MyWorker::work()
{

}

MyWorker::MyWorker() {
    for(int i=0; i<900; i++) {
        float x = 100*rand() / double(RAND_MAX);
        float y = 100*rand() / double(RAND_MAX);
        float z = 100*rand() / double(RAND_MAX);
        vertices.push_back(QVector3D(x,y,z));
    }
}
