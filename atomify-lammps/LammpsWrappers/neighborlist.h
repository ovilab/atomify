#ifndef NEIGHBORLIST_H
#define NEIGHBORLIST_H
#include <QVector>
#include <mpi.h>
#include <lammps.h>

struct Neighborlist {
    class Bonds *bonds;
    QVector<QVector<int>> neighbors;
    Neighborlist();
    Neighborlist(const Neighborlist &old);
    ~Neighborlist();
    void synchronize(LAMMPS_NS::LAMMPS *lammps);

private:
    void reset(int numberOfAtoms, int maxNeighbors);
};

#endif // NEIGHBORLIST_H
