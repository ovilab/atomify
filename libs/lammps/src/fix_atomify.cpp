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
#include "memory.h"
#include "compute.h"
#include "modify.h"
#include "error.h"
#include "update.h"
#include "compute_pe_atom.h"
#include "compute_stress_atom.h"
#include <iostream>
using namespace std;
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
  // mask |= POST_PARSE_COMMAND;
  // mask |= POST_EXECUTE_COMMAND;
  mask |= END_OF_STEP;
  mask |= MIN_POST_FORCE;
  return mask;
}

/* ---------------------------------------------------------------------- */

void FixAtomify::init()
{
  if (callback == NULL)
    error->all(FLERR,"Fix atomify callback function not set");
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
      // update_compute(peAtom->id);
    }
    // ComputePEAtom *peAtom = dynamic_cast<ComputePEAtom *>(modify->compute[i]);
    // if(peAtom) { update_compute(peAtom->id); }

    // ComputeStressAtom *stressAtom = dynamic_cast<ComputeStressAtom*>(modify->compute[i]);
    // if(stressAtom) { update_compute(stressAtom->id); }

  }

  // update_compute("thermo_press");
  // update_compute("thermo_pe");
}

void FixAtomify::end_of_step()
{
    (this->callback)(ptr_caller,END_OF_STEP);

    update_computes();

    // cout << "Found compute: " << compute << endl;
}

/* ---------------------------------------------------------------------- */

void FixAtomify::min_post_force(int vflag)
{
  (this->callback)(ptr_caller,MIN_POST_FORCE);
}

/* ---------------------------------------------------------------------- */

void FixAtomify::set_callback(FnPtr caller_callback, void *caller_ptr)
{
  callback = caller_callback;
  ptr_caller = caller_ptr;
}
