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
#include <QThread>
#include <lmptype.h>

void synchronizeLAMMPS_callback(void *caller, int mode);

class Cancelled : public std::exception { };

class LAMMPSController
{
private:
    LAMMPS_NS::LAMMPS *m_lammps = nullptr;
    void changeWorkingDirectoryToScriptLocation();
public:
    class System *system = nullptr;

    LAMMPSController();
    ~LAMMPSController();
    class MyWorker *worker = nullptr;
    // Getters/setters
    QString scriptFilePath;
    QMap<QString, class SimulatorControl*> simulatorControls;
    bool paused = false;
    bool finished = false;
    bool didCancel = false;
    LAMMPS_NS::LAMMPS *lammps() const;
    bool tick();
    void stop();
    void start();
    QThread *qmlThread = nullptr;

    void synchronizeLAMMPS(int mode);
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
