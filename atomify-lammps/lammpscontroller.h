#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H
#include <memory>
#include <QVector>
#include <QString>
#include <QMap>
#include <mpi.h>
#include <lammps.h>
#include <compute.h>
#include <lammpsexception.h>
#include "lammpsoutput.h"
#include "CPcompute.h"
#include "scripthandler.h"

using namespace LAMMPS_NS;
class MyWorker;
class LAMMPSController
{
private:
    struct State {
        bool paused = false;
        bool crashed = false;
        bool runCommandActive = false;
        bool preRunNeeded = true;
        int  simulationSpeed = 1;
        unsigned long timeSpentInLammps = 0;
        int numberOfTimesteps = 1;
        bool dataDirty = false;
        unsigned int runCommandStart = 0;
        unsigned int runCommandEnd = 0;
    };
    ScriptHandler m_scriptHandler;
    QMap<QString, CPCompute*> m_computes;
    LammpsException m_currentException;
    LAMMPS *m_lammps = NULL;
    MyWorker *m_worker = NULL;

    void processComputes();
    void executeActiveRunCommand();
    int findComputeId(QString identifier);
    bool computeExists(QString identifier);
    int findFixId(QString identifier);
    bool fixExists(QString identifier);
    LAMMPS_NS::Compute *findCompute(QString identifier);
    LAMMPS_NS::Fix *findFix(QString identifier);

public:
    LammpsOutput output;
    State m_state;

    LAMMPSController();
    ~LAMMPSController();

    // Getters/setters
    LAMMPS *lammps() const;
    void setLammps(LAMMPS *lammps);
    ScriptHandler *scriptHandler();
    void setWorker(MyWorker *worker);
    int  simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    QMap<QString, CPCompute *> computes() const;
    void setComputes(const QMap<QString, CPCompute *> &computes);
    bool paused() const;
    void setPaused(bool value);
    bool dataDirty() const;
    void setDataDirty(bool value);
    bool crashed() const;
    double timePerTimestep();
    LammpsException &currentException();
    double simulationTime();
    int numberOfAtoms() const;
    int numberOfAtomTypes() const;
    void disableAllEnsembleFixes();
    QVector3D systemSize() const;
    // Actions
    void executeCommandInLAMMPS(QString command);
    void processCommand(QString command);
    void reset();
    void tick();
};

#endif // LAMMPSCONTROLLER_H
