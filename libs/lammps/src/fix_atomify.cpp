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
#include "error.h"

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
  mask |= POST_PARSE_COMMAND;
  mask |= POST_EXECUTE_COMMAND;
  mask |= END_OF_STEP;
  return mask;
}

/* ---------------------------------------------------------------------- */

void FixAtomify::init()
{
  if (callback == NULL)
    error->all(FLERR,"Fix atomify callback function not set");
}

/* ---------------------------------------------------------------------- */

void FixAtomify::end_of_step()
{
  (this->callback)(ptr_caller,END_OF_STEP);
}

/* ---------------------------------------------------------------------- */

void FixAtomify::post_parse_command()
{
  (this->callback)(ptr_caller,POST_PARSE_COMMAND);
}

/* ---------------------------------------------------------------------- */

void FixAtomify::post_execute_command()
{
  (this->callback)(ptr_caller,POST_EXECUTE_COMMAND);
}

/* ---------------------------------------------------------------------- */

void FixAtomify::set_callback(FnPtr caller_callback, void *caller_ptr)
{
  callback = caller_callback;
  ptr_caller = caller_ptr;
}
