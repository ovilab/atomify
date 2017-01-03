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
    Q_PROPERTY(MarchingCubes* marchingCubes READ marchingCubes WRITE setMarchingCubes NOTIFY marchingCubesChanged)

public:
    MarchingCubes* marchingCubes() const;

public slots:
    void setMarchingCubes(MarchingCubes* marchingCubes);

signals:
    void marchingCubesChanged(MarchingCubes* marchingCubes);

protected:
    SimulatorWorker *createWorker();

private:
    friend class MyWorker;
    MarchingCubes* m_marchingCubes = nullptr;
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
    MarchingCubesGenerator m_marchingCubesGenerator;
};

#endif // MARCHINGCUBESSIMULATOR_H
