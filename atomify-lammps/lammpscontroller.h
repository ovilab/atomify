#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H
#include <mpi.h>
#include "lammps/lammps.h"
#include "lammps/compute.h"
#include "lammpsoutput.h"
#include "CPcompute.h"
#include <memory>
#include <QVector>
#include <QString>
#include <QMap>
using namespace LAMMPS_NS;
class LAMMPSController
{
private:
    struct State {
        bool runCommandActive = false;
        bool needPreRun = true;
        bool outputNotUpdated = true;
        int simulationSpeed = 1;
        unsigned int runCommandStart = 0;
        unsigned int runCommandEnd = 0;

    };

    State m_state;
    LAMMPS *m_lammps = NULL;
    LammpsOutput *m_outputParser = NULL;
    QVector<QString> m_commands;
    QMap<QString, CPCompute*> m_computes;

    QString readFile(QString filename);
    QString copyDataFileToReadablePath(QString filename);
    QString copyFileAndFixNewCommand(QString command, std::stringstream &commandStringStream);
    void processComputes();
    void checkOutput();
public:
    LAMMPSController();
    ~LAMMPSController();

    LAMMPS_NS::LAMMPS *lammps() const;
    void setLammps(LAMMPS *lammps);
    void runCommand(const char *command);
    void runCommand(QString command);
    void processCommand(QString command);
    void tick();
    void loadScriptFromFile(QString filename);
    void runScript(QString script);
    void reset();

    int  simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    LammpsOutput *outputParser() const;
    void setOutputParser(LammpsOutput *outputParser);
    QMap<QString, CPCompute *> computes() const;
    void setComputes(const QMap<QString, CPCompute *> &computes);
};

#endif // LAMMPSCONTROLLER_H
