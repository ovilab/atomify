#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H
#include <memory>
#include <QVector>
#include <QString>
#include <QMap>
#include <mpi.h>
#include <lammps.h>
#include <compute.h>
#include <modify.h>
#include <variable.h>
#include <lammpsexception.h>
#include "LammpsWrappers/cpcompute.h"
#include "scripthandler.h"

struct LammpsState {
    bool paused = false;
    bool crashed = false;
    bool staticSystem = false;
    bool runCommandActive = false;
    bool preRunNeeded = true;
    bool dataDirty = true;
    bool canProcessSimulatorControls = false;
    bool hasExecutedRunCommand = false;
    int  simulationSpeed = 1;
    unsigned long timeSpentInLammps = 0;
    int numberOfTimesteps = 1;
    unsigned int runCommandStart = 0;
    unsigned int runCommandEnd = 0;
    unsigned int runCommandCurrent = 0;
    ScriptCommand nextCommand;
};

using namespace LAMMPS_NS;
class MyWorker;
class SimulatorControl;
class LAMMPSController
{
public:
    LammpsState state;

    LAMMPSController();
    ~LAMMPSController();

    // Getters/setters
    LAMMPS *lammps() const;
    void setLammps(LAMMPS *lammps);
    void setWorker(MyWorker *worker);
    int  simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    bool paused() const;
    void setPaused(bool value);
    bool crashed() const;
    double timePerTimestep();
    LammpsException &currentException();
    unsigned long numberOfTimesteps();
    void disableAllEnsembleFixes();
    ScriptHandler* scriptHandler() const;
    void setScriptHandler(ScriptHandler* scriptHandler);
    class System *system() const;
    void setSystem(class System *system);
    double simulationTime();

    // Actions
    void executeCommandInLAMMPS(QString command);
    void processCommand(QString command);
    void reset();
    void tick();
    int findComputeId(QString identifier);
    bool computeExists(QString identifier);
    int findFixIndex(QString identifier);
    bool fixExists(QString identifier);
    LAMMPS_NS::Compute *findComputeByIdentifier(QString identifier);
    LAMMPS_NS::Fix *findFixByIdentifier(QString identifier);
    LAMMPS_NS::Variable *findVariableByIdentifier(QString identifier);
    int findVariableIndex(QString identifier);
    template<class T>
    T *findFixByType();
private:
    ScriptHandler *m_scriptHandler = nullptr;
    LammpsException m_currentException;
    LAMMPS *m_lammps = nullptr;
    MyWorker *m_worker = nullptr;
    class System *m_system = nullptr;
    void executeActiveRunCommand();
};

template<class T>
T *LAMMPSController::findFixByType() {
    for(int i=0; i<m_lammps->modify->nfix; i++) {
        LAMMPS_NS::Fix *fix = m_lammps->modify->fix[i];

        T *myFix = dynamic_cast<T*>(fix);
        if(myFix) {
            return myFix;
        }
    }

    return nullptr;
}

#endif // LAMMPSCONTROLLER_H
