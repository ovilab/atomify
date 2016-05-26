#ifndef NEIGHBORLIST_H
#define NEIGHBORLIST_H
#include <QVector>
#include <mpi.h>
#include <lammps.h>
#include <QVector3D>
typedef QVector<QVector<int>> CellList;

struct Neighborlist {
    class Bonds *bonds = nullptr;
    QVector<QVector<int>> neighbors;
    CellList cellList;
    int m_numCellsX = 0;
    int m_numCellsY = 0;
    int m_numCellsZ = 0;
    int m_lastNeighborListSync = -1;

    Neighborlist();
    Neighborlist(const Neighborlist &old);
    ~Neighborlist();
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    void build(QVector<QVector3D> &positions, class System *system);
    int index(int cx, int cy, int cz) { return cx*m_numCellsY*m_numCellsZ + cy*m_numCellsZ + cz; }
private:
    void reset(int numberOfAtoms, int maxNeighbors);
};

#endif // NEIGHBORLIST_H
