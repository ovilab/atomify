#ifndef REGIONS_H
#define REGIONS_H

#include <QObject>
#include <mpi.h>
#include <lammps.h>

class Regions : public QObject
{
    Q_OBJECT
public:
    Regions(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
private:

};

#endif // REGIONS_H
