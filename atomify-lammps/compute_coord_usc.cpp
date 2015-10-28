/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#include "string.h"
#include "compute_coord_usc.h"
#include "atom.h"
#include "update.h"
#include "modify.h"
#include "comm.h"
#include "force.h"
#include "memory.h"
#include "error.h"
#include "neighbor.h"
#include "neigh_request.h"
#include <cmath>
#include <iostream>
using namespace LAMMPS_NS;

/* ---------------------------------------------------------------------- */

ComputeCoordUSC::ComputeCoordUSC(LAMMPS *lmp, int narg, char **arg) :
    Compute(lmp, narg, arg)
{
    if(narg < 3 || (narg-3) % 4) error->all(FLERR,"Illegal compute coord/usc command");

    nmax = 0;
    coordinationNumbers = NULL;
    list = NULL;
    R = NULL;
    D = NULL;

    // Each extra argument set is
    // type1 type2 R D

    numAtomPairs = (narg-3) / 4;
    memory->create(type1type2ToIndexMap, (atom->ntypes+1), (atom->ntypes+1),"coord/usc:type1type2ToIndexMap");
    memory->create(R, numAtomPairs, "coord/usc:R");
    memory->create(D, numAtomPairs, "coord/usc:D");
    memory->create(typeIsFirstInPair, atom->ntypes+1, "coord/usc:type1FirstPair");

    // First assume that no types are used
    for(int atomType1=0; atomType1<atom->ntypes+1; atomType1++) {
        typeIsFirstInPair[atomType1] = false;
        for(int atomType2=0; atomType2<atom->ntypes+1; atomType2++) {
            type1type2ToIndexMap[atomType1][atomType2] = -1;
        }
    }

    for(int i=0; i<numAtomPairs; i++) {
        int atomType1 = atoi(arg[3+4*i + 0]);
        int atomType2 = atoi(arg[3+4*i + 1]);

        typeIsFirstInPair[atomType1] = true;
        R[i] = atof(arg[3+4*i + 2]);
        D[i] = atof(arg[3+4*i + 3]);
        type1type2ToIndexMap[atomType1][atomType2] = i;

    }

    peratom_flag = 1;
    size_peratom_cols = 1;
}

/* ---------------------------------------------------------------------- */

ComputeCoordUSC::~ComputeCoordUSC()
{
    memory->destroy(coordinationNumbers);
    memory->destroy(D);
    memory->destroy(R);
    memory->destroy(type1type2ToIndexMap);
    memory->destroy(typeIsFirstInPair);
}

/* ---------------------------------------------------------------------- */

void ComputeCoordUSC::init()
{
    int irequest = neighbor->request(this,instance_me);
    neighbor->requests[irequest]->pair = 0;
    neighbor->requests[irequest]->compute = 1;
    neighbor->requests[irequest]->half = 0;
    neighbor->requests[irequest]->full = 1;
    neighbor->requests[irequest]->occasional = 1;
}

void LAMMPS_NS::ComputeCoordUSC::init_list(int, NeighList *neighList)
{
    list = neighList;
}

/* ---------------------------------------------------------------------- */

void ComputeCoordUSC::compute_peratom()
{
    invoked_peratom = update->ntimestep;

    // grow ke array if necessary

    if (atom->nlocal+atom->nghost > nmax) {
        memory->destroy(coordinationNumbers);
        nmax = atom->nmax;
        memory->create(coordinationNumbers, nmax, numAtomPairs,"coord/usc:coord");
        array_atom = coordinationNumbers;
    }
    // compute kinetic energy for each atom in group
    double **x = atom->x;
    int *type = atom->type;

    neighbor->build_one(list);

    int inum = list->inum;
    int *ilist = list->ilist;
    int *numneigh = list->numneigh;
    int **firstneigh = list->firstneigh;
    int numberOfAtomsIncludingGhosts = atom->nlocal + atom->nghost;
    for (int i = 0; i < numberOfAtomsIncludingGhosts; i++) {
        for(int j=0; j<numAtomPairs; j++) coordinationNumbers[i][j] = 0.0;
    }

    for (int ii = 0; ii < inum; ii++) {
        int i = ilist[ii];
        int itype = atom->type[i];

        // Reset the coordination numbers for this atom, we will check all its neighbors now

        if(!typeIsFirstInPair[itype]) continue; // This atom type does not appear as first pair, hence not interesting.

        double xi = x[i][0];
        double yi = x[i][1];
        double zi = x[i][2];
        // two-body interactions, skip half of them
        int *jlist = firstneigh[i];
        int jnum = numneigh[i];

        for (int jj = 0; jj < jnum; jj++) {
            int j = jlist[jj];
            j &= NEIGHMASK;
            int jtype = type[j];
            int atomPairIndex = type1type2ToIndexMap[itype][jtype];
            if(atomPairIndex < 0) continue; // This atom pair isn't interesting

            double RPlusD = R[atomPairIndex] + D[atomPairIndex];
            double RMinusD = R[atomPairIndex] - D[atomPairIndex];
            double RPlusDSquared = RPlusD*RPlusD;

            double dx = xi - x[j][0];
            double dy = yi - x[j][1];
            double dz = zi - x[j][2];
            double rsq = dx*dx + dy*dy + dz*dz;
            if(rsq > RPlusDSquared) continue;

            double r = sqrt(rsq);
            if(r < RMinusD) {
                coordinationNumbers[i][atomPairIndex] += 1.0;
            } else {
                coordinationNumbers[i][atomPairIndex] += 1.0;
                coordinationNumbers[i][atomPairIndex] -= 0.5*(r - R[atomPairIndex] + D[atomPairIndex])/D[atomPairIndex];
                coordinationNumbers[i][atomPairIndex] += sin(M_PI*(r - R[atomPairIndex] + D[atomPairIndex])/D[atomPairIndex])/(2*M_PI);
            }
        }
//        if(atom->tag[i] == 2) {
//            std::cout << "Coordination numbers for oxygen atom " << i << " on local CPU with tag " << atom->tag[i] << " and type " << atom->type[i] << ": " << std::endl;
//            std::cout << "nSi = " << coordinationNumbers[i][0] << std::endl;
//            std::cout << "nH = " << coordinationNumbers[i][1] << std::endl;
//        }
    }

    comm_forward = numAtomPairs;
    comm->forward_comm_compute(this);
}

/* ----------------------------------------------------------------------
   memory usage of local atom-based array
------------------------------------------------------------------------- */

double ComputeCoordUSC::memory_usage()
{
    double bytes = nmax * sizeof(double);
    return bytes;
}

int LAMMPS_NS::ComputeCoordUSC::pack_forward_comm(int n, int *list, double *buf,
                                                  int pbc_flag, int *pbc)
{
    int m = 0;
    for (int i = 0; i < n; i++) {
        int j = list[i];
        for(int k=0; k<comm_forward; k++) {
            // std::cout << "Sending coordination number for coordinationNumbers[" << j << "][" << k << "]=" << coordinationNumbers[j][k] << " as index " << m << std::endl;
            buf[m++] = coordinationNumbers[j][k];
        }
    }

    return m;
}

void LAMMPS_NS::ComputeCoordUSC::unpack_forward_comm(int n, int first, double *buf)
{
    int m = 0;
    int last = first + n;
    for (int i = first; i < last; i++) {
        for(int k=0; k<comm_forward; k++) {
            // std::cout << "Receiving coordination number for coordinationNumbers[" << i << "][" << k << "]=" << buf[m] << " as index " << m << std::endl;
            coordinationNumbers[i][k] = buf[m++];
        }
    }
}
