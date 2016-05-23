#include "neighborlist.h"
#include <neighbor.h>
#include <atom.h>
#include <neigh_list.h>
#include <domain.h>
#include "bonds.h"

using namespace LAMMPS_NS;

Neighborlist::Neighborlist()
{

}

Neighborlist::Neighborlist(const Neighborlist &old)
{
    numNeighbors = old.numNeighbors;
    neighbors = old.neighbors;
    bonds = old.bonds;
}

Neighborlist::~Neighborlist()
{
    bonds = nullptr;
    numNeighbors.clear();
    neighbors.clear();
}

void Neighborlist::synchronize(LAMMPS *lammps)
{
    if(!lammps) return;

    bool hasNeighborLists = lammps->neighbor->nlist > 0;
    if(hasNeighborLists) {
        NeighList *list = lammps->neighbor->lists[0];
        int inum = list->inum;
        int *ilist = list->ilist;
        int *numneigh = list->numneigh;
        int **firstneigh = list->firstneigh;
        double **x = lammps->atom->x;
        int *type = lammps->atom->type;
        neighbors.resize(inum);
        numNeighbors.resize(inum);

        for (int ii = 0; ii < inum; ii++) {
            int i = ilist[ii];
            double xi[3];
            xi[0] = x[i][0];
            xi[1] = x[i][1];
            xi[2] = x[i][2];
            int itype = type[i];
            int *jlist = firstneigh[i];
            int jnum = numneigh[i];
            numNeighbors[i] = 0;
            neighbors[i].resize(jnum);

            for (int jj = 0; jj < jnum; jj++) {
                int j = jlist[jj];
                j &= NEIGHMASK;
                double xj[3];
                xj[0] = x[j][0];
                xj[1] = x[j][1];
                xj[2] = x[j][2];
                lammps->domain->remap(xi);
                lammps->domain->remap(xj);

                double delx = xi[0] - xj[0];
                double dely = xi[1] - xj[1];
                double delz = xi[2] - xj[2];
                double rsq = delx*delx + dely*dely + delz*delz;
                int jtype = type[j];
                if(rsq < bonds->bondLengths()[itype][jtype]*bonds->bondLengths()[itype][jtype] ) {
                    neighbors[i][ numNeighbors[i] ] = j;
                    numNeighbors[i]++;
                }
            }
            neighbors.resize(numNeighbors[i]);
        }
    }
}

