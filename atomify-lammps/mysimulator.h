#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <QElapsedTimer>
#include <QStateMachine>
#include <QVector3D>
#include "commandparser.h"
#include "lammpscontroller.h"

using namespace LAMMPS_NS;

class MyWorker : public SimulatorWorker
{
    Q_OBJECT
public:
    MyWorker();

    void setNeedsSynchronization(bool value);
    bool needsSynchronization();
    long m_workCount = 0;
    bool m_cancelPending = false;
    bool m_reprocessRenderingData = false;
    QMutex m_workerRenderingMutex; // horrible name
private:
    QElapsedTimer m_elapsed;
    QElapsedTimer m_sinceStart;
    LAMMPSController m_lammpsController;

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
    Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)
    Q_PROPERTY(QString scriptFilePath READ scriptFilePath WRITE setScriptFilePath NOTIFY scriptFilePathChanged)
    Q_PROPERTY(QVector3D cameraPositionRequest READ cameraPositionRequest WRITE setCameraPositionRequest NOTIFY cameraPositionRequestChanged)
    Q_PROPERTY(QVector3D cameraViewCenterRequest READ cameraViewCenterRequest WRITE setCameraViewCenterRequest NOTIFY cameraViewCenterRequestChanged)
    Q_PROPERTY(bool welcomeSimulationRunning READ welcomeSimulationRunning WRITE setWelcomeSimulationRunning NOTIFY welcomeSimulationRunningChanged)
public:
    int syncCount = 0;
    AtomifySimulator();
    ~AtomifySimulator();

    int simulationSpeed() const;
    Q_INVOKABLE void togglePaused();
    Q_INVOKABLE void increaseSimulationSpeed();
    Q_INVOKABLE void decreaseSimulationSpeed();
    class System* system() const;
    class States* states() const;
    QString scriptFilePath() const;
    QString error() const;
    CommandParser &parser();
    QVector3D cameraPositionRequest() const;
    QVector3D cameraViewCenterRequest() const;
    bool welcomeSimulationRunning() const;

public slots:
    void setSimulationSpeed(int arg);
    void setSystem(class System* system);
    void setStates(class States* states);
    void setScriptFilePath(QString scriptFilePath);
    void setError(QString error);
    void setCameraPositionRequest(QVector3D cameraPositionRequest);
    void setCameraViewCenterRequest(QVector3D cameraViewCenterRequest);
    void setWelcomeSimulationRunning(bool welcomeSimulationRunning);

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
    void scriptFilePathChanged(QString scriptFilePath);
    void errorChanged(QString error);
    void cameraPositionRequestChanged(QVector3D cameraPositionRequest);
    void cameraViewCenterRequestChanged(QVector3D cameraViewCenterRequest);
    void newCameraPositionRequest(QVector3D cameraPosition);
    void newViewCenterRequest(QVector3D viewCenter);
    void newCameraPositionAndViewCenterRequest(QVector3D cameraPosition, QVector3D viewCenter);
    void welcomeSimulationRunningChanged(bool welcomeSimulationRunning);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    class System* m_system;
    class States* m_states;
    int m_simulationSpeed;
    QString m_scriptFilePath;
    QString m_error;
    CommandParser m_parser;
    QVector3D m_cameraPositionRequest;
    QVector3D m_cameraViewCenterRequest;
    bool m_welcomeSimulationRunning = true;
};

#endif // MYSIMULATOR_H
