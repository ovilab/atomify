#include "lammpscontroller.h"
#include "states.h"
#include "LammpsWrappers/lammpserror.h"
#include <fix_ave_time.h>
#include <unistd.h>
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
#include <fix_atomify.h>
#include <stdio.h>
#include <QDebug>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <functional>
#include "LammpsWrappers/computes.h"
#include "parser/scriptcommand.h"
#include "mysimulator.h"
#include "LammpsWrappers/simulatorcontrols/simulatorcontrol.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/atoms.h"
#include "performance.h"

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

void synchronizeLAMMPS_callback(void *caller, int mode)
{
    LAMMPSController *controller = static_cast<LAMMPSController*>(caller);
    controller->synchronizeLAMMPS(mode);
}

void LAMMPSController::synchronizeLAMMPS(int mode)
{
    if(mode != LAMMPS_NS::FixConst::END_OF_STEP && mode != LAMMPS_NS::FixConst::MIN_POST_FORCE) return;
    if(!system) {
        qDebug() << "Error, we dont have system object. Anders or Svenn-Arne did a horrible job here...";
        exit(1);
    }

    system->synchronize(this);
    m_synchronizationCount++;
    if(m_synchronizationCount % 50 == 0) {
        // TODO: Move into system sync
        long t = m_timer.restart();
        double timePerTimestep = t / 50.0 / 1000; // Convert to seconds
        double speed = (1.0 / 60.0) / timePerTimestep;
        double oldSpeed = system->performance()->effectiveSimulationSpeed();
        speed = 0.9*oldSpeed + 0.1*speed; // Low pass filter
        system->performance()->setEffectiveSimulationSpeed(speed);
    }

    if(m_lammps->update->ntimestep - m_lastSynchronizationTimestep < simulationSpeed) return;
    m_lastSynchronizationTimestep = m_lammps->update->ntimestep;

    system->atoms()->processModifiers(system);
    system->atoms()->createRenderererData(this);
    worker->m_reprocessRenderingData = false;

    system->updateThreadOnDataObjects(qmlThread);

    worker->setNeedsSynchronization(true);
    while(worker->needsSynchronization()) {
        if(QThread::currentThread()->isInterruptionRequested()) {
            // Happens if main thread wants to exit application
            throw Cancelled();
        }

        if(worker->m_reprocessRenderingData) {
            system->atoms()->processModifiers(system);
            if(worker->m_workerRenderingMutex.tryLock()) {
                system->atoms()->createRenderererData(this);
                worker->m_reprocessRenderingData = false;
                worker->m_workerRenderingMutex.unlock();
            }
        }

        QThread::currentThread()->msleep(1); // Sleep 1/60th of a second
    }

    if(worker->m_cancelPending) {
        throw Cancelled();
    }
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
    doContinue = false;
    finished = false;
    didCancel = false;
    crashed = false;
    m_synchronizationCount = 0;
    m_lastSynchronizationTimestep = 0;
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

void LAMMPSController::changeWorkingDirectoryToScriptLocation() {
    QFileInfo fileInfo(scriptFilePath);
    if(!fileInfo.exists()) {
        qDebug() << "File " << scriptFilePath << " does not exist";
        return;
    }

    QString currentDirectory = fileInfo.absoluteDir().path();
    QByteArray currentDirBytes = currentDirectory.toUtf8();
    chdir(currentDirBytes.constData());
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
    finished = false;
    didCancel = false;
    crashed = false;
    lammps_command(m_lammps, "fix atomify all atomify");
    if(!fixExists("atomify")) {
        qDebug() << "Damn, could not create the fix... :/";
        exit(1);
    }
    m_timer.restart();

    FixAtomify *fix = dynamic_cast<FixAtomify*>(findFixByIdentifier(QString("atomify")));
    fix->set_callback(&synchronizeLAMMPS_callback, this);
    changeWorkingDirectoryToScriptLocation();
}

bool LAMMPSController::run()
{
    if(!m_lammps) return false;

    if(finished || didCancel || crashed) return false;

    QByteArray ba = scriptFilePath.toUtf8();
    scriptFilePath = "";

    try {
        if(doContinue) {
            QString command = "run 1000000000";

            qDebug() << "Will do continue in LAMMPS";
            lammps_command(m_lammps, command.toUtf8().data());
        } else {
            lammps_file(m_lammps, ba.data());
        }

        bool hasError = m_lammps->error->get_last_error() != NULL;
        if(hasError) {
            // Handle error
            errorMessage = QString::fromUtf8(m_lammps->error->get_last_error());

            crashed = true;
            // error = new LammpsError();
            // error->create(message, commandObject);
            return true;
        } else {
            finished = true;
        }
    } catch(Cancelled cancelled) {
        Q_UNUSED(cancelled)
        didCancel = true;
    }
    return true;
}
