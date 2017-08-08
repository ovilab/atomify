#ifndef MARCHINGCUBESSIMULATOR_H
#define MARCHINGCUBESSIMULATOR_H

#include <SimVis/Simulator>
#include <SimVis/QuickWorker>
#include <SimVis/MarchingCubes>
#include <QVector3D>
#include <generators/marchingcubes/marchingcubesgenerator.h>

class MyWorker;
class MySimulator : public Simulator
{
    Q_OBJECT


public slots:

signals:

protected:
    SimulatorWorker *createWorker();

private:
    friend class MyWorker;
    QT3D_CLONEABLE(MySimulator)
};

class MyWorker : public QuickWorker
{
    Q_OBJECT
public:
    MyWorker();

public slots:

signals:

private:
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void work() override;
    virtual void synchronizeRenderer(Renderable *renderableObject) override;
};

#endif // MARCHINGCUBESSIMULATOR_H
