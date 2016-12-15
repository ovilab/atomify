#include "lammpscontroller.h"
#include "states.h"
#include "LammpsWrappers/lammpserror.h"
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
#include <fix_external.h>
#include <stdio.h>
#include <QDebug>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <functional>
#include "parser/scriptcommand.h"
#include "mysimulator.h"
#include "LammpsWrappers/simulatorcontrols/simulatorcontrol.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/atoms.h"

using namespace std;
using namespace LAMMPS_NS;

LAMMPSController::LAMMPSController() :
    system(nullptr)
{

}

LAMMPSController::~LAMMPSController()
{
    stop();
}

void synchronizeLAMMPS_callback(void *caller, int mode, LAMMPS_NS::bigint ntimestep, int numberOfAtoms, LAMMPS_NS::tagint *tag, double **atom, double **force)
{
    LAMMPSController *controller = static_cast<LAMMPSController*>(caller);
    controller->synchronizeLAMMPS();
}

void LAMMPSController::synchronizeLAMMPS()
{
    if(!system) {
        qDebug() << "Error, we dont have system object. Anders or Svenn-Arne did a horrible job here...";
        exit(1);
    }
    system->synchronize(this);
    system->atoms()->updateData(system);
    system->atoms()->createRenderererData();

    qDebug() << "Yeah buddy, did sync, atoms: " << system->numberOfAtoms();
    didSynchronizeSimulator = false;
    locker->unlock();
    while(!didSynchronizeSimulator) {
        QThread::currentThread()->msleep(17); // Sleep 1/60th of a second
    }
     throw std::out_of_range("hello bitch");
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

    paused = false;
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
    lammps_command(m_lammps, "fix atomify_ext all external pf/callback 1 1");
    if(!fixExists("atomify_ext")) {
        qDebug() << "Damn, could not create the fix... :/";
        exit(1);
    }

    FixExternal *fix = dynamic_cast<FixExternal*>(findFixByIdentifier(QString("atomify_ext")));
    fix->set_callback(&synchronizeLAMMPS_callback, this);

//    std::function<void(LAMMPS *lammps)> func = [&](LAMMPS *lammps) {
//        synchronizeLAMMPS(lammps);
//    };

    // lammps_register_callback(m_lammps, &func);
}

bool LAMMPSController::tick()
{
    qDebug() << "Tick with LAMMPS: " << m_lammps;
    if(!m_lammps) return false;

    m_scriptFileName = "/Users/anderhaf/Desktop/lj.in";
    QByteArray ba = m_scriptFileName.toLatin1();

    try {
        qDebug() << "Trying to do cool stuff";
        lammps_file(m_lammps, ba.data());
    } catch(std::out_of_range error) {
        qDebug() << "Got the error: " << error.what();
        stop();
    }

//    if(!m_lammps || error || paused) return false;

//    for(ScriptCommand &commandObject : commands) {
//        const QString &command = commandObject.command();
//        executeCommandInLAMMPS(command);

//        bool hasError = m_lammps->error->get_last_error() != NULL;
//        if(hasError) {
//            // Handle error
//            QString message = QString::fromUtf8(m_lammps->error->get_last_error());
//            error = new LammpsError();
//            error->create(message, commandObject);
//            return true;
//        }
//        m_canProcessSimulatorControls = commandObject.canProcessSimulatorControls();
//    }

//    commands.clear();
//    return true;
}
