/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */
#ifdef COMPUTE_CLASS

ComputeStyle(coord/usc,ComputeCoordUSC)

#else

#ifndef LMP_COMPUTE_COORD_USC_H
#define LMP_COMPUTE_COORD_USC_H

#include "compute.h"
#include "neigh_list.h"
namespace LAMMPS_NS {

class ComputeCoordUSC : public Compute {
public:
    ComputeCoordUSC(class LAMMPS *, int, char **);
    ~ComputeCoordUSC();
    void init();
    void compute_peratom();
    double memory_usage();

private:
    int nmax;                     // Allocated memory
    int numAtomPairs;                   // Number of neighbor types we are interested in
    double **coordinationNumbers; // Stores the computed coordination numbers
    double *R;
    double *D;
    int **type1type2ToIndexMap;
    int *typeIsFirstInPair;
    NeighList *list;

    // Compute interface
public:

public:
    virtual void init_list(int, NeighList *neighList);
    virtual int pack_forward_comm(int n, int *list, double *buf, int pbc_flag, int *pbc);
    virtual void unpack_forward_comm(int, int, double *);
};

}

#endif
#endif

/* ERROR/WARNING messages:

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

*/
