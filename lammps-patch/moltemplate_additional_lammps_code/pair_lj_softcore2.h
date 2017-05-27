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

/* ----------------------------------------------------------------------
   Contributing author: Andrew Jewett  (jewett.aij at gmail)
------------------------------------------------------------------------- */

#ifndef LMP_PAIR_LJ_SOFTCORE2_H
#define LMP_PAIR_LJ_SOFTCORE2_H

#include <cmath>
#include <string>
#include <vector>
using namespace std;
#include "error.h"
#include "pair_lj_remix.h"


namespace LAMMPS_NS {

class PairLJSoftcore2 : public PairLJRemix {
    // I only decided to have PairLJSoftcore2 inherit from PairLJRemix because
    // it needs to know "exponent_a" and "exponent_b" (usually 12 and 6).
    // (These are members of PairLJRemix.)

  protected:

  PairLJSoftcore2(class LAMMPS *lmp);

  virtual ~PairLJSoftcore2();

  // Outside the "softcore" cutoff radius, ordinary Lennard-Jones potentials
  // are used:
  //   U(r) = lj3/r^12 - lj4/r^6
  // -dU/dr = lj1/r^13 - lj2/r^7
  //
  // Inside the "softcore" radius, a 1/r^2 potential is used:
  //   Ucore(r) =  core_coeff/r^2 - core_offset
  // -dUcore/dr = core_D_coeff/r^3     (core_D_coeff = -2*core_coeff)
  double cut_core_sigma;
  double **cut_core_sqd, **core_coeff, **core_D_coeff, **core_offset;
  int ignore_large_rcore;
  int core_singular;

  virtual void allocate();

  //virtual void settings(int &narg, char **arg);
  virtual void settings(vector<string>&);

  virtual void init_one(int i, int j,
                        double **aa_sigma,
                        double **aa_cut_core_sqd,
                        double **aa_core_coeff,
                        double **aa_core_D_coeff,
                        double **aa_core_offset,
                        double **aa_lj3,
                        double **aa_lj4);

  virtual void write_restart_pair(FILE *fp,
                                  int i, int j,
                                  double **aa_cut_core_sqd,
                                  double **aa_core_coeff,
                                  double **aa_core_D_coeff,
                                  double **aa_core_offset);

  virtual void read_restart_pair(FILE *fp,
                                 int i, int j,
                                 double **aa_cut_core_sqd,
                                 double **aa_core_coeff,
                                 double **aa_core_D_coeff,
                                 double **aa_core_offset);

  virtual void write_restart_settings(FILE *fp);

  virtual void read_restart_settings(FILE *fp);

}; //class PairLJSoftcore2


} //namespace LAMMPS_NS

#endif //#ifndef LMP_PAIR_LJ_SOFTCORE2_H
