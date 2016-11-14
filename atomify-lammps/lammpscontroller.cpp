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

double LAMMPSController::variableValue(QString identifier) {
    if(!variableExists(identifier)) {
        return 0; // should never happen anyway
    }
    QByteArray bytes = identifier.toUtf8();
    return *(static_cast<double*>(lammps_extract_variable((void*)m_lammps, bytes.data(), nullptr)));
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
