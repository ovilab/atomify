#include "lammpscontroller.h"

#include <fix_ave_time.h>
#include <integrate.h>
#include <library.h>
#include <atom.h>
#include <input.h>
#include <variable.h>
#include <update.h>
#include <modify.h>
#include <output.h>
#include <dump.h>
#include <domain.h>
#include <fix.h>

#include "CPcompute.h"
#include "lammpsfilehandler.h"
#include "mysimulator.h"

#include <stdio.h>
#include <QDebug>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
using namespace std;

LAMMPSController::LAMMPSController()
{

}

LAMMPSController::~LAMMPSController()
{
    setLammps(NULL);
    m_computes.clear();
}

LAMMPS_NS::LAMMPS *LAMMPSController::lammps() const
{
    return m_lammps;
}

void LAMMPSController::setLammps(LAMMPS *lammps)
{
    if(m_lammps) {
        m_lammps->screen = NULL; // Avoids closing of the output parser.
        lammps_close((void*)m_lammps);
        m_lammps = NULL;
    }

    m_lammps = lammps;
}

ScriptHandler *LAMMPSController::scriptHandler()
{
    return &m_scriptHandler;
}

void LAMMPSController::setWorker(MyWorker *worker)
{
    m_worker = worker;
}


int LAMMPSController::simulationSpeed() const
{
    return m_state.simulationSpeed;
}

void LAMMPSController::setSimulationSpeed(int simulationSpeed)
{
    if(m_state.simulationSpeed != simulationSpeed) {
        m_state.simulationSpeed = simulationSpeed;
    }
}

QMap<QString, CPCompute *> LAMMPSController::computes() const
{
    return m_computes;
}

void LAMMPSController::setComputes(const QMap<QString, CPCompute *> &computes)
{
    m_computes = computes;
}

void LAMMPSController::executeCommandInLAMMPS(QString command) {
    if(m_lammps == NULL) {
        qDebug() << "Warning, trying to run a LAMMPS command with no LAMMPS object.";
        qDebug() << "Command: " << command;
        return;
    }

    try {
        lammps_command((void*)m_lammps, (char*) command.toStdString().c_str());
    } catch (LammpsException &exception) {
        m_currentException = exception; // Store a copy of the exception to communicate to GUI
        m_state.crashed = true;
    }
}

void LAMMPSController::processCommand(QString command) {
    // Parse one single LAMMPS command. Three notes:
    //   Files are read with full filenames in LAMMPS. Files added as Qt Resrouces have path :/file, which will not work.
    //   We have introduced the notation ext://file which will copy the file to a temp location and is readable with its full filename.
    //
    //   Also, run commands are modified so that run N will be split into smaller run commands to allow smooth visualizations.
    //   This means that if the number of timesteps are given as a LAMMPS variable, we need to extract that.
    //
    //   We also added a fastrun command that act as a normal run command with no visualization.
    stringstream command_ss(command.toStdString());
    string word;
    QString processedCommand;

    int wordCount = 0;
    while(command_ss >> word) {
        if(word.compare("pause") == 0) {
            m_worker->setWillPause(true);
            return;
        }

        // If this is the first word in a command and it is "run"
        if(wordCount == 0 && word.compare("run") == 0) {
            command_ss >> word; // Next word is the number of timesteps
            int numberOfTimesteps = atoi(word.c_str());
            if(numberOfTimesteps == 0) {
                // We probably need to read it as a variable in lammps.
                string wordCopy = word;
                wordCopy.erase(0,2);
                wordCopy.erase(word.length()-3,1);
                const char *word_c_str = wordCopy.c_str();

                int found = m_lammps->input->variable->find((char*)word_c_str);
                if(found > -1) {
                    char *numberOfTimestepsString = m_lammps->input->variable->retrieve((char*)word_c_str);
                    numberOfTimesteps = atoi(numberOfTimestepsString);
                }
            }

            if(command_ss >> word) {
                qDebug() << "Warning, LAMMPS visualizer doesn't support run commands with arguments. These will be ignored.";
            }

            m_state.runCommandActive = true;
            m_state.runCommandStart = m_lammps->update->ntimestep;
            m_state.runCommandEnd = m_state.runCommandStart + numberOfTimesteps;
            return;
        }

        if(wordCount == 0 && word.compare("fastrun") == 0) {
            processedCommand = "run ";
            wordCount++;
            continue;
        }

        processedCommand.append(QString::fromStdString(word)).append(" ");
        wordCount++;
    }

    executeCommandInLAMMPS(processedCommand);
}

LAMMPS_NS::Fix* LAMMPSController::findFix(QString identifier) {
    int fixId = findFixId(identifier);
    if(fixId < 0) return NULL;
    else return m_lammps->modify->fix[fixId];
}

int LAMMPSController::findFixId(QString identifier) {
    return m_lammps->modify->find_fix(identifier.toStdString().c_str());
}

bool LAMMPSController::fixExists(QString identifier) {
    return findFixId(identifier) >= 0;
}

LAMMPS_NS::Compute* LAMMPSController::findCompute(QString identifier) {
    int computeId = findComputeId(identifier);
    if(computeId < 0) return NULL;
    else return m_lammps->modify->compute[computeId];
}

int LAMMPSController::findComputeId(QString identifier) {
    return m_lammps->modify->find_compute(identifier.toStdString().c_str());
}

bool LAMMPSController::computeExists(QString identifier) {
    return (findComputeId(identifier) >= 0);
}

void LAMMPSController::processComputes()
{
    for(QString key : m_computes.keys()) {
        if(!computeExists(key)) {
            CPCompute *compute = m_computes[key];
            // We need to add it. First check all dependencies
            bool allDependenciesFound = true; // Assume all are found and find potential counterexample
            foreach(QString dependencyIdentifier, compute->dependencies()) {
                if(!computeExists(dependencyIdentifier)) {
                    allDependenciesFound = false;
                    break;
                }
            }

            if(allDependenciesFound) {
                m_state.preRunNeeded = true; // When a new compute is added, a run with pre yes is needed for it to be included
                // Now that all dependencies are met we can add this one too
                executeCommandInLAMMPS(compute->command());
                // Now we need to create a fix that will store these values
                QString fixIdentifier = QString("fix%1").arg(compute->identifier());
                compute->setFixIdentifier(fixIdentifier);

                QString fixCommand = QString("fix %1 all ave/time 1 5 10 c_%2").arg(fixIdentifier, compute->identifier());
                if(compute->isVector()) fixCommand.append(" mode vector");

                compute->setFixCommand(fixCommand);
                executeCommandInLAMMPS(fixCommand);
                // Now replace the output on the object
                FixAveTime *fix = dynamic_cast<FixAveTime*>(findFix(fixIdentifier));
                if(fix) {
                    // This is our baby
                    fix->fp = compute->output().stream();
                }
            }
        }
    }
}

void LAMMPSController::executeActiveRunCommand() {
    unsigned int currentTimestep = m_lammps->update->ntimestep;
    int maxSimulationSpeed = m_state.runCommandEnd - currentTimestep; // We cannot exceed the last timestep in the active run command

    int simulationSpeed = min(m_state.simulationSpeed, maxSimulationSpeed);

    QElapsedTimer t;
    t.start();
    if(currentTimestep == m_state.runCommandStart || m_state.preRunNeeded) {
        // If this is the first timestep in this run command, execute the pre yes command to prepare the full run command.
        executeCommandInLAMMPS(QString("run %1 pre yes post no start %2 stop %3").arg(simulationSpeed).arg(m_state.runCommandStart).arg(m_state.runCommandEnd));
        m_state.preRunNeeded = false;
    } else {
        executeCommandInLAMMPS(QString("run %1 pre no post no start %2 stop %3").arg(simulationSpeed).arg(m_state.runCommandStart).arg(m_state.runCommandEnd));
    }
    m_state.timeSpentInLammps += t.elapsed();
    m_state.numberOfTimesteps += simulationSpeed;
    currentTimestep = m_lammps->update->ntimestep;
    m_state.runCommandActive = currentTimestep < m_state.runCommandEnd;
}

void LAMMPSController::reset()
{
    if(m_lammps != NULL) {
        // We need to set FILE pointers in fixes to NULL so that they are not closed when LAMMPS deallocates.
        for(CPCompute *compute : m_computes) {
            if(computeExists(compute->identifier())) {
                FixAveTime *fix = dynamic_cast<FixAveTime*>(findFix(compute->fixIdentifier()));
                if(fix != NULL) {
                    fix->fp = NULL;
                }
            }
        }
    }
    setLammps(NULL); // This will destroy the LAMMPS object within the LAMMPS library framework
    lammps_open_no_mpi(0, 0, (void**)&m_lammps); // This creates a new LAMMPS object
    m_lammps->screen = NULL;

    m_state = State(); // Reset current state variables
}

void LAMMPSController::tick()
{
    if(m_lammps == NULL) return;
    if(m_state.crashed) return;

    // If we have an active run command, perform the run command with the current chosen speed.
    if(m_state.runCommandActive > 0) {
        processComputes(); // Only work with computes and output when we will do a run
        executeActiveRunCommand();
        m_state.dataDirty = true;
        return;
    }

//    if(!m_state.queuedCommands.isEmpty()) {
//        QString command = m_state.queuedCommands.front();
//        m_state.queuedCommands.pop_front();
//        processCommand(command);
//        m_state.preRunNeeded = true;
//        m_state.dataDirty = true;
//        return;
//    }

    QPair<QString, CommandInfo> nextCommandObject = m_scriptHandler.nextCommand();
    QString nextCommand = nextCommandObject.first;
    CommandInfo nextCommandInfo = nextCommandObject.second;
    if(!nextCommand.isEmpty()) {
        // If the command stack has any commands left, process them.
        if(nextCommandInfo.type == CommandInfo::Type::SingleCommand) {
            m_state.preRunNeeded = true;
        }

        processCommand(nextCommand);
    } else {
        if(m_state.paused) return;
        // If no commands are queued, just perform a normal run command with the current simulation speed.
        processComputes(); // Only work with computes and output when we will do a run
        QElapsedTimer t;
        t.start();

        if(m_state.preRunNeeded) {
            executeCommandInLAMMPS(QString("run %1 pre yes post no").arg(m_state.simulationSpeed));
            m_state.preRunNeeded = false;
        } else {
            executeCommandInLAMMPS(QString("run %1 pre no post no").arg(m_state.simulationSpeed));
        }
        m_state.numberOfTimesteps += m_state.simulationSpeed;
        m_state.timeSpentInLammps += t.elapsed();
    }

    m_state.dataDirty = true;
}

int LAMMPSController::numberOfAtoms() const
{
    if(!m_lammps) return 0;
    return m_lammps->atom->natoms;
}

int LAMMPSController::numberOfAtomTypes() const
{
    if(!m_lammps) return 0;
    return m_lammps->atom->ntypes;
}

QVector3D LAMMPSController::systemSize() const
{
    if(!m_lammps) return QVector3D();
    return QVector3D(m_lammps->domain->xprd, m_lammps->domain->yprd, m_lammps->domain->zprd);
}

bool LAMMPSController::paused() const
{
    return m_state.paused;
}

void LAMMPSController::setPaused(bool value)
{
    m_state.paused = value;
}

bool LAMMPSController::dataDirty() const
{
    return m_state.dataDirty;
}

void LAMMPSController::setDataDirty(bool value)
{
    m_state.dataDirty = value;
}

bool LAMMPSController::crashed() const
{
    return m_state.crashed;
}

double LAMMPSController::timePerTimestep()
{
    return ((double)m_state.timeSpentInLammps) / (m_state.numberOfTimesteps ? m_state.numberOfTimesteps : 1); // Measured in ms
}

LammpsException &LAMMPSController::currentException()
{
    return m_currentException;
}

double LAMMPSController::simulationTime()
{
    if(!m_lammps) return 0;
    return m_lammps->update->atime;
}
