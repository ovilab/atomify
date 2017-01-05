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
#include <qelapsedtimer.h>

void synchronizeLAMMPS_callback(void *caller, int mode);

class Cancelled : public std::exception { };

class LAMMPSController
{
private:
    LAMMPS_NS::LAMMPS *m_lammps = nullptr;
    unsigned long m_lastSynchronizationTimestep = 0;
    void changeWorkingDirectoryToScriptLocation();
    QElapsedTimer m_timer;
    unsigned long m_synchronizationCount = 0;
    double m_timePerTimestep = 0;
public:
    class System *system = nullptr;
    unsigned long simulationSpeed = 1;

    LAMMPSController();
    ~LAMMPSController();
    class MyWorker *worker = nullptr;
    // Getters/setters
    QString scriptFilePath;
    QMap<QString, class SimulatorControl*> simulatorControls;
    bool finished = false;
    bool didCancel = false;
    bool crashed = false;
    bool doContinue = false;
    QString errorMessage;
    LAMMPS_NS::LAMMPS *lammps() const;
    bool run();
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
