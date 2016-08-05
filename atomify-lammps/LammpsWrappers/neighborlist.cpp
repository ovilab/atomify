#include "neighborlist.h"
#include "system.h"
#include <neighbor.h>
#include <atom.h>
#include <neigh_list.h>
#include <domain.h>
#include "cpbonds.h"
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
    memoryUsage = numberOfAtoms*maxNeighbors*sizeof(int);
}

void Neighborlist::synchronize(LAMMPS *lammps)
{
    if(!lammps) return;
    QElapsedTimer t;
    t.start();
    bool hasNeighborLists = lammps->neighbor->nlist > 0;
    int numNeighbors = 0;

    if(hasNeighborLists) {
        NeighList *list = lammps->neighbor->lists[0];
        int inum = list->inum;
        int *ilist = list->ilist;
        int *numneigh = list->numneigh;
        int **firstneigh = list->firstneigh;
        reset(inum);
        t.start();
        memoryUsage = 0;
        for (int ii = 0; ii < inum; ii++) {
            int i = ilist[ii];
            int *jlist = firstneigh[i];
            int jnum = numneigh[i];

            for (int jj = 0; jj < jnum; jj++) {
                int j = jlist[jj];
                j &= NEIGHMASK;
                if(j < lammps->atom->natoms) {

                    neighbors[i].push_back(j);
                    numNeighbors++;
                } else {
                    int mappedIndex = lammps->atom->map(lammps->atom->tag[j]);
                    if(mappedIndex > -1) {
                        // We have enabled map in lammps. We now know what atom this ghost really is
                        neighbors[i].push_back(mappedIndex);
                        numNeighbors++;
                    }
                }
            }

            memoryUsage += neighbors[i].capacity()*sizeof(int);
        }
    }
}
