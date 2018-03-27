#include "lammpsthread.h"
#include "tmplammps.h"
#include <QDebug>

namespace atomify {

LAMMPSThread::LAMMPSThread(QObject *parent)
    : QThread(parent)
    , m_dataDirty(false)
{

}


void LAMMPSThread::run()
{
    while (true) {
        TMPLAMMPS lammps;
        lammps.setCallback([&](TMPLAMMPS *lmp) {
            callback(lmp);
        });
        lammps.run();
    }
}

void LAMMPSThread::callback(TMPLAMMPS *lmp)
{
    {
        QMutexLocker locker(&m_mutex);
        m_data.timestep = lmp->timestep;
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
