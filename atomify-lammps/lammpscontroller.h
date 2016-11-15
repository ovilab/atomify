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
#include <error.h>
#include "parser/scriptcommand.h"

class LAMMPSController
{
private:
    bool executeCommandInLAMMPS(QString command);
    LAMMPS_NS::LAMMPS *m_lammps = nullptr;
    LAMMPS_NS::LAMMPSException m_currentException;
    class System *m_system = nullptr;

public:
    LAMMPSController();
    ~LAMMPSController();

    // Getters/setters
    QMap<QString, class SimulatorControl*> simulatorControls;
    QList<ScriptCommand> commands;
    bool exceptionHandled() const;
    void setExceptionHandled(bool value);
    System *system() const;
    void setSystem(System *system);
    LAMMPS_NS::LAMMPS *lammps() const;
    bool tick();
    void stop();
    void start();

    // LAMMPS internal access
    int findComputeId(QString identifier);
    int findVariableIndex(QString identifier);
    int findFixIndex(QString identifier);
    bool computeExists(QString identifier);
    bool fixExists(QString identifier);
    bool variableExists(QString identifier);
    bool groupExists(QString identifier);
    bool regionExists(QString identifier);
    double variableValue(QString identifier);
    LAMMPS_NS::Compute *findComputeByIdentifier(QString identifier);
    LAMMPS_NS::Fix *findFixByIdentifier(QString identifier);
};

#endif // LAMMPSCONTROLLER_H
