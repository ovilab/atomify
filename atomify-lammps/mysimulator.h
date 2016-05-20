#ifndef MYSIMULATOR_H
#define MYSIMULATOR_H
#include <SimVis/Simulator>
#include <SimVis/CylinderData>
#include <QMap>
#include <QElapsedTimer>
#include <memory>
#include <cmath>
#include <QVector3D>
#include <mpi.h>
#include <lammps.h>
#include "lammpscontroller.h"
#include "scripthandler.h"
#include "LammpsContainers/atoms.h"
#include "LammpsContainers/system.h"

using namespace LAMMPS_NS;
struct BondsData {
    QVector<QVector<float>> bondLengths;
    BondsData() {
        int numTypes = 10;
        bondLengths.resize(numTypes);
        for(QVector<float> &vec : bondLengths) {
            vec.resize(numTypes);
            for(float &length : vec) {
                length = 0.0;
            }
        }
    }
};

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
    QVector<CylinderVBOData> m_cylinders;
    BondsData bondsData;

    // SimulatorWorker interface
    virtual void synchronizeSimulator(Simulator *simulator) override;
    virtual void work() override;
    bool m_willPause = false;
    QVector<int> m_atomTypes;
    void synchronizePositions(AtomifySimulator *simulator);
    void updateBonds(LAMMPS *lammps);
};

class AtomifySimulator : public Simulator
{
    Q_OBJECT
    Q_PROPERTY(int simulationSpeed READ simulationSpeed WRITE setSimulationSpeed NOTIFY simulationSpeedChanged)
    Q_PROPERTY(double simulationTime READ simulationTime WRITE setSimulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(int numberOfTimesteps READ numberOfTimesteps WRITE setNumberOfTimesteps NOTIFY numberOfTimestepsChanged)
    Q_PROPERTY(int numberOfAtoms READ numberOfAtoms WRITE setNumberOfAtoms NOTIFY numberOfAtomsChanged)
    Q_PROPERTY(int numberOfAtomTypes READ numberOfAtomTypes WRITE setNumberOfAtomTypes NOTIFY numberOfAtomTypesChanged)
    Q_PROPERTY(QVector3D systemSize READ systemSize WRITE setSystemSize NOTIFY systemSizeChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(double timePerTimestep READ timePerTimestep WRITE setTimePerTimestep NOTIFY timePerTimestepChanged)
    Q_PROPERTY(QString lammpsError READ lammpsError WRITE setLammpsError NOTIFY lammpsErrorChanged)
    Q_PROPERTY(QString lammpsErrorMessage READ lammpsErrorMessage WRITE setLammpsErrorMessage NOTIFY lammpsErrorMessageChanged)
    Q_PROPERTY(ScriptHandler* scriptHandler READ scriptHandler WRITE setScriptHandler NOTIFY scriptHandlerChanged)
    Q_PROPERTY(bool willReset READ willReset WRITE setWillReset NOTIFY willResetChanged)
    Q_PROPERTY(float cameraToSystemCenterDistance READ cameraToSystemCenterDistance WRITE setCameraToSystemCenterDistance NOTIFY cameraToSystemCenterDistanceChanged)
    Q_PROPERTY(bool addPeriodicCopies READ addPeriodicCopies WRITE setAddPeriodicCopies NOTIFY addPeriodicCopiesChanged)
    Q_PROPERTY(SphereData* sphereData READ sphereData NOTIFY sphereDataChanged)
    Q_PROPERTY(CylinderData* cylinderData READ cylinderData CONSTANT)
    Q_PROPERTY(Atoms* atoms READ atoms WRITE setAtoms NOTIFY atomsChanged)
    Q_PROPERTY(System* system READ system WRITE setSystem NOTIFY systemChanged)
public:
    AtomifySimulator();
    ~AtomifySimulator();

    // Simulator interface
    int simulationSpeed() const;
    double simulationTime() const;
    bool paused() const;
    int numberOfAtoms() const;
    int numberOfAtomTypes() const;
    QVector3D systemSize() const;
    double timePerTimestep() const;
    QString lammpsError() const;
    QString lammpsErrorMessage() const;
    ScriptHandler* scriptHandler() const;
    bool willReset() const;
    int numberOfTimesteps() const;
    Q_INVOKABLE void clearSimulatorControls();
    float cameraToSystemCenterDistance() const;
    bool addPeriodicCopies() const;
    class SphereData* sphereData() const;
    class CylinderData* cylinderData() const;
    Atoms* atoms() const;
    System* system() const;

public slots:
    void setSimulationSpeed(int arg);
    void setPaused(bool paused);
    void setSimulationTime(double simulationTime);
    void setNumberOfAtoms(int numberOfAtoms);
    void setNumberOfAtomTypes(int numberOfAtomTypes);
    void setSystemSize(QVector3D systemSize);
    void setTimePerTimestep(double timePerTimestep);
    void setLammpsError(QString lammpsError);
    void setLammpsErrorMessage(QString lammpsErrorMessage);
    void setScriptHandler(ScriptHandler* scriptHandler);
    void setWillReset(bool willReset);
    void setNumberOfTimesteps(int numberOfTimesteps);
    void setCameraToSystemCenterDistance(float cameraToSystemCenterDistance);
    void setAddPeriodicCopies(bool addPeriodicCopies);

    void setAtoms(Atoms* atoms);

    void setSystem(System* system);

signals:
    void simulationSpeedChanged(int arg);
    void pausedChanged(bool paused);
    void simulationTimeChanged(double simulationTime);
    void numberOfAtomsChanged(int numberOfAtoms);
    void numberOfAtomTypesChanged(int numberOfAtomTypes);
    void systemSizeChanged(QVector3D systemSize);
    void timePerTimestepChanged(double timePerTimestep);
    void errorInLammpsScript();
    void lammpsErrorChanged(QString lammpsError);
    void lammpsErrorMessageChanged(QString lammpsErrorMessage);
    void lammpsDidReset();
    void scriptHandlerChanged(ScriptHandler* scriptHandler);
    void willResetChanged(bool willReset);
    void numberOfTimestepsChanged(int numberOfTimesteps);
    void cameraToSystemCenterDistanceChanged(float cameraToSystemCenterDistance);
    void addPeriodicCopiesChanged(bool addPeriodicCopies);
    void sphereDataChanged(SphereData* sphereData);

    void atomsChanged(Atoms* atoms);

    void systemChanged(System* system);

protected:
    virtual MyWorker *createWorker() override;

private:
    friend class MyWorker;
    LammpsState lammpsState;
    int m_simulationSpeed = 1;
    bool m_paused = false;
    double m_simulationTime = 0.0;
    AtomStyle* m_atomStyle = nullptr;
    int m_numberOfAtoms;
    int m_numberOfAtomTypes;
    QVector3D m_systemSize;
    double m_timePerTimestep = 0;
    QString m_lammpsError;
    QString m_lammpsErrorMessage;
    ScriptHandler* m_scriptHandler = nullptr;
    bool m_willReset = false;
    int m_numberOfTimesteps = 0;
    float m_cameraToSystemCenterDistance = 10;
    bool m_addPeriodicCopies = false;
    SphereData* m_sphereData = nullptr;
    CylinderData* m_cylinderData = nullptr;
    QT3D_CLONEABLE(AtomifySimulator)
    Atoms* m_atoms;
    System* m_system;
};

#endif // MYSIMULATOR_H
