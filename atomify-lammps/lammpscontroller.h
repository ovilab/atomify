#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H
#include <memory>
#include <QVector>
#include <QString>
#include <QMap>
#include <mpi.h>
#include <lammps.h>
#include <fix.h>
#include <compute.h>
#include <modify.h>
#include <variable.h>
#include <lammpsexception.h>
#include "scripthandler.h"
using namespace LAMMPS_NS;

struct LammpsState {
    bool paused = false;
    bool crashed = false;
    bool staticSystem = false;
    bool runCommandActive = false;
    bool preRunNeeded = true;
    int  simulationSpeed = 1;
    unsigned long timeSpentInLammps = 0;
    int numberOfTimesteps = 1;
    bool dataDirty = false;
    unsigned int runCommandStart = 0;
    unsigned int runCommandEnd = 0;
    ScriptCommand nextCommand;
};

class LAMMPSController
{
private:
    ScriptHandler *m_scriptHandler = nullptr;
    LAMMPS *m_lammps = nullptr;
    class MyWorker *m_worker = nullptr;
    class System *m_system = nullptr;
    LammpsException m_currentException;
    void executeActiveRunCommand();
public:
    LammpsState state;

    LAMMPSController();
    ~LAMMPSController();

    // Getters/setters
    QList<class SimulatorControl*> simulatorControls;
    LAMMPS *lammps() const;
    void setLammps(LAMMPS *lammps);
    void setWorker(class MyWorker *worker);
    int  simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    bool paused() const;
    void setPaused(bool value);
    bool crashed() const;
    LammpsException &currentException();
    void disableAllEnsembleFixes();
    ScriptHandler* scriptHandler() const;
    void setScriptHandler(ScriptHandler* scriptHandler);
    class System *system() const;
    void setSystem(class System *system);

    // Actions
    void executeCommandInLAMMPS(QString command);
    void processCommand(QString command);
    void reset();
    void tick();

    int findComputeId(QString identifier);
    int findVariableIndex(QString identifier);
    int findFixIndex(QString identifier);
    bool computeExists(QString identifier);
    bool fixExists(QString identifier);
    bool variableExists(QString identifier);
    LAMMPS_NS::Compute *findComputeByIdentifier(QString identifier);
    LAMMPS_NS::Fix *findFixByIdentifier(QString identifier);
    LAMMPS_NS::Variable *findVariableByIdentifier(QString identifier);
    template<class T>
    T *findFixByType();
    void processSimulatorControls();
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
