#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <QElapsedTimer>
#include <QStateMachine>
#include "lammpscontroller.h"

using namespace LAMMPS_NS;

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    MyWorker();
    QMutex *locker = nullptr;
    bool willPause() const;
    void setWillPause(bool willPause);

private:
    QElapsedTimer m_elapsed;
    QElapsedTimer m_sinceStart;
    LAMMPSController m_lammpsController;
    long m_workCount = 0;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void work() override;
};

class AtomifySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(System* system READ system WRITE setSystem NOTIFY systemChanged)
    Q_PROPERTY(States* states READ states WRITE setStates NOTIFY statesChanged)

public:
    AtomifySimulator();
    ~AtomifySimulator() { }

    int simulationSpeed() const;
    Q_INVOKABLE void togglePaused();
    class System* system() const;
    class States* states() const;

public slots:
    void setSimulationSpeed(int arg);
    void setSystem(class System* system);
    void setStates(class States* states);

signals:
    void simulationSpeedChanged(int arg);
    void systemChanged(class System* system);
    void statesChanged(class States* states);

    // State changes
    void reset();
    void didReset();
    void continued();
    void started();
    void crashed();
    void finished();
    void parsing();
    void paused();
    void unPaused();

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    class System* m_system;
    class States* m_states;
    int m_simulationSpeed;
};

#endif // MYSIMULATOR_H
