#include "lammpsthread.h"
#include "tmplammps.h"
#include "LammpsWrappers/atomdata.h"

// std
#include <unistd.h>

// Qt
#include <QDebug>
#include <QFileInfo>
#include <QDir>

// SimVis
#include <SimVis/SphereData>

// LAMMPS
#include <fix.h>
#include <fix_atomify.h>
#include <lammps.h>
#include <library.h>
#include <modify.h>
#include <error.h>
#include <input.h>
#include <atom.h>
#include <domain.h>

using namespace LAMMPS_NS;

namespace atomify {

LAMMPSThread::LAMMPSThread(QObject *parent)
    : QThread(parent)
    , m_dataDirty(false)
{

}

int findFixIndex(LAMMPS_NS::LAMMPS *lammps, QString identifier) {
    Q_ASSERT(lammps);
    QByteArray identifierBytes = identifier.toUtf8();
    return lammps->modify->find_fix(identifierBytes.constData());
}

bool fixExists(LAMMPS_NS::LAMMPS *lammps, QString identifier) {
    return findFixIndex(lammps, identifier) >= 0;
}

void changeWorkingDirectoryToScriptLocation(QString scriptFilePath) {
    QFileInfo fileInfo(scriptFilePath);
    if(!fileInfo.exists()) {
        qDebug() << "File " << scriptFilePath << " does not exist";
        return;
    }

    QString currentDirectory = fileInfo.absoluteDir().path();
    QByteArray currentDirBytes = currentDirectory.toUtf8();
    chdir(currentDirBytes.constData());
}

LAMMPS_NS::Fix* findFixByIdentifier(LAMMPS_NS::LAMMPS *lammps, QString identifier) {
    const int fixId = findFixIndex(lammps, identifier);
    if(fixId < 0) {
        return nullptr;
    }
    return lammps->modify->fix[fixId];
}

void LAMMPSThread::run()
{
    while (true) {
        LAMMPS_NS::LAMMPS *lammps = nullptr;

        int nargs = 1;
        char **argv = new char*[nargs];
        for(int i=0; i<nargs; i++) {
            argv[i] = new char[100];
        }

        lammps_open_no_mpi(nargs, argv, (void**)&lammps); // This creates a new LAMMPS object
        // m_lammps->screen = NULL;
        lammps_command(lammps, "fix atomify all atomify");
        if(!fixExists(lammps, "atomify")) {
            qDebug() << "Damn, could not create the fix... :/";
            exit(1);
        }

        LAMMPS_NS::Fix *originalFix = findFixByIdentifier(lammps, QString("atomify"));
        if (!originalFix) {
            throw std::runtime_error("Could not find fix with name atomify. Did you run ./configure.py?");
        }
        LAMMPS_NS::FixAtomify *fix = dynamic_cast<LAMMPS_NS::FixAtomify*>(originalFix);
        fix->set_callback([this, lammps](int stepType){
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

            if(finished || didCancel || crashed)
                break;

            try {
                if(doContinue) {
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
            } catch(Cancelled cancelled) {
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

void LAMMPSThread::callback(LAMMPS_NS::LAMMPS *lammps, int mode)
{
    // TODO move these dummy variables
    float m_bondScale = 1.0;
    float m_globalScale = 1.0;
    QString m_renderingMode = "Sticks";
    QByteArray m_sphereDataRaw;
    // END TODO

    if(mode != LAMMPS_NS::FixConst::END_OF_STEP && mode != LAMMPS_NS::FixConst::MIN_POST_FORCE) {
        return;
    }

    Atom *atom = lammps->atom;
    Domain *domain = lammps->domain;
    int *types = lammps->atom->type;
    int numberOfAtoms = atom->natoms;

    {
        QMutexLocker locker(&m_mutex);

        AtomData &atomData = m_data.atomData;
        if(atomData.positions.size() != numberOfAtoms) {
            atomData.positions.resize(numberOfAtoms);
        }
        if(atomData.deltaPositions.size() != numberOfAtoms) {
            atomData.deltaPositions.resize(numberOfAtoms);
            for(QVector3D &delta : atomData.deltaPositions) delta = QVector3D(); // Reset delta
        }
        if(atomData.types.size() != numberOfAtoms) {
            atomData.types.resize(numberOfAtoms);
        }
        if(atomData.bitmask.size() != numberOfAtoms) {
            atomData.bitmask.resize(numberOfAtoms);
        }
        if(atomData.visible.size() != numberOfAtoms) {
            atomData.visible.resize(numberOfAtoms);
        }
        if(atomData.colors.size() != numberOfAtoms) {
            atomData.colors.resize(numberOfAtoms);
            for(QVector3D &color : atomData.colors) color = QVector3D(0.9, 0.2, 0.1);
        }
        if(atomData.originalIndex.size() != numberOfAtoms) {
            atomData.originalIndex.resize(numberOfAtoms);
        }
        if(atomData.radii.size() != numberOfAtoms) {
            atomData.radii.resize(numberOfAtoms);
            for(float &radii : atomData.radii) radii = 1.0;
        }
        if(atomData.lammpsParticleId.size() != numberOfAtoms) {
            atomData.lammpsParticleId.resize(numberOfAtoms);
            for(int &id : atomData.lammpsParticleId) id = 0;
        }
        atomData.radiiFromLAMMPS = lammps->atom->radius_flag;

        for(int i=0; i<numberOfAtoms; i++) {
            atomData.types[i] = types[i];
            atomData.originalIndex[i] = i;
            if (lammps->atom->tag_enable) {
                tagint tag = lammps->atom->tag[i];
                atomData.lammpsParticleId[i] = tag;
            } else {
                // TODO this might not be the right way to do it, consider giving error message
                atomData.lammpsParticleId[i] = i + 1;
            }

            double position[3];
            position[0] = atom->x[i][0];
            position[1] = atom->x[i][1];
            position[2] = atom->x[i][2];
            if(atomData.radiiFromLAMMPS) {
                atomData.radii[i] = atom->radius[i];
            }
            domain->remap(position); // remap into system boundaries with PBC

            atomData.positions[i][0] = position[0]*m_globalScale;
            atomData.positions[i][1] = position[1]*m_globalScale;
            atomData.positions[i][2] = position[2]*m_globalScale;
            atomData.bitmask[i] = atom->mask[i];
            atomData.visible[i] = true;
            atomData.deltaPositions[i] = QVector3D();
        }
        atomData.dirty = true;
        atomData.paused = false;

        int visibleAtomCount = 0;

        float radius = 0.2f * m_bondScale;
        if (m_renderingMode == "Sticks" || m_renderingMode == "Wireframe") {
            radius = 0.1f * m_bondScale;
        }

        for(int i = 0; i < atomData.size(); i++) {
            if(atomData.visible[i]) {
                atomData.positions[visibleAtomCount] = atomData.positions[i];
                atomData.colors[visibleAtomCount] = atomData.colors[i];
                atomData.radii[visibleAtomCount] = atomData.radii[i];
                atomData.deltaPositions[visibleAtomCount] = atomData.deltaPositions[i];
                atomData.radii[visibleAtomCount] = radius;
                atomData.lammpsParticleId[visibleAtomCount] = atomData.lammpsParticleId[i];
                visibleAtomCount++;
            }
        }
        atomData.resize(visibleAtomCount);

        m_dataDirty = true;
        qDebug() << "Did callback with ts = " << m_data.timestep;
    }
}

bool LAMMPSThread::dataDirty() const
{
    QMutexLocker locker(&m_mutex);
    return m_dataDirty;
}

LAMMPSData LAMMPSThread::data()
{
    LAMMPSData data;

    {
        QMutexLocker locker(&m_mutex);
        std::swap(data, m_data);
        m_dataDirty = false;
    }

    return data;
}

}
