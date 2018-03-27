#include "lammpsthread.h"
#include "tmplammps.h"
#include <QDebug>

namespace atomify {

LAMMPSThread::LAMMPSThread(QObject *parent) : QThread(parent)
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
    qDebug() << "ts: " << lmp->timestep;
}

}
