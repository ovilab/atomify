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
#include <fix_nve.h>
#include <fix_nvt.h>
#include <fix_npt.h>

#include <stdio.h>
#include <QDebug>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>

#include "CPcompute.h"
#include "mysimulator.h"
#include "simulatorcontrol.h"
#include "scriptcommand.h"
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
    return state.simulationSpeed;
}

void LAMMPSController::setSimulationSpeed(int simulationSpeed)
{
    if(state.simulationSpeed != simulationSpeed) {
        state.simulationSpeed = simulationSpeed;
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

//    qDebug() << command;
    try {
        lammps_command((void*)m_lammps, (char*) command.toStdString().c_str());
    } catch (LammpsException &exception) {
        m_currentException = exception; // Store a copy of the exception to communicate to GUI
        state.crashed = true;
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

            state.runCommandActive = true;
            state.runCommandStart = m_lammps->update->ntimestep;
            state.runCommandEnd = state.runCommandStart + numberOfTimesteps;
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

LAMMPS_NS::Fix* LAMMPSController::findFixByIdentifier(QString identifier) {
    int fixId = findFixIndex(identifier);
    if(fixId < 0) return NULL;
    else return m_lammps->modify->fix[fixId];
}

int LAMMPSController::findFixIndex(QString identifier) {
    return m_lammps->modify->find_fix(identifier.toStdString().c_str());
}

bool LAMMPSController::fixExists(QString identifier) {
    return findFixIndex(identifier) >= 0;
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

void LAMMPSController::processSimulatorControls() {
    for(SimulatorControl *control : simulatorControls) {
        control->synchronizeLammps(this);
    }
}

void LAMMPSController::processComputes()
{
    for(QString key : m_computes.keys()) {
        if(!computeExists(key)) {
            qDebug() << "Missing key: " << key;

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
                qDebug() << "All dependencies found";
                state.preRunNeeded = true; // When a new compute is added, a run with pre yes is needed for it to be included
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
                FixAveTime *fix = dynamic_cast<FixAveTime*>(findFixByIdentifier(fixIdentifier));
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
    int maxSimulationSpeed = state.runCommandEnd - currentTimestep; // We cannot exceed the last timestep in the active run command

    int simulationSpeed = min(state.simulationSpeed, maxSimulationSpeed);

    QElapsedTimer t;
    t.start();
    if(currentTimestep == state.runCommandStart || state.preRunNeeded) {
        // If this is the first timestep in this run command, execute the pre yes command to prepare the full run command.
        executeCommandInLAMMPS(QString("run %1 pre yes post no start %2 stop %3").arg(simulationSpeed).arg(state.runCommandStart).arg(state.runCommandEnd));
        state.preRunNeeded = false;
    } else {
        executeCommandInLAMMPS(QString("run %1 pre no post no start %2 stop %3").arg(simulationSpeed).arg(state.runCommandStart).arg(state.runCommandEnd));
    }
    state.timeSpentInLammps += t.elapsed();
    state.numberOfTimesteps += simulationSpeed;
    currentTimestep = m_lammps->update->ntimestep;
    state.runCommandActive = currentTimestep < state.runCommandEnd;
}

void LAMMPSController::reset()
{
    if(m_lammps != NULL) {
        // We need to set FILE pointers in fixes to NULL so that they are not closed when LAMMPS deallocates.
        for(CPCompute *compute : m_computes) {
            if(computeExists(compute->identifier())) {
                FixAveTime *fix = dynamic_cast<FixAveTime*>(findFixByIdentifier(compute->fixIdentifier()));
                if(fix != NULL) {
                    fix->fp = NULL;
                }
            }
        }
    }
    setLammps(NULL); // This will destroy the LAMMPS object within the LAMMPS library framework
    lammps_open_no_mpi(0, 0, (void**)&m_lammps); // This creates a new LAMMPS object
    m_lammps->screen = NULL;
    state = State(); // Reset current state variables
}

void LAMMPSController::tick()
{
    if(m_lammps == NULL) return;
    if(state.crashed) return;

    // If we have an active run command, perform the run command with the current chosen speed.
    if(state.runCommandActive > 0) {
        processSimulatorControls();
        processComputes(); // Only work with computes and output when we will do a run
        executeActiveRunCommand();
        state.dataDirty = true;
        return;
    }

    // If the command stack has any commands left, process them.
    ScriptCommand nextCommand = state.nextCommand;
    if(nextCommand.type() == ScriptCommand::Type::SkipLammpsTick) return;

    if(nextCommand.type() != ScriptCommand::Type::NoCommand) {
        if(nextCommand.type() == ScriptCommand::Type::SingleCommand) {
            state.preRunNeeded = true;
        }

        bool didProcessCommand = m_scriptHandler.parseLammpsCommand(nextCommand.command(), this);
        if(didProcessCommand) {
            return;
        }

        processCommand(nextCommand.command());
    } else {
        if(state.paused) return;
        // If no commands are queued, just perform a normal run command with the current simulation speed.
        processSimulatorControls();
        processComputes(); // Only work with computes and output when we will do a run
        QElapsedTimer t;
        t.start();

        if(state.preRunNeeded) {
            executeCommandInLAMMPS(QString("run %1 pre yes post no").arg(state.simulationSpeed));
            state.preRunNeeded = false;
        } else {
            executeCommandInLAMMPS(QString("run %1 pre no post no").arg(state.simulationSpeed));
        }
        state.numberOfTimesteps += state.simulationSpeed;
        state.timeSpentInLammps += t.elapsed();
    }

    state.dataDirty = true;
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

void LAMMPSController::disableAllEnsembleFixes()
{
    state.preRunNeeded = true;
    while(true) {
        bool didFindFix = false;
        for(int i=0; i<m_lammps->modify->nfix; i++) {
            LAMMPS_NS::Fix *fix = m_lammps->modify->fix[i];

            LAMMPS_NS::FixNVT *nvt = dynamic_cast<LAMMPS_NS::FixNVT*>(fix);
            if(nvt) {
                executeCommandInLAMMPS(QString("unfix %1").arg(nvt->id));
                didFindFix = true;
                break;
            }

            LAMMPS_NS::FixNVE *nve = dynamic_cast<LAMMPS_NS::FixNVE*>(fix);
            if(nve) {
                executeCommandInLAMMPS(QString("unfix %1").arg(nve->id));
                didFindFix = true;
                break;
            }

            LAMMPS_NS::FixNPT *npt = dynamic_cast<LAMMPS_NS::FixNPT*>(fix);
            if(npt) {
                executeCommandInLAMMPS(QString("unfix %1").arg(npt->id));
                didFindFix = true;
                break;
            }
        }

        if(!didFindFix) return;
    }
}

QVector3D LAMMPSController::systemSize() const
{
    if(!m_lammps) return QVector3D();
    return QVector3D(m_lammps->domain->xprd, m_lammps->domain->yprd, m_lammps->domain->zprd);
}

bool LAMMPSController::paused() const
{
    return state.paused;
}

void LAMMPSController::setPaused(bool value)
{
    state.paused = value;
}

bool LAMMPSController::dataDirty() const
{
    return state.dataDirty;
}

void LAMMPSController::setDataDirty(bool value)
{
    state.dataDirty = value;
}

bool LAMMPSController::crashed() const
{
    return state.crashed;
}

double LAMMPSController::timePerTimestep()
{
    return ((double)state.timeSpentInLammps) / (state.numberOfTimesteps ? state.numberOfTimesteps : 1); // Measured in ms
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
