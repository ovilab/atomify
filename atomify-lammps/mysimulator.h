#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <functional>
#include <QMap>
#include <QElapsedTimer>
#include "simulations/simulations.h"
#include "lammpscontroller.h"
#include "mpi_stubs/mpi.h"
#include "lammps/lammps.h"
using std::function;
using namespace LAMMPS_NS;

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    MyWorker();
private:
    QElapsedTimer m_elapsed;
    QElapsedTimer m_sinceStart;
    LAMMPSController m_lammpsController;
    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void synchronizeRenderer(Renderable *renderableObject) override;
    virtual void work() override;
    Simulation *m_currentSimulation = 0;
    QMap<QString, Simulation*> m_simulations;
    void loadSimulation(QString simulationId);
};

class MySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(QVector3D newCameraPosition READ newCameraPosition WRITE setNewCameraPosition NOTIFY newCameraPositionChanged)

public:
    MySimulator();
    ~MySimulator();

    // Simulator interface
    QVector3D newCameraPosition() const;
    int simulationSpeed() const;

public slots:
    void loadSimulation(QString simulationId);
    void setNewCameraPosition(QVector3D arg);
    void setSimulationSpeed(int arg);

signals:
    void newCameraPositionChanged(QVector3D arg);
    void simulationSpeedChanged(int arg);

protected:
    bool m_willLoadSimulation = false;
    QString m_simulationIdToLoad;

    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    QVector3D m_newCameraPosition;
    int m_simulationSpeed;
};

#endif // MYSIMULATOR_H
