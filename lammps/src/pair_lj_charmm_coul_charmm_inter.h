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

#ifdef PAIR_CLASS

PairStyle(lj/charmm/coul/charmm/inter,PairLJCharmmCoulCharmmInter)

#else

#ifndef LMP_PAIR_LJ_CHARMM_COUL_CHARMM_INTER_H
#define LMP_PAIR_LJ_CHARMM_COUL_CHARMM_INTER_H

#include "pair_lj_softcore2.h"

namespace LAMMPS_NS {

class PairLJCharmmCoulCharmmInter : public PairLJSoftcore2 {

 public:
  PairLJCharmmCoulCharmmInter(class LAMMPS *);
  virtual ~PairLJCharmmCoulCharmmInter();
  virtual void compute(int, int);
  void settings(int, char **);
  void coeff(int, char **);
  void init_style();
  double init_one(int, int);
  void write_restart(FILE *);
  void read_restart(FILE *);
  void write_restart_settings(FILE *);
  void read_restart_settings(FILE *);
  void write_data(FILE *);
  void write_data_all(FILE *);
  virtual double single(int, int, int, int, double, double, double, double &);
  virtual void *extract(const char *, int &);

 protected:
  virtual void settings(vector<string>&);
  int implicit;
  double cut_lj_inner,cut_lj,cut_coul_inner,cut_coul;
  double cut_lj_innersq,cut_ljsq,cut_coul_innersq,cut_coulsq,cut_bothsq;
  double denom_lj_inv, denom_coul_inv;
  int cut_coul_and_lj_differ;
  tagint mol_lo, mol_hi, jmol_lo, jmol_hi;
  int invert_selection;
  int either_irange_or_jrange;
  int ignore_imol_equals_jmol;

  // Outside the "softcore" cutoff radius, ordinary Lennard-Jones potentials
  // are used:
  //   U(r) = lj3/r^12 - lj4/r^6
  // -dU/dr = lj1/r^13 - lj2/r^7

  //double **lj1,**lj2,**lj3,**lj4;               <-- see "pair_lj_remix.h"
  //double **epsilon, **sigma, **kappa, **lambda; <-- see "pair_lj_remix.h"

  double **eps14, **sigma14, **kappa14, **lambda14;
  double **lj14_1,**lj14_2,**lj14_3,**lj14_4; //coeffs for 1-4 interactions

  double **epsDIF, **sigmaDIF, **kappaDIF, **lambdaDIF;
  double **ljDIF_1, **ljDIF_2, **ljDIF_3, **ljDIF_4; //coeffs for different mols


  // Inside the "softcore" radius, a 1/r^2 potential is used:
  //   Ucore(r) =  core_coeff/r^2 - core_offset
  // -dUcore/dr = core_D_coeff/r^3     (core_D_coeff = -2*core_coeff)

  // double **cut_core_sqd, **core_coeff,  <-- see "pair_lj_softcore2.h"
  // double **core_D_coeff, **core_offset; <-- see "pair_lj_softcore2.h"

  double **cut_coreDIF_sqd,**core_coeffDIF,**core_D_coeffDIF,**core_offsetDIF;

  // There's no way to modify 14 behavior without changing dihedral_charmm.cpp
  // Disabling the softcore behavior for 1-4 interactions, and commenting out:
  //double **cut_core14_sqd, **core_coeff14, **core_D_coeff14, **core_offset14;

  void allocate();


  inline bool MolPairUseDIF(tagint imol, tagint jmol) {
    bool imol_in_range = ((mol_lo <= imol) && (imol <= mol_hi));
    bool jmol_in_range = ((mol_lo <= jmol) && (jmol <= mol_hi));
    bool selection_unspecified = (mol_hi < 0);
    bool in_selection = 
      (invert_selection != 
       ((either_irange_or_jrange && (imol_in_range || jmol_in_range))
	||
	((imol_in_range && jmol_in_range)
	 ||                                            // also check i<-->j swap:
	 (((jmol_lo <= imol) && (imol <= jmol_hi)) &&  // is i in j's range
	  ( (mol_lo <= jmol) && (jmol <= mol_hi) ))    //and j in i's range?
	 )
	)
       );

    return (((imol != jmol) && (in_selection || selection_unspecified))
	    ||
	    (ignore_imol_equals_jmol && in_selection));
  }

};  //class PairLJCharmmCoulCharmmInter

} //namespace LAMMPS_NS

#endif //#ifndef LMP_PAIR_LJ_CHARMM_COUL_CHARMM_INTER_H
#endif //#ifdef PAIR_CLASS


/* ERROR/WARNING messages:

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

E: Incorrect args for pair coefficients

Self-explanatory.  Check the input script or data file.

E: Pair style lj/charmm/coul/charmm requires atom attribute q

The atom style defined does not have these attributes.

E: Pair inner cutoff >= Pair outer cutoff

The specified cutoffs for the pair style are inconsistent.

*/
