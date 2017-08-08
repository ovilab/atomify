#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H

#include <QVector>
#include <QVector3D>
#include <SimVis/Simulator>

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    MyWorker();
    QVector<QVector3D> vertices;
private:
    virtual void synchronizeSimulator(Simulator *simulator);
    virtual void synchronizeRenderer(Renderable *renderableObject);
    virtual void work();
};

class MySimulator : public Simulator
{
    Q_OBJECT
public:
    MySimulator();

    // Simulator interface
protected:
    virtual SimulatorWorker *createWorker();
};

#endif // MYSIMULATOR_H
