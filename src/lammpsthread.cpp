#include "lammpsthread.h"
#include "tmplammps.h"

// std
#include <unistd.h>

// Qt
#include <QDebug>
#include <QFileInfo>
#include <QDir>

// LAMMPS
#include <fix.h>
#include <fix_atomify.h>
#include <lammps.h>
#include <library.h>
#include <modify.h>

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
        bool finished = false;
        bool didCancel = false;
        bool crashed = false;
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

        // TODO update
//        changeWorkingDirectoryToScriptLocation("");
    }
}

void LAMMPSThread::callback(LAMMPS_NS::LAMMPS *lmp, int stepType)
{
    {
        QMutexLocker locker(&m_mutex);
//        m_data.timestep = lmp->timestep;
//        m_data.positions = lmp->positions;
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
