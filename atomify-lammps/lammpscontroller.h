#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H
#include <mpi.h>
#include "lammps/lammps.h"
#include <QVector>
#include <QString>
using namespace LAMMPS_NS;

class LAMMPSController
{
private:
    LAMMPS *m_lammps = 0;

    QVector<QString> m_commands;
    int m_simulationSpeed = 1;

    unsigned int m_runCommandStart = 0;
    unsigned int m_runCommandEnd = 0;
    bool m_runCommandActive = false;

    QString readFile(QString filename);
    QString copyDataFileToReadablePath(QString filename);
    QString copyFileAndFixNewCommand(QString command, std::stringstream &commandStringStream);
public:
    LAMMPSController();
    ~LAMMPSController();

    LAMMPS *lammps() const;
    void setLammps(LAMMPS *lammps);
    void runCommand(const char *command);
    void processCommand(QString command);
    void tick();
    void loadLammpsScript(QString filename);
    void reset();
    void runCommand(QString command);
    int simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
};

#endif // LAMMPSCONTROLLER_H
