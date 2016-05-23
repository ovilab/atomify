#ifndef GROUPS_H
#define GROUPS_H
#include <QObject>
#include <mpi.h>
#include <lammps.h>


struct Group {
    QString name;
};

class Groups : public QObject
{
    Q_OBJECT
public:
    Groups(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
};

#endif // GROUPS_H
