#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <functional>
#include <QMap>
#include <QElapsedTimer>
#include <memory>

#include "simulations/simulations.h"
#include "lammpscontroller.h"
#include "lammpsoutput.h"
#include "mpi_stubs/mpi.h"
#include "lammps/lammps.h"
using std::function; using std::unique_ptr;

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
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
public:
    MySimulator() { }
    ~MySimulator() { }

    // Simulator interface
    int simulationSpeed() const;
    void addCompute(CPCompute *compute);
    QMap<QString, CPCompute *> computes() const;
    void setComputes(const QMap<QString, CPCompute *> &computes);
    LammpsOutput* lammpsOutput() const;
    bool paused() const;

public slots:
    void runScript(QString script);
    void setSimulationSpeed(int arg);
    void setLammpsOutput(LammpsOutput* lammpsOutput);
    void setPaused(bool paused);

signals:
    void simulationSpeedChanged(int arg);
    void lammpsOutputChanged(LammpsOutput* lammpsOutput);
    void pausedChanged(bool paused);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    int m_simulationSpeed = 1;
    bool m_paused = false;
    bool m_willLoadSimulation = false;
    QString m_scriptToRun;
    QMap<QString, CPCompute*> m_computes;
    LammpsOutput *m_lammpsOutput = NULL;
};

#endif // MYSIMULATOR_H
