#ifndef NEIGHBORLIST_H
#define NEIGHBORLIST_H
#include <QVector>
#include <mpi.h>
#include <lammps.h>

struct Neighborlist {
    class Bonds *bonds;
    QVector<int> numNeighbors;
    QVector<QVector<int>> neighbors;
    Neighborlist();
    Neighborlist(const Neighborlist &old);
    ~Neighborlist();
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
};

#endif // NEIGHBORLIST_H
