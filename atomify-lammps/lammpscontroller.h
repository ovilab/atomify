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
    LAMMPS *m_lammps = NULL;
    LammpsOutput *m_outputParser = NULL;
    QVector<QString> m_commands;
    QMap<QString, CPCompute*> m_computes;
    int m_simulationSpeed = 1;

    unsigned int m_runCommandStart = 0;
    unsigned int m_runCommandEnd = 0;
    bool m_runCommandActive = false;
    bool m_needPreRun = true;
    bool m_outputNotUpdated = true;

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
