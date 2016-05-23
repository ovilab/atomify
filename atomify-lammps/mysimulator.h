#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <SimVis/CylinderData>
#include <QMap>
#include <QVector3D>
#include <QElapsedTimer>
#include <memory>
#include <cmath>
#include <mpi.h>
#include <lammps.h>
#include "lammpscontroller.h"

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

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void work() override;
    bool m_willPause = false;
};

class AtomifySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(QString lammpsError READ lammpsError WRITE setLammpsError NOTIFY lammpsErrorChanged)
    Q_PROPERTY(QString lammpsErrorMessage READ lammpsErrorMessage WRITE setLammpsErrorMessage NOTIFY lammpsErrorMessageChanged)
    Q_PROPERTY(ScriptHandler* scriptHandler READ scriptHandler WRITE setScriptHandler NOTIFY scriptHandlerChanged)
    Q_PROPERTY(bool willReset READ willReset WRITE setWillReset NOTIFY willResetChanged)
    Q_PROPERTY(System* system READ system WRITE setSystem NOTIFY systemChanged)
public:
    AtomifySimulator();
    ~AtomifySimulator();

    // Simulator interface
    int simulationSpeed() const;
    bool paused() const;
    QString lammpsError() const;
    QString lammpsErrorMessage() const;
    ScriptHandler* scriptHandler() const;
    bool willReset() const;
    Q_INVOKABLE void clearSimulatorControls();
    class System* system() const;

public slots:
    void setSimulationSpeed(int arg);
    void setPaused(bool paused);
    void setLammpsError(QString lammpsError);
    void setLammpsErrorMessage(QString lammpsErrorMessage);
    void setScriptHandler(ScriptHandler* scriptHandler);
    void setWillReset(bool willReset);
    void setSystem(class System* system);

signals:
    void simulationSpeedChanged(int arg);
    void pausedChanged(bool paused);
    void errorInLammpsScript();
    void lammpsErrorChanged(QString lammpsError);
    void lammpsErrorMessageChanged(QString lammpsErrorMessage);
    void lammpsDidReset();
    void scriptHandlerChanged(ScriptHandler* scriptHandler);
    void willResetChanged(bool willReset);
    void systemChanged(class System* system);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    LammpsState lammpsState;
    int m_simulationSpeed = 1;
    bool m_paused = false;
    QString m_lammpsError;
    QString m_lammpsErrorMessage;
    ScriptHandler* m_scriptHandler = nullptr;
    bool m_willReset = false;
    class System* m_system = nullptr;
    QT3D_CLONEABLE(AtomifySimulator)
};

#endif // MYSIMULATOR_H
