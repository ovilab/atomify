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

#include "LammpsWrappers/simulatorcontrols/cpcompute.h"
#include "mysimulator.h"
#include "LammpsWrappers/simulatorcontrols/simulatorcontrol.h"
#include "scriptcommand.h"
using namespace std;

LAMMPSController::LAMMPSController()
{

}

LAMMPSController::~LAMMPSController()
{
    setLammps(nullptr);
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
        m_lammps = nullptr;
    }

    m_lammps = lammps;
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

void LAMMPSController::executeCommandInLAMMPS(QString command) {
    if(m_lammps == nullptr) {
        qDebug() << "Warning, trying to run a LAMMPS command with no LAMMPS object.";
        qDebug() << "Command: " << command;
        return;
    }

    if(!command.startsWith("run")) {
        qDebug() << command;
    }

    try {
        QByteArray commandBytes = command.toUtf8();
        lammps_command((void*)m_lammps, (char*)commandBytes.data());
    } catch (LammpsException &exception) {
        qDebug() << "ERROR: LAMMPS threw an exception!";
        qDebug() << "ERROR: File:" << QString::fromStdString(exception.file());
        qDebug() << "ERROR: Command:" << command;
        qDebug() << "ERROR: Message:" << QString::fromStdString(exception.error());
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
    string commandString = command.toStdString();
    stringstream command_ss(commandString);
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

                // int found = m_lammps->input->variable->find((char*)word_c_str);
                if(variableExists(QString::fromStdString(wordCopy))) {
                // if(found > -1) {
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

int LAMMPSController::findVariableIndex(QString identifier) {
    if(!m_lammps) {
        return -1;
    }
    char identifier_cstr[1000];
    QByteArray identifierBytes = identifier.toUtf8();
    strcpy(identifier_cstr, identifierBytes.constData());
    return m_lammps->input->variable->find(identifier_cstr);
}

LAMMPS_NS::Variable* LAMMPSController::findVariableByIdentifier(QString identifier) {
    int fixId = findFixIndex(identifier);
    if(fixId < 0) return nullptr;
    qFatal("LAMMPSController::findVariableByIdentifier(QString identifier) is not implemented yet");
    return nullptr;
}

LAMMPS_NS::Fix* LAMMPSController::findFixByIdentifier(QString identifier) {
    int fixId = findFixIndex(identifier);
    if(fixId < 0) {
        return nullptr;
    } else {
        return m_lammps->modify->fix[fixId];
    }
}

int LAMMPSController::findFixIndex(QString identifier) {
    if(!m_lammps) {
        return -1;
    }
    QByteArray identifierBytes = identifier.toUtf8();
    return m_lammps->modify->find_fix(identifierBytes.constData());
}

bool LAMMPSController::fixExists(QString identifier) {
    return findFixIndex(identifier) >= 0;
}

bool LAMMPSController::variableExists(QString identifier)
{
    return findVariableIndex(identifier) >= 0;
}

LAMMPS_NS::Compute* LAMMPSController::findComputeByIdentifier(QString identifier) {
    int computeId = findComputeId(identifier);
    if(computeId < 0) {
        return nullptr;
    } else {
        return m_lammps->modify->compute[computeId];
    }
}

int LAMMPSController::findComputeId(QString identifier) {
    if(!m_lammps) {
        return -1;
    }
    QByteArray identifierBytes = identifier.toUtf8();
    return m_lammps->modify->find_compute(identifierBytes.constData());
}

bool LAMMPSController::computeExists(QString identifier) {
    return (findComputeId(identifier) >= 0);
}

void LAMMPSController::processSimulatorControls() {
    for(SimulatorControl *control : simulatorControls) {
        control->update(this);
    }
}

System *LAMMPSController::system() const
{
    return m_system;
}

void LAMMPSController::setSystem(System *system)
{
    m_system = system;
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
    int nargs = 1;
    char **argv = new char*[nargs];
    for(int i=0; i<nargs; i++) {
        argv[i] = new char[100];
    }
    if(nargs>0) {
        sprintf(argv[0], "myprogram");
    }
    if(nargs>1) {
        sprintf(argv[1], "-sf");
    }
    if(nargs>2) {
        sprintf(argv[2], "gpu");
    }
//    sprintf(argv[3], "-pk");
//    sprintf(argv[4], "gpu");
//    sprintf(argv[5], "1");

    setLammps(nullptr); // This will destroy the LAMMPS object within the LAMMPS library framework
    lammps_open_no_mpi(nargs, argv, (void**)&m_lammps); // This creates a new LAMMPS object
    m_lammps->screen = NULL;
    state = LammpsState(); // Reset current state variables
}

void LAMMPSController::tick()
{
    if(m_lammps == nullptr) {
        return;
    }
    if(state.crashed) {
        return;
    }

    // If we have an active run command, perform the run command with the current chosen speed.
    if(state.runCommandActive > 0) {
        executeActiveRunCommand();
        processSimulatorControls();
        state.dataDirty = true;
        return;
    }

    // If the command stack has any commands left, process them.
    ScriptCommand nextCommand = state.nextCommand;
    if(nextCommand.type() == ScriptCommand::Type::SkipLammpsTick) return;

    if(nextCommand.type() != ScriptCommand::Type::NoCommand) {
//        if(nextCommand.type() == ScriptCommand::Type::SingleCommand) {
//        }
        state.preRunNeeded = true;

        bool didProcessCommand = m_scriptHandler->parseLammpsCommand(nextCommand.command(), this);
        if(didProcessCommand) {
            return;
        }

        processCommand(nextCommand.command());
    } else {
        if(state.paused) return;
        if(state.staticSystem) return;
        // If no commands are queued, just perform a normal run command with the current simulation speed.
        QElapsedTimer t;
        t.start();

        if(state.preRunNeeded) {
            executeCommandInLAMMPS(QString("run %1 pre yes post no").arg(state.simulationSpeed));
            state.preRunNeeded = false;
        } else {
            executeCommandInLAMMPS(QString("run %1 pre no post no").arg(state.simulationSpeed));
        }

        processSimulatorControls();
        state.numberOfTimesteps += state.simulationSpeed;
        state.timeSpentInLammps += t.elapsed();
    }

    state.dataDirty = true;
}

void LAMMPSController::disableAllEnsembleFixes()
{
    if(!m_scriptHandler) {
        qDebug() << "Error, LAMMPSController doesn't have script handler. Aborting!";
        exit(1);
    }
    state.preRunNeeded = true;
    for(int i=0; i<m_lammps->modify->nfix; i++) {
        LAMMPS_NS::Fix *fix = m_lammps->modify->fix[i];

        LAMMPS_NS::FixNVT *nvt = dynamic_cast<LAMMPS_NS::FixNVT*>(fix);
        if(nvt) {
            m_scriptHandler->addCommandToTop(ScriptCommand(QString("unfix %1").arg(nvt->id), ScriptCommand::Type::SingleCommand));
        }

        LAMMPS_NS::FixNVE *nve = dynamic_cast<LAMMPS_NS::FixNVE*>(fix);
        if(nve) {
            m_scriptHandler->addCommandToTop(ScriptCommand(QString("unfix %1").arg(nve->id), ScriptCommand::Type::SingleCommand));
        }

        LAMMPS_NS::FixNPT *npt = dynamic_cast<LAMMPS_NS::FixNPT*>(fix);
        if(npt) {
            m_scriptHandler->addCommandToTop(ScriptCommand(QString("unfix %1").arg(npt->id), ScriptCommand::Type::SingleCommand));
        }
    }
}

ScriptHandler *LAMMPSController::scriptHandler() const
{
    return m_scriptHandler;
}

void LAMMPSController::setScriptHandler(ScriptHandler *scriptHandler)
{
    m_scriptHandler = scriptHandler;
}

bool LAMMPSController::paused() const
{
    return state.paused;
}

void LAMMPSController::setPaused(bool value)
{
    state.paused = value;
}

bool LAMMPSController::crashed() const
{
    return state.crashed;
}

LammpsException &LAMMPSController::currentException()
{
    return m_currentException;
}
