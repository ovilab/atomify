#include "lammpsthread.h"
#include "tmplammps.h"

// std
#include <unistd.h>

// Qt
#include <QDebug>
#include <QDir>
#include <QFileInfo>

// SimVis
#include <SimVis/SphereData>

// LAMMPS
#include <atom.h>
#include <domain.h>
#include <error.h>
#include <fix.h>
#include <fix_atomify.h>
#include <input.h>
#include <lammps.h>
#include <library.h>
#include <modify.h>

using namespace LAMMPS_NS;

namespace atomify {

LAMMPSThread::LAMMPSThread(QObject* parent)
    : QThread(parent)
    , m_dataDirty(false)
{
}

int findFixIndex(LAMMPS_NS::LAMMPS* lammps, QString identifier)
{
    Q_ASSERT(lammps);
    QByteArray identifierBytes = identifier.toUtf8();
    return lammps->modify->find_fix(identifierBytes.constData());
}

bool fixExists(LAMMPS_NS::LAMMPS* lammps, QString identifier)
{
    return findFixIndex(lammps, identifier) >= 0;
}

void changeWorkingDirectoryToScriptLocation(QString scriptFilePath)
{
    QFileInfo fileInfo(scriptFilePath);
    if (!fileInfo.exists()) {
        qDebug() << "File " << scriptFilePath << " does not exist";
        return;
    }

    QString currentDirectory = fileInfo.absoluteDir().path();
    QByteArray currentDirBytes = currentDirectory.toUtf8();
    chdir(currentDirBytes.constData());
}

LAMMPS_NS::Fix* findFixByIdentifier(LAMMPS_NS::LAMMPS* lammps, QString identifier)
{
    const int fixId = findFixIndex(lammps, identifier);
    if (fixId < 0) {
        return nullptr;
    }
    return lammps->modify->fix[fixId];
}

void LAMMPSThread::run()
{
    while (true) {
        LAMMPS_NS::LAMMPS* lammps = nullptr;

        int nargs = 1;
        char** argv = new char*[nargs];
        for (int i = 0; i < nargs; i++) {
            argv[i] = new char[100];
        }

        lammps_open_no_mpi(nargs, argv, (void**)&lammps); // This creates a new LAMMPS object
        // m_lammps->screen = NULL;
        lammps_command(lammps, "fix atomify all atomify");
        if (!fixExists(lammps, "atomify")) {
            qDebug() << "Damn, could not create the fix... :/";
            exit(1);
        }

        LAMMPS_NS::Fix* originalFix = findFixByIdentifier(lammps, QString("atomify"));
        if (!originalFix) {
            throw std::runtime_error("Could not find fix with name atomify. Did you run ./configure.py?");
        }
        LAMMPS_NS::FixAtomify* fix = dynamic_cast<LAMMPS_NS::FixAtomify*>(originalFix);
        fix->set_callback([this, lammps](int stepType) {
            callback(lammps, stepType);
        });

        while (true) {
            // TODO remove dummy variables
            bool finished = false;
            bool didCancel = false;
            bool crashed = false;
            // END TODO

            // TODO synchronize with frontend
            bool doContinue = false;
            //            const QString scriptFilePath = "/home/svenni/projects/atomify/atomify/src/simulations/diffusion/simple_diffusion/simple_diffusion.in";
            const QString scriptFilePath = "/Users/anderhaf/Desktop/lj.in";

            if (finished || didCancel || crashed)
                break;

            try {
                if (doContinue) {
                    QString command = "run 1000000000";

                    lammps_command(lammps, command.toUtf8().data());
                } else {
                    QByteArray ba = scriptFilePath.toUtf8();
                    if (!QFile(scriptFilePath).exists()) {
                        qWarning() << "LAMMPSController::run: Script does not exist:" << scriptFilePath;
                        break;
                    }
                    lammps_file(lammps, ba.data());
                }

                bool hasError = lammps->error->get_last_error() != NULL;
                if (hasError) {
                    // Handle error
                    QString errorMessage = QString::fromUtf8(lammps->error->get_last_error());
                    // Show last error so user understands where the error occured
                    //                    system->setLastCommand(QString::fromUtf8(lammps->input->lastLine()));

                    crashed = true;
                    break;
                } else {
                    finished = true;
                }
            } catch (Cancelled cancelled) {
                Q_UNUSED(cancelled)
                didCancel = true;
                break;
            }
            // TODO synchronize changes from frontend (script name, etc.)
            // TODO setup script
            //        changeWorkingDirectoryToScriptLocation("");
            // TODO run LAMMPS
            // TODO break free when done
        }

        lammps->screen = NULL; // Avoids closing of the output parser.
        lammps_close((void*)lammps);
    }
}

void LAMMPSThread::callback(LAMMPS_NS::LAMMPS* lammps, int mode)
{
    // TODO move these dummy variables
    if (mode != LAMMPS_NS::FixConst::END_OF_STEP && mode != LAMMPS_NS::FixConst::MIN_POST_FORCE) {
        return;
    }

    {
        QMutexLocker locker(&m_mutex);
        copy(&m_data.atomData, static_cast<void*>(lammps));
        copy(&m_data.systemData, static_cast<void*>(lammps));
        m_dataDirty = true;
        qDebug() << "Did callback with ts = " << m_data.systemData.ntimestep;
    }
}

bool LAMMPSThread::dataDirty() const
{
    QMutexLocker locker(&m_mutex);
    return m_dataDirty;
}

LAMMPSData LAMMPSThread::data()
{
    {
        QMutexLocker locker(&m_mutex);
        std::swap(m_cachedData, m_data);
        m_dataDirty = false;
    }
    return m_cachedData;
}
}
