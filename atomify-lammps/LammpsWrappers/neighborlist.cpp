#include "neighborlist.h"
#include "system.h"
#include <neighbor.h>
#include <atom.h>
#include <neigh_list.h>
#include <domain.h>
#include "bonds.h"
#include <QDebug>
#include <QElapsedTimer>
using namespace LAMMPS_NS;

Neighborlist::Neighborlist()
{

}

Neighborlist::Neighborlist(const Neighborlist &old)
{
    neighbors = old.neighbors;
    bonds = old.bonds;
}

Neighborlist::~Neighborlist()
{
    bonds = nullptr;
    neighbors.clear();
}

void Neighborlist::reset(int numberOfAtoms, int maxNeighbors) {
    neighbors.resize(numberOfAtoms);
    for(int i=0; i<numberOfAtoms; i++) {
        if(neighbors[i].capacity() < maxNeighbors) {
            neighbors[i].reserve(maxNeighbors);
        }
        neighbors[i].resize(0);
    }
}

void Neighborlist::synchronize(LAMMPS *lammps)
{
    if(!lammps) return;
    QElapsedTimer t;
    t.start();
    bool hasNeighborLists = lammps->neighbor->nlist > 0;
    if(hasNeighborLists && lammps->neighbor->ago==0) {
        NeighList *list = lammps->neighbor->lists[0];
        int inum = list->inum;
        int *ilist = list->ilist;
        int *numneigh = list->numneigh;
        int **firstneigh = list->firstneigh;
        reset(inum, 100);

        t.start();
        for (int ii = 0; ii < inum; ii++) {
            int i = ilist[ii];
            int *jlist = firstneigh[i];
            int jnum = numneigh[i];

            for (int jj = 0; jj < jnum; jj++) {
                int j = jlist[jj];
                j &= NEIGHMASK;
                if(j < lammps->atom->natoms) {
                    neighbors[i].push_back(j);
                }
            }
        }
    }
    // qDebug() << "Neighborlist copy synced in " << t.elapsed();
}

void Neighborlist::build(QVector<QVector3D> &positions, System *system)
{
    QVector3D origin = system->origin();
    QVector3D size = system->size();
    float maxBondLength = bonds->maxBondLength();
    if(maxBondLength <= 0) return;

    int numCellsX = size[0] / maxBondLength;
    int numCellsY = size[1] / maxBondLength;
    int numCellsZ = size[2] / maxBondLength;
    QVector3D cellSize(size[0]/numCellsX, size[1]/numCellsY, size[2]/numCellsZ);
    int numberOfCells = numCellsX*numCellsY*numCellsZ;
    if(numberOfCells != m_numCellsX*m_numCellsY*m_numCellsZ) {
        QElapsedTimer t;
        t.start();
        cellList.resize(numberOfCells);
        for(int i=0; i<numberOfCells; i++) {
            cellList[i].resize(0);
            cellList[i].reserve(100);
        }
        m_numCellsX = numCellsX;
        m_numCellsY = numCellsY;
        m_numCellsZ = numCellsZ;
        qDebug() << "Allocating took " << t.elapsed() << " ms.";
    } else {
        QElapsedTimer t;
        t.start();
        for(int i=0; i<numberOfCells; i++) {
            cellList[i].resize(0);
        }
        qDebug() << "Resetting took " << t.elapsed() << " ms.";
        qDebug() << "We have " << numCellsX << " " << numCellsY << " " << numCellsZ << " cells";
    }

    QElapsedTimer t;
    t.start();
    QVector3D oneOverCellSize(1.0/cellSize[0], 1.0/cellSize[1], 1.0/cellSize[2]);
    for(int i=0; i<positions.size(); i++) {
        const QVector3D &position = positions.at(i);
        int cx = (position[0] - origin[0]) * oneOverCellSize[0];
        int cy = (position[1] - origin[1]) * oneOverCellSize[1];
        int cz = (position[2] - origin[2]) * oneOverCellSize[2];
        cellList[index(cx, cy, cz)].push_back(i);
    }
    qDebug() << "Building took " << t.elapsed() << " ms.";
}

