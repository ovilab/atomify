#ifndef NEIGHBORLIST_H
#define NEIGHBORLIST_H
#include <QVector>
#include <mpi.h>
#include <lammps.h>
#include <QVector3D>
typedef QVector<QVector<int>> CellList;

struct Neighborlist {
    class CPBonds *bonds = nullptr;
    QVector<QVector<int>> neighbors;
    long memoryUsage = 0;

    Neighborlist();
    Neighborlist(const Neighborlist &old);
    ~Neighborlist();
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    void reset(int numberOfAtoms, int maxNeighbors = 100);
};

#endif // NEIGHBORLIST_H
