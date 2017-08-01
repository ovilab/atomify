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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fix_atomify.h"
#include "atom.h"
#include "comm.h"
#include "neighbor.h"
#include "neigh_request.h"
#include "neigh_list.h"
#include "memory.h"
#include "compute.h"
#include "modify.h"
#include "error.h"
#include "update.h"
#include "compute_pe_atom.h"
#include "compute_stress_atom.h"

using namespace LAMMPS_NS;
using namespace FixConst;

/* ---------------------------------------------------------------------- */

FixAtomify::FixAtomify(LAMMPS *lmp, int narg, char **arg) :
    Fix(lmp, narg, arg)
{
    callback = NULL;
}

/* ---------------------------------------------------------------------- */

FixAtomify::~FixAtomify() { }

/* ---------------------------------------------------------------------- */

int FixAtomify::setmask()
{
    int mask = 0;
    mask |= END_OF_STEP;
    mask |= MIN_POST_FORCE;
    return mask;
}

/* ---------------------------------------------------------------------- */

void FixAtomify::init()
{
    if (callback == NULL)
        error->all(FLERR,"Fix atomify callback function not set");
    error->warning(FLERR,"Asking for neigh list");
    int irequest = neighbor->request(this,instance_me);
    neighbor->requests[irequest]->pair = 0;
    neighbor->requests[irequest]->fix = 1;
    neighbor->requests[irequest]->occasional = 1;
    neighbor->requests[irequest]->half = 1;
    neighbor->requests[irequest]->full = 0;
}

void FixAtomify::init_list(int id, NeighList *ptr)
{
  list = ptr;
}

void FixAtomify::lost_atoms()
{
    bigint natoms;
    bigint nblocal = atom->nlocal;
    MPI_Allreduce(&nblocal,&natoms,1,MPI_LMP_BIGINT,MPI_SUM,world);
    if (natoms != atom->natoms && comm->me == 0) {
        char str[128];
        sprintf(str,
                "Lost atoms: original " BIGINT_FORMAT " current " BIGINT_FORMAT,
                atom->natoms,natoms);
        error->all(FLERR,str);
    }
}

/* ---------------------------------------------------------------------- */

void FixAtomify::update_compute(const char computeId[])
{
    int icompute = modify->find_compute(computeId);
    if (icompute < 0) return;
    
    Compute *compute = modify->compute[icompute];
    compute->addstep(update->ntimestep+1);
}

void FixAtomify::update_computes()
{
    for(int i=0; i<modify->ncompute; i++) {
        Compute *compute = modify->compute[i];
        if(compute->peatomflag || compute->peflag || compute->pressatomflag || compute->pressflag) {
            compute->addstep(update->ntimestep+1);
        }
    }
}

void FixAtomify::end_of_step()
{
    neighbor->build_one(list);
    lost_atoms();
    (this->callback)(ptr_caller,END_OF_STEP);
    update_computes();
}

/* ---------------------------------------------------------------------- */

void FixAtomify::min_post_force(int vflag)
{
    neighbor->build_one(list);
    lost_atoms();
    (this->callback)(ptr_caller,MIN_POST_FORCE);
}

/* ---------------------------------------------------------------------- */

void FixAtomify::set_callback(FnPtr caller_callback, void *caller_ptr)
{
    callback = caller_callback;
    ptr_caller = caller_ptr;
}
