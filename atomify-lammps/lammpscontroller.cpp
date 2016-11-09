#include "lammpscontroller.h"
#include "states.h"

#include <fix_ave_time.h>
#include <integrate.h>
#include <library.h>
#include <atom.h>
#include <group.h>
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
#include "parser/scriptcommand.h"
#include "mysimulator.h"
#include "LammpsWrappers/simulatorcontrols/simulatorcontrol.h"
using namespace std;
using namespace LAMMPS_NS;

System *LAMMPSController::system() const
{
    return m_system;
}

void LAMMPSController::setSystem(System *system)
{
    m_system = system;
}

LAMMPSController::LAMMPSController() :
    m_currentException("")
{

}

LAMMPSController::~LAMMPSController()
{
    stop();
}

LAMMPS *LAMMPSController::lammps() const
{
    return m_lammps;
}

void LAMMPSController::stop()
{
    if(m_lammps) {
        m_lammps->screen = NULL; // Avoids closing of the output parser.
        lammps_close((void*)m_lammps);
        m_lammps = nullptr;
    }
}

bool LAMMPSController::executeCommandInLAMMPS(QString command) {
    if(m_lammps == nullptr) {
        qDebug() << "Warning, trying to run a LAMMPS command with no LAMMPS object.";
        qDebug() << "Command: " << command;
        return true;
    }

    if(true || !command.startsWith("run")) {
        qDebug() << "EXECUTING LAMMPS COMMAND: " << command;
    }

    QByteArray commandBytes = command.toUtf8();
    lammps_command((void*)m_lammps, (char*)commandBytes.data());
    char *lammpsError = m_lammps->error->get_last_error();
    if(lammpsError != NULL) {
        m_currentException =  LAMMPSException(lammpsError); // Store a copy of the exception to communicate to GUI
        return false;
    }
    return true;
}

// void LAMMPSController::processCommand(QString command) {
// Parse one single LAMMPS command. Three notes:
//   Files are read with full filenames in LAMMPS. Files added as Qt Resrouces have path :/file, which will not work.
//   We have introduced the notation ext://file which will copy the file to a temp location and is readable with its full filename.
//
//   Also, run commands are modified so that run N will be split into smaller run commands to allow smooth visualizations.
//   This means that if the number of timesteps are given as a LAMMPS variable, we need to extract that.
//
//   We also added a fastrun command that act as a normal run command with no visualization.

//    string commandString = command.toStdString();
//    stringstream command_ss(commandString);
//    string word;
//    QString processedCommand;

//    int wordCount = 0;
//    while(command_ss >> word) {
//        // If this is the first word in a command and it is "run"
//        if(wordCount == 0 && word.compare("run") == 0) {
//            command_ss >> word; // Next word is the number of timesteps
//            if(QString::fromStdString(word).trimmed().compare("0") == 0) {
//                // It is a run 0 command.
//                processedCommand = "run 0";
//                continue;
//            }

//            int numberOfTimesteps = atoi(word.c_str());
//            if(numberOfTimesteps == 0) {
//                // We probably need to read it as a variable in lammps.
//                string wordCopy = word;
//                wordCopy.erase(0,2);
//                wordCopy.erase(word.length()-3,1);
//                const char *word_c_str = wordCopy.c_str();

//                // int found = m_lammps->input->variable->find((char*)word_c_str);
//                if(variableExists(QString::fromStdString(wordCopy))) {
//                    // if(found > -1) {
//                    char *numberOfTimestepsString = m_lammps->input->variable->retrieve((char*)word_c_str);
//                    numberOfTimesteps = atoi(numberOfTimestepsString);
//                }
//            }

//            if(command_ss >> word) {
//                qDebug() << "Warning, LAMMPS visualizer doesn't support run commands with arguments. These will be ignored.";
//            }

//            state.runCommandActive = true;
//            state.runCommandStart = m_lammps->update->ntimestep;
//            state.runCommandEnd = state.runCommandStart + numberOfTimesteps;
//            return;
//        }

//        if(wordCount == 0 && word.compare("fastrun") == 0) {
//            processedCommand = "run ";
//            wordCount++;
//            continue;
//        }

//        processedCommand.append(QString::fromStdString(word)).append(" ");
//        wordCount++;
//    }

//    executeCommandInLAMMPS(processedCommand);
// }

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

bool LAMMPSController::groupExists(QString identifier)
{
    if(!m_lammps) {
        return -1;
    }
    QByteArray identifierBytes = identifier.toUtf8();
    return m_lammps->group->find(identifierBytes.constData())>=0; // -1 means not found
}

bool LAMMPSController::regionExists(QString identifier)
{
    if(!m_lammps) {
        return -1;
    }
    QByteArray identifierBytes = identifier.toUtf8();
    return m_lammps->domain->find_region(identifierBytes.data())>=0; // -1 means not found
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

bool LAMMPSController::exceptionHandled() const
{
    return m_exceptionHandled;
}

void LAMMPSController::setExceptionHandled(bool value)
{
    m_exceptionHandled = value;
}

//void LAMMPSController::executeActiveRunCommand() {
//    unsigned int currentTimestep = m_lammps->update->ntimestep;
//    int maxSimulationSpeed = state.runCommandEnd - currentTimestep; // We cannot exceed the last timestep in the active run command

//    int simulationSpeed = min(state.simulationSpeed, maxSimulationSpeed);


//    QElapsedTimer t;

//    t.start();
//    if(currentTimestep == state.runCommandStart || state.preRunNeeded) {
//        // If this is the first timestep in this run command, execute the pre yes command to prepare the full run command.
//        executeCommandInLAMMPS(QString("run %1 pre yes post no start %2 stop %3").arg(simulationSpeed).arg(state.runCommandStart).arg(state.runCommandEnd));
//        state.preRunNeeded = false;
//    } else {
//        executeCommandInLAMMPS(QString("run %1 pre no post no start %2 stop %3").arg(simulationSpeed).arg(state.runCommandStart).arg(state.runCommandEnd));
//    }
//    state.timeSpentInLammps += t.elapsed();
//    currentTimestep = m_lammps->update->ntimestep;
//    state.runCommandActive = currentTimestep < state.runCommandEnd;
//}

void LAMMPSController::start() {
    if(m_lammps) {
        stop();
    }

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
    lammps_open_no_mpi(nargs, argv, (void**)&m_lammps); // This creates a new LAMMPS object
    m_lammps->screen = NULL;
}

bool LAMMPSController::tick()
{
    for(ScriptCommand &commandObject : commands) {
        const QString &command = commandObject.command();
        bool ok = executeCommandInLAMMPS(command);
        if(!ok) {
            // Handle error
            qDebug() << "Error in LAMMPS. We need to handle this.";
            exit(1);
        }
    }

    commands.clear();
    //    if(nextCommand.type() == ScriptCommand::Type::SkipLammpsTick) return true;

    //    if(nextCommand.type() == ScriptCommand::Type::NoCommand) {
    //        // If no commands are queued, but user has pressed play again, do normal run
    //        if(!states->continued()->active()) return true;

    //        if(state.preRunNeeded) {
    //            executeCommandInLAMMPS(QString("run %1 pre yes post no").arg(state.simulationSpeed));
    //            state.preRunNeeded = false;
    //        } else {
    //            executeCommandInLAMMPS(QString("run %1 pre no post no").arg(state.simulationSpeed));
    //        }
    //        state.canProcessSimulatorControls = true;

    //    } else {
    //        state.preRunNeeded = true;

    //        bool didProcessCommand = m_scriptHandler->parseLammpsCommand(nextCommand.command(), this);
    //        if(didProcessCommand) {
    //            return true;
    //        }

    //        processCommand(nextCommand.command());
    //    }

    //    state.dataDirty = true;
    return true;
}
