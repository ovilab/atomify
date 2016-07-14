#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <QMap>
#include <QElapsedTimer>
#include <memory>
#include <cmath>
#include <QVector3D>
#include <mpi.h>
#include <lammps.h>

#include "lammpscontroller.h"
#include "scripthandler.h"
#include "LammpsWrappers/lammpserror.h"

using namespace LAMMPS_NS;

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    MyWorker();
    bool willPause() const;
    void setWillPause(bool willPause);

private:
    QElapsedTimer m_elapsed;
    QElapsedTimer m_sinceStart;
    LAMMPSController m_lammpsController;
    bool m_addPeriodicCopies = false;
    float m_cameraToSystemCenterDistance = 1.0;
    Atoms *m_atoms = nullptr;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void synchronizeRenderer(Renderable *renderableObject) override;
    virtual void work() override;
    void synchronizeSpheres(class Spheres *spheres);
    void synchronizeBonds(class Bonds *bonds);
    bool m_willPause = false;
};

class AtomifySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(ScriptHandler* scriptHandler READ scriptHandler WRITE setScriptHandler NOTIFY scriptHandlerChanged)
    Q_PROPERTY(bool willReset READ willReset WRITE setWillReset NOTIFY willResetChanged)
    Q_PROPERTY(LammpsError* lammpsError READ lammpsError WRITE setLammpsError NOTIFY lammpsErrorChanged)
    Q_PROPERTY(System* system READ system WRITE setSystem NOTIFY systemChanged)

public:
    AtomifySimulator();
    ~AtomifySimulator();

    // Simulator interface
    int simulationSpeed() const;
    bool paused() const;
    ScriptHandler* scriptHandler() const;
    bool willReset() const;
    Q_INVOKABLE void clearSimulatorControls();
    class System* system() const;
    LammpsError* lammpsError() const;

public slots:
    void setSimulationSpeed(int arg);
    void setPaused(bool paused);
    void setScriptHandler(ScriptHandler* scriptHandler);
    void setWillReset(bool willReset);
    void setSystem(class System* system);
    void setLammpsError(LammpsError* lammpsError);

signals:
    void simulationSpeedChanged(int arg);
    void pausedChanged(bool paused);
    void errorInLammpsScript();
    void lammpsDidReset();
    void scriptHandlerChanged(ScriptHandler* scriptHandler);
    void willResetChanged(bool willReset);
    void cameraToSystemCenterDistanceChanged(float cameraToSystemCenterDistance);
    void systemChanged(class System* system);
    void lammpsErrorChanged(LammpsError* lammpsError);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    LammpsError* m_lammpsError = nullptr;
    class System* m_system = nullptr;
    ScriptHandler* m_scriptHandler = nullptr;
    LammpsState lammpsState;
    int m_simulationSpeed = 1;
    bool m_paused = false;
    bool m_willReset = false;
};

#endif // MYSIMULATOR_H
