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
   Contributing authors: Andrew Jewett (jewett.aij-at-gmail) and Paul Crozier
------------------------------------------------------------------------- */

#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pair_lj_charmm_coul_charmm_inter.h"
#include "atom.h"
#include "comm.h"
#include "force.h"
#include "neighbor.h"
#include "neigh_list.h"
#include "memory.h"
#include "error.h"
#include <cassert>
using namespace LAMMPS_NS;

#define ENABLE_INTER_DIF
#define ENABLE_SOFTCORE2
#define NDEBUG //(disable assert())

/* ---------------------------------------------------------------------- */

PairLJCharmmCoulCharmmInter::
PairLJCharmmCoulCharmmInter(LAMMPS *lmp) : PairLJSoftcore2(lmp)
{
  implicit = 0;

  core_singular = true; // By default, atoms can not pass through each other.
                        //(Use U(r) = U0 + k/r^2 for small r below the softcore
                        // cutoff (if it is > 0). Otherwise use U(r)=U0 - k*r^2)

  // default styles:
  mix_flag = ARITHMETIC;

  cut_coul_and_lj_differ = 0; //=false;   (same switching func used for LJ and COUL)

  mol_lo = -1;
  mol_hi = -1;
  jmol_lo = -1;
  jmol_hi = -1; //impossible values disable this feature
  invert_selection = 1; //= true;
  either_irange_or_jrange = 0; //= false;
  ignore_imol_equals_jmol = 0; //= true;

}


/* ---------------------------------------------------------------------- */

PairLJCharmmCoulCharmmInter::~PairLJCharmmCoulCharmmInter()
{
  if (allocated) {
    memory->destroy(cutsq);

    // Tables needed for initialization
    memory->destroy(setflag);

    memory->destroy(eps14);
    memory->destroy(sigma14);
    memory->destroy(kappa14);
    memory->destroy(lambda14);

    #ifdef ENABLE_INTER_DIF
    memory->destroy(epsDIF);
    memory->destroy(sigmaDIF);
    memory->destroy(kappaDIF);
    memory->destroy(lambdaDIF);
    #endif

    // Usually force computation only requires these tables:
    memory->destroy(lj14_1);
    memory->destroy(lj14_2);
    memory->destroy(lj14_3);
    memory->destroy(lj14_4);

    #ifdef ENABLE_INTER_DIF
    memory->destroy(ljDIF_1);
    memory->destroy(ljDIF_2);
    memory->destroy(ljDIF_3);
    memory->destroy(ljDIF_4);
    #endif


    #ifdef ENABLE_SOFTCORE2
    // Arrays describing parameters for the potential used below 
    // the "core" cutoff (soft repulsive function for short distances)
    #ifdef ENABLE_INTER_DIF
    memory->destroy(cut_coreDIF_sqd);
    memory->destroy(core_coeffDIF);
    memory->destroy(core_D_coeffDIF);
    memory->destroy(core_offsetDIF);
    #endif
    // There's no way to modify 14 behavior without changing dihedral_charmm.cpp
    // Disabling the cutoff behavior for 1-4 interactions, and commenting out:
    //memory->destroy(cut_core14_sqd);
    //memory->destroy(core_coeff14);
    //memory->destroy(core_D_coeff14);
    //memory->destroy(core_offset14);
    #endif //#ifdef ENABLE_SOFTCORE2


  } //if (allocated)
} //~PairLJCharmmCoulCharmmInter()


/* ---------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::compute(int eflag, int vflag)
{
  int i,j,ii,jj,inum,jnum,itype,jtype;
  double q_i,q_j,xtmp,ytmp,ztmp,delx,dely,delz,evdwl,ecoul,fpair;
  double rsq,r2inv,r6inv,forcecoul,forcelj,factor_coul,factor_lj;
  double   cutljsq_minus_rsq,   cutljsq_minus_rsq_sq;
  double cutcoulsq_minus_rsq, cutcoulsq_minus_rsq_sq;
  double philj, switch1_coul, switch2_coul, switch1_lj, switch2_lj;
  int *ilist,*jlist,*numneigh,**firstneigh;

  evdwl = ecoul = 0.0;
  if (eflag || vflag) ev_setup(eflag,vflag);
  else evflag = vflag_fdotr = 0;

  double **x = atom->x;
  double **f = atom->f;
  double *q = atom->q;
  int *type = atom->type;
  int nlocal = atom->nlocal;
  double *special_coul = force->special_coul;
  double *special_lj = force->special_lj;
  int newton_pair = force->newton_pair;
  double qqrd2e = force->qqrd2e;

  int *molecule = atom->molecule;
  double _lj1, _lj2, _lj3, _lj4;
  #ifdef ENABLE_SOFTCORE2
  double _cut_core_sqd, _core_coeff, _core_D_coeff, _core_offset;
  #endif

  inum = list->inum;
  ilist = list->ilist;
  numneigh = list->numneigh;
  firstneigh = list->firstneigh;

  // loop over neighbors of my atoms

  for (ii = 0; ii < inum; ii++) {
    i = ilist[ii];
    q_i = q[i];
    xtmp = x[i][0];
    ytmp = x[i][1];
    ztmp = x[i][2];
    itype = type[i];
    jlist = firstneigh[i];
    jnum = numneigh[i];

    for (jj = 0; jj < jnum; jj++) {
      j = jlist[jj];
      factor_lj = special_lj[sbmask(j)];
      factor_coul = special_coul[sbmask(j)];
      j &= NEIGHMASK;

      delx = xtmp - x[j][0];
      dely = ytmp - x[j][1];
      delz = ztmp - x[j][2];
      rsq = delx*delx + dely*dely + delz*delz;

      if (rsq < cut_bothsq) {
        r2inv = 1.0/rsq;

        if (rsq < cut_coulsq) {
          q_j = q[j];
          //Only evaluate the expensive sqrt() operation for nonzero charges.
          //On intel i7 hardware, this if statement accelerates simulations
          //noticably for uncharged systems (approx %30).
          //However in that case, it's probably better just to set cut_coulsq=0.
          //using the pair_style command (set inner_cutoff2 = outer_cutoff2 = 0)
          //if ((q_i != 0.0) && (q_j != 0.0))
          forcecoul = qqrd2e * q_i * q_j * sqrt(r2inv);
          //else
          //  forcecoul = 0.0;

          if (rsq > cut_coul_innersq) {
            cutcoulsq_minus_rsq = (cut_coulsq-rsq);
            cutcoulsq_minus_rsq_sq = cutcoulsq_minus_rsq * cutcoulsq_minus_rsq;
            switch1_coul = cutcoulsq_minus_rsq_sq *
              (cut_coulsq + 2.0*rsq - 3.0*cut_coul_innersq) * denom_coul_inv;
            switch2_coul = 12.0*rsq * cutcoulsq_minus_rsq * 
              (rsq-cut_coul_innersq) * denom_coul_inv;
            forcecoul *= switch1_coul + switch2_coul;
          }
        } else forcecoul = 0.0;

        if (rsq < cut_ljsq) {
          r6inv = r2inv*r2inv*r2inv;
          jtype = type[j];

          #ifdef ENABLE_INTER_DIF

          // Question: Do we use ordinary Lennard-Jones parameters or "DIF" parameters?
          //
	  // Originally I only checked to see if the two molecule-IDs are equal
	  //
	  // if (imol != jmol) {
	  //
          // This meant: if two atoms belong to different molecules, use "DIF" parameters.
          // Currently, I've replaced this simple if-statement with an ugly mess
          // which also takes into account whether or not the two molecule-IDs also
          // fall inside (or outside) a range which the user requested.
          // These possibilities are explained pictorally in the documentation for
          // pair_style lj/charmm/coul/charmm/inter.

	  if (MolPairUseDIF(atom->molecule[i],
			    atom->molecule[j]))
	  {
            // Then use the "DIF" params (alternate Lennard-Jones parameters)
            _lj1 = ljDIF_1[itype][jtype];
            _lj2 = ljDIF_2[itype][jtype];
            _lj3 = ljDIF_3[itype][jtype];
            _lj4 = ljDIF_4[itype][jtype];
            #ifdef ENABLE_SOFTCORE2
            _cut_core_sqd = cut_coreDIF_sqd[itype][jtype];
            _core_coeff   =   core_coeffDIF[itype][jtype];
            _core_D_coeff = core_D_coeffDIF[itype][jtype];
            _core_offset  =  core_offsetDIF[itype][jtype];
            #endif
          }
	  else
          #endif //#ifdef ENABLE_INTER_DIF
          {
            // Otherwise, use the ordinary/default Lennard-Jones parameters:
            _lj1 = lj1[itype][jtype];
            _lj2 = lj2[itype][jtype];
            _lj3 = lj3[itype][jtype];
            _lj4 = lj4[itype][jtype];
            #ifdef ENABLE_SOFTCORE2
            _cut_core_sqd = cut_core_sqd[itype][jtype];
            _core_coeff   =   core_coeff[itype][jtype];
            _core_D_coeff = core_D_coeff[itype][jtype];
            _core_offset  =  core_offset[itype][jtype];
            #endif
          } //else clause for "if (molecule[i] != molecule[j])"

          #ifdef ENABLE_SOFTCORE2
          if (rsq > _cut_core_sqd) {
          #endif

            //  Standard Lennard-Jones function
            //    Ulj(r) = (_lj3/r^12- _lj4/r^6) * switch1_lj(r)
            forcelj = r6inv * (_lj1*r6inv - _lj2);

            if (rsq > cut_lj_innersq) {
              switch1_lj = switch1_coul;
              switch2_lj = switch2_coul;
              if (cut_coul_and_lj_differ) { // then recompute switch1 & switch2
                cutljsq_minus_rsq = (cut_ljsq-rsq);
                cutljsq_minus_rsq_sq = cutljsq_minus_rsq * cutljsq_minus_rsq;
                switch1_lj = cutljsq_minus_rsq_sq *
                  (cut_ljsq + 2.0*rsq - 3.0*cut_lj_innersq) * denom_lj_inv;
                switch2_lj = 12.0*rsq * cutljsq_minus_rsq * 
                  (rsq-cut_lj_innersq) * denom_lj_inv;
              }
              philj = r6inv * (_lj3*r6inv - _lj4);
              forcelj = forcelj*switch1_lj + philj*switch2_lj;
            }

          #ifdef ENABLE_SOFTCORE2
          }
          else {
            //  Soft-core repulsive function
            if (core_singular) {
              //    Ulj(r) = (_core_coeff/r^2  + _core_offset) * switch1_lj(r)
              forcelj = _core_D_coeff * r2inv; //(will be divided by rij later)
            }
            else {
              //    Ulj(r) = (_core_coeff*r^2  + _core_offset) * switch1_lj(r)
              forcelj = _core_D_coeff * rsq;   //(will be divided by rij later)
            }

            if (rsq > cut_lj_innersq) {
              // The next 8 lines should be identical to the lines above.  (No
              // way to move them out without creating an extra if-then check.)
              switch1_lj = switch1_coul;
              switch2_lj = switch2_coul;
              if (cut_coul_and_lj_differ) { // then recompute switch1 & switch2
                cutljsq_minus_rsq = (cut_ljsq-rsq);
                cutljsq_minus_rsq_sq = cutljsq_minus_rsq * cutljsq_minus_rsq;
                switch1_lj = cutljsq_minus_rsq_sq *
                  (cut_ljsq + 2.0*rsq - 3.0*cut_lj_innersq) * denom_lj_inv;
                switch2_lj = 12.0*rsq * cutljsq_minus_rsq * 
                  (rsq-cut_lj_innersq) * denom_lj_inv;
              }
              //forcelj = forcelj*switch1_lj + philj*switch2_lj;
              //fprintf(stderr,"ACCESS ERROR: reached %s:%d\n",__FILE__,__LINE__);
              //exit(-1);
            }
          } //else clause for "if (rsq > _cut_core_sqd)"
          #endif //#ifdef ENABLE_SOFTCORE2

        } else forcelj = 0.0;

        fpair = (factor_coul*forcecoul + factor_lj*forcelj) * r2inv;

        f[i][0] += delx*fpair;
        f[i][1] += dely*fpair;
        f[i][2] += delz*fpair;
        if (newton_pair || j < nlocal) {
          f[j][0] -= delx*fpair;
          f[j][1] -= dely*fpair;
          f[j][2] -= delz*fpair;
        }

        if (eflag) {
          if (rsq < cut_coulsq) {
            // Recompute the energy (philj) just in case it was
            // not computed above, and set evdwl equal to it.
            q_j = q[j];
            //if ((q_i != 0.0) && (q_j != 0.0))
            ecoul = qqrd2e * q_i * q_j * sqrt(r2inv);
            //else
            //  ecoul = 0.0;
            if (rsq > cut_coul_innersq)
              ecoul *= switch1_coul; //switch1_coul is calculated earlier
                                     //whenever rsq > cut_coul_innersq

            ecoul *= factor_coul;
          } else ecoul = 0.0;
          //fprintf(stderr, "~~~~ switch1_coul=%g, ecoul=%g\n", switch1_coul, ecoul);
          if (rsq < cut_ljsq) {

            #ifdef ENABLE_SOFTCORE2
            if (rsq > _cut_core_sqd)
            #endif

              philj = r6inv*(_lj3*r6inv - _lj4);

            #ifdef ENABLE_SOFTCORE2
            else {
            //  Soft-core repulsive function
              if (core_singular)
                //    Ulj(r) = (_core_coeff/r^2  + _core_offset) * switch1_lj(r)
                philj = _core_coeff * r2inv + _core_offset;
              else
                //    Ulj(r) = (_core_coeff*r^2  + _core_offset) * switch1_lj(r)
                philj = _core_coeff * rsq + _core_offset;
              //fprintf(stderr,"ACCESS ERROR: reached %s:%d\n",__FILE__,__LINE__);
              //exit(-1);
            }
            #endif

            if (rsq > cut_lj_innersq) {
              philj *= switch1_lj; //switch1_lj is calculated earlier
                                   //whenever rsq > cut_lj_innersq
            }
          } else philj = 0.0;

          philj *= factor_lj;
          evdwl = philj;
          //fprintf(stderr, "~~~~ switch1_lj=%g, evdwl=%g\n", switch1_lj,evdwl);
        }

        if (evflag) ev_tally(i,j,nlocal,newton_pair,
                             evdwl,ecoul,fpair,delx,dely,delz);

      } //if (rsq < cut_bothsq)
    } //for (jj = 0; jj < jnum; jj++)
  } //for (ii = 0; ii < jnum; ii++)

  if (vflag_fdotr) virial_fdotr_compute();
} //void PairLJCharmmCoulCharmmInter::compute()


/* ----------------------------------------------------------------------
   allocate all arrays
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::allocate()
{
  allocated = 1;
  int n = atom->ntypes;

  memory->create(cutsq,n+1,n+1,"pair:cutsq");

  // Tables needed for initialization
  memory->create(setflag,n+1,n+1,"pair:setflag");
  for (int i = 1; i <= n; i++)
    for (int j = i; j <= n; j++)
      setflag[i][j] = 0;

  PairLJRemix::allocate();     //this allocates the epsilon, sigma, 
                               //kappa, lambda, lj1, lj2, lj3, lj4 arrays

  PairLJSoftcore2::allocate(); //this allocates the cut_core_sqd, core_coeff, 
                               //core_D_coeff, core_offset arrays

  memory->create(eps14,n+1,n+1,"pair:eps14");
  memory->create(sigma14,n+1,n+1,"pair:sigma14");
  memory->create(kappa14,n+1,n+1,"pair:kappa14");
  memory->create(lambda14,n+1,n+1,"pair:lambda14");
  #ifdef DBG_LJ_REMIX_MEM
  for (int itype=0; itype<=n; itype++) {
    for (int jtype=0; jtype<=n; jtype++) {
      eps14[itype][jtype] = 0.0;
      sigma14[itype][jtype] = 0.0;
      kappa14[itype][jtype] = 0.0;
      lambda14[itype][jtype] = 0.0;
    }
  }
  #endif //#ifdef DBG_LJ_REMIX_MEM

  #ifdef ENABLE_INTER_DIF
  memory->create(epsDIF,n+1,n+1,"pair:epsDIF");
  memory->create(sigmaDIF,n+1,n+1,"pair:sigmaDIF");
  memory->create(kappaDIF,n+1,n+1,"pair:kappaDIF");
  memory->create(lambdaDIF,n+1,n+1,"pair:lambdaDIF");
  #ifdef DBG_LJ_REMIX_MEM
  for (int itype=0; itype<=n; itype++) {
    for (int jtype=0; jtype<=n; jtype++) {
      epsDIF[itype][jtype] = 0.0;
      sigmaDIF[itype][jtype] = 0.0;
      kappaDIF[itype][jtype] = 0.0;
      lambdaDIF[itype][jtype] = 0.0;
    }
  }
  #endif //#ifdef DBG_LJ_REMIX_MEM
  #endif //#ifdef ENABLE_INTER_DIF

  memory->create(lj14_1,n+1,n+1,"pair:lj14_1");
  memory->create(lj14_2,n+1,n+1,"pair:lj14_2");
  memory->create(lj14_3,n+1,n+1,"pair:lj14_3");
  memory->create(lj14_4,n+1,n+1,"pair:lj14_4");
  #ifdef DBG_LJ_REMIX_MEM
  for (int itype=0; itype<=n; itype++) {
    for (int jtype=0; jtype<=n; jtype++) {
      lj14_1[itype][jtype] = 0.0;
      lj14_2[itype][jtype] = 0.0;
      lj14_3[itype][jtype] = 0.0;
      lj14_4[itype][jtype] = 0.0;
    }
  }
  #endif //#ifdef DBG_LJ_REMIX_MEM

  #ifdef ENABLE_INTER_DIF
  memory->create(ljDIF_1,n+1,n+1,"pair:ljDIF_1");
  memory->create(ljDIF_2,n+1,n+1,"pair:ljDIF_2");
  memory->create(ljDIF_3,n+1,n+1,"pair:ljDIF_3");
  memory->create(ljDIF_4,n+1,n+1,"pair:ljDIF_4");
  #ifdef DBG_LJ_REMIX_MEM
  for (int itype=0; itype<=n; itype++) {
    for (int jtype=0; jtype<=n; jtype++) {
      ljDIF_1[itype][jtype] = 0.0;
      ljDIF_2[itype][jtype] = 0.0;
      ljDIF_3[itype][jtype] = 0.0;
      ljDIF_4[itype][jtype] = 0.0;
    }
  }
  #endif //#ifdef DBG_LJ_REMIX_MEM
  #endif //#ifdef ENABLE_INTER_DIF

  #ifdef ENABLE_SOFTCORE2
  // Parameters needed below the softcore cutoff radius:
  #ifdef ENABLE_INTER_DIF
  memory->create(cut_coreDIF_sqd,n+1,n+1,"pair:cut_coreDIF_sqd");
  memory->create(core_coeffDIF,n+1,n+1,"pair:core_coeffDIF");
  memory->create(core_D_coeffDIF,n+1,n+1,"pair:core_D_coeffDIF");
  memory->create(core_offsetDIF,n+1,n+1,"pair:core_offsetDIF");
  #ifdef DBG_LJ_REMIX_MEM
  for (int itype=0; itype<=n; itype++) {
    for (int jtype=0; jtype<=n; jtype++) {
      cut_coreDIF_sqd[itype][jtype] = 0.0;
      core_coeffDIF[itype][jtype]   = 0.0;
      core_D_coeffDIF[itype][jtype] = 0.0;
      core_offsetDIF[itype][jtype]  = 0.0;
    }
  }
  #endif //#ifdef DBG_LJ_REMIX_MEM
  #endif //#ifdef ENABLE_INTER_DIF

  // There's no way to modify 14 behavior without changing dihedral_charmm.cpp
  // Disabling the cutoff behavior for 1-4 interactions, and commenting out:
  //memory->create(cut_core14_sqd,n+1,n+1,"pair:cut_core14_sqd");
  //memory->create(core_coeff14,n+1,n+1,"pair:core_coeff14");
  //memory->create(core_D_coeff14,n+1,n+1,"pair:core_D_coeff14");
  //memory->create(core_offset14,n+1,n+1,"pair:core_offset14");
  #endif //#ifdef ENABLE_SOFTCORE2

} //PairLJCharmmCoulCharmmInter::allocate()


/* ----------------------------------------------------------------------
   global settings
   unlike other pair styles,
     there are no individual pair settings that these override
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::settings(int narg, char **arg)
{
  vector<string> vArgs(narg,"");
  for (int i=0; i<narg; i++)
    vArgs[i] = string(arg[i]);
  settings(vArgs);
}


void PairLJCharmmCoulCharmmInter::settings(vector<string>& vArgs)
{
  PairLJRemix::settings(vArgs);
  PairLJSoftcore2::settings(vArgs);

  bool mi_neq_mj_override = false;

  // All remaining arguments should be numeric

  int i=0;
  while (i < vArgs.size())
  {
    bigint b_mol_lo, b_mol_hi, b_jmol_lo, b_jmol_hi;
    int nargs_delete = 0;
    // Is this argument a number?
    char *pc;
    strtod(vArgs[i].c_str(), &pc);
    if (strlen(pc) != 0) // then vArgs[i] is not a number
    { 
      if (vArgs[i] == "both") {
        //force->boundsbig(FLERR,vArgs[i+1].c_str(),MAXTAGINT-1,mol_lo,mol_hi);
        //force->boundsbig(FLERR,vArgs[i+2].c_str(),MAXTAGINT-1,jmol_lo,jmol_hi);
        boundsbig_cppstr(FLERR,vArgs[i+1],MAXTAGINT-1,b_mol_lo,b_mol_hi);
        boundsbig_cppstr(FLERR,vArgs[i+2],MAXTAGINT-1,b_jmol_lo,b_jmol_hi);
        mol_lo = b_mol_lo;
        mol_hi = b_mol_hi;
        jmol_lo = b_jmol_lo;
        jmol_hi = b_jmol_hi;
        invert_selection = 0;        //= false;
        either_irange_or_jrange = 0; //= false;
        ignore_imol_equals_jmol = 1; //= true;
        nargs_delete = 3;
      }
      else if (vArgs[i] == "nboth") {
        //force->boundsbig(FLERR,vArgs[i+1].c_str(),MAXTAGINT-1,mol_lo,mol_hi);
        //force->boundsbig(FLERR,vArgs[i+2].c_str(),MAXTAGINT-1,jmol_lo,jmol_hi);
        boundsbig_cppstr(FLERR,vArgs[i+1],MAXTAGINT-1,b_mol_lo,b_mol_hi);
        boundsbig_cppstr(FLERR,vArgs[i+2],MAXTAGINT-1,b_jmol_lo,b_jmol_hi);
        mol_lo = b_mol_lo;
        mol_hi = b_mol_hi;
        jmol_lo = b_jmol_lo;
        jmol_hi = b_jmol_hi;
        invert_selection = 1;        //= true;
        either_irange_or_jrange = 0; //= false;
        ignore_imol_equals_jmol = 1; //= true;
        nargs_delete = 3;
      }
      else if (vArgs[i] == "either") {
        //force->boundsbig(FLERR,vArgs[i+1].c_str(),MAXTAGINT-1,mol_lo,mol_hi);
        boundsbig_cppstr(FLERR,vArgs[i+1],MAXTAGINT-1,b_mol_lo,b_mol_hi);
        mol_lo = b_mol_lo;
        mol_hi = b_mol_hi;
        jmol_lo = mol_lo;
        jmol_hi = mol_hi;
        invert_selection = 0;        //= false;
        either_irange_or_jrange = 1; //= true;
        ignore_imol_equals_jmol = 1; //= true;
        nargs_delete = 2;
      }
      else if (vArgs[i] == "neither") {
        //force->boundsbig(FLERR,vArgs[i+1].c_str(),MAXTAGINT-1,mol_lo,mol_hi);
        boundsbig_cppstr(FLERR,vArgs[i+1],MAXTAGINT-1,b_mol_lo,b_mol_hi);
        mol_lo = b_mol_lo;
        mol_hi = b_mol_hi;
        jmol_lo = mol_lo;
        jmol_hi = mol_hi;
        invert_selection = 1;        //= true;
        either_irange_or_jrange = 1; //= true;
        ignore_imol_equals_jmol = 1; //= true;
        nargs_delete = 2;
      }
      else if (vArgs[i] == "nucleate") {
        //force->boundsbig(FLERR,vArgs[i+1].c_str(),MAXTAGINT-1,mol_lo,mol_hi);
        boundsbig_cppstr(FLERR,vArgs[i+1],MAXTAGINT-1,b_mol_lo,b_mol_hi);
        mol_lo = b_mol_lo;
        mol_hi = b_mol_hi;
        jmol_lo = mol_lo;
        jmol_hi = mol_hi;
        invert_selection = 1;        //= true;
        either_irange_or_jrange = 1; //= true;
        ignore_imol_equals_jmol = 0; //= false;
        nargs_delete = 2;
      }
      else if (vArgs[i] == "mi!=mj") {
        //ignore_imol_equals_jmol = 0; //= false;
	mi_neq_mj_override = true;
        nargs_delete = 1;
      }
      else {
        char err_msg[2048];
        sprintf(err_msg, "Unrecognized argument to pair_style command:\n"
                "       \"%s\"\n", vArgs[i].c_str());
        error->all(FLERR,"Unrecognized argument to pair_style command");
      }

      // now delete the argument(s) we just processed
      if (i+nargs_delete > vArgs.size()) {
        error->all(FLERR,
                   "Illegal pair_style command (wrong number of arguments)");
      }
      vArgs.erase(vArgs.begin() + i,
                  vArgs.begin() + i + nargs_delete);

    } //if (strlen(pc) != 0)
    else
      i++;

  } //while (i < vArgs.size())

  if (mi_neq_mj_override)
    ignore_imol_equals_jmol = 0; //= false;

  // What's left over are the numerical arguments:
  if ((vArgs.size() != 2) && (vArgs.size() != 4))
    error->all(FLERR, "Illegal pair_style command (wrong number of arguments)");

  cut_lj_inner = numeric_cppstr(FLERR,vArgs[0]);
  cut_lj = numeric_cppstr(FLERR,vArgs[1]);
  cut_coul_and_lj_differ = 0;
  cut_coul_inner = cut_lj_inner;
  cut_coul = cut_lj;
  if (vArgs.size() == 4) {
    //cut_coul_inner = force->numeric(FLERR,vArgs[2]);
    cut_coul_inner = numeric_cppstr(FLERR,vArgs[2]);
    //cut_coul = force->numeric(FLERR,vArgs[3]);
    cut_coul = numeric_cppstr(FLERR,vArgs[3]);
    if ((vArgs[0] != vArgs[2]) || (vArgs[1] != vArgs[3]))
      cut_coul_and_lj_differ = 1;
  }


  if ((coeff_style == COEFF_EPS_SIG_X_4n4) ||
      (coeff_style == COEFF_EPS_SIG_X_1n2) ||
      (coeff_style == COEFF_EPS_SIG_X_11L) ||
      (coeff_style == COEFF_EPS_SIG_X_44L) ||
      (coeff_style == COEFF_EPS_SIG_X_12L))
  {
    // These coeff styles use U(r) = X*r^2 + offset   for small r < rc
    //                         (where rc=sqrt(aa_cut_core_sqd[i][j]))

    if (core_singular) {
      error->all(FLERR, " \"pair_style lj/charmm/coul/charmm/inter\" command:\n"
                 "This coeff_style assumes you are using a pair potential with a soft core.\n"
                 "(IE. It assumes you want Upair(r) to remain finite at r=0.)\n"
                 "To do that, you must add \"rsoftcore\", to the \"pair_style\" command, followed\n"
                 "by an initial guess for the cutoff radius (in units of sigma). If you\n"
                 "did NOT plan to use a soft core, then use a different coeff_style, such as\n"
                 "\"es\", \"es1-2\", \"eskl\", \"esx4k4l\", or \"es1k2l\"\n");
    }
                
    core_singular = false;

    #ifndef ENABLE_SOFTCORE2 
    {
      char err_msg[2048];
      sprintf(err_msg, "Error in \"pair_style\" command:\n"
              "The \"coeff_style\" you selected indicates you want to use softcore potential.\n"
              "But this version of LAMMPS was not compiled with support softcore potentials.\n"
              "To enable this feature, edit the\n"
              "   \"%s\" and \"pair_lj_softcore2.cpp\" files, and \n"
              
              "uncomment the line(s) containing \"#define ENABLE_SOFTCORE2\"\n"
              "(near the beginning of each file).\n"
              "Then recompile.\n", __FILE__);
      error->all(FLERR, err_msg);
    }
    #endif
  }
  else {
    // All other coeff styles use U(r) = X/r^2 + offset   for small r < rc
    //                         (where rc=sqrt(aa_cut_core_sqd[i][j]))
    if (! core_singular) {
      error->all(FLERR, " \"pair_style lj/charmm/coul/charmm/inter\" command:\n"
                 "The coeff_style option you selected assumes you are NOT planning\n"
                 "to use a potential with a soft core (which is finite at r=0).\n"
                 "Either use \"rsoftcore\", or use a different coeff_style,\n"
                 "such as \"esx\", \"esx1-2\", \"esxl\", or \"esx12l\"\n");
    }
    core_singular = true;
  }

} //void PairLJCharmmCoulCharmmInter::settings()




/* ----------------------------------------------------------------------
   set coeffs for one or more type pairs
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::coeff(int narg, char **arg)
{
  char err_msg[1024];
  if (coeff_style == COEFF_AB) {
    if ((narg != 2+2) && (narg != 4+2) && (narg != 6+2))
      error->all(FLERR,"Incorrect args for pair coefficients");
  }
  else if (UsesABcoeffs() || UsesEScoeffs()) 
    // ((coeff_style == COEFF_AB) ||
    //  (coeff_style == COEFF_EPS_SIG_4n4) ||
    //  (coeff_style == COEFF_EPS_SIG_1n2))
  {
    if ((narg != 2+2) && (narg != 4+2) && (narg != 6+2))
      error->all(FLERR,"Incorrect args for pair coefficients");
  }
  else if (UsesESKcoeffs())
    // ((coeff_style == COEFF_EPS_SIG_X_4n4) ||
    //  (coeff_style == COEFF_EPS_SIG_X_1n2))
  {
    if ((narg != 3+2) && (narg != 6+2) && (narg != 9+2))
      error->all(FLERR,"Incorrect args for pair coefficients");
  }
  else if (UsesESKLcoeffs())
    //((coeff_style == COEFF_EPS_SIG_KL) ||
    // (coeff_style == COEFF_EPS_SIG_4K4L) ||
    // (coeff_style == COEFF_EPS_SIG_1K2L) ||
    // (coeff_style == COEFF_EPS_SIG_X_11L) ||
    // (coeff_style == COEFF_EPS_SIG_X_44L) ||
    // (coeff_style == COEFF_EPS_SIG_X_12L))
  {
    if ((narg != 4+2) && (narg != 8+2) && (narg != 12+2))
      error->all(FLERR,"Incorrect args for pair coefficients");
  }
  else
    assert(false); // should not be reached

  if (!allocated) allocate();

  int ilo,ihi,jlo,jhi;
  force->bounds(FLERR,arg[0],atom->ntypes,ilo,ihi);
  force->bounds(FLERR,arg[1],atom->ntypes,jlo,jhi);

  double A_one = -1.0;
  double B_one = -1.0;
  double kappa_one = 1.0;
  double lambda_one = -1.0;
  double epsilon_one = force->numeric(FLERR,arg[2]);
  double sigma_one = force->numeric(FLERR,arg[3]);

  double A14_one = -1.0;
  double B14_one = -1.0;
  double kappa14_one = 1.0;
  double lambda14_one = -1.0;
  double eps14_one = epsilon_one;
  double sigma14_one = sigma_one;

  #ifdef ENABLE_INTER_DIF
  double ADIF_one = -1.0;
  double BDIF_one = -1.0;
  double kappaDIF_one = 1.0;
  double lambdaDIF_one = -1.0;
  double epsDIF_one = epsilon_one;
  double sigmaDIF_one = sigma_one;
  #endif

  if (coeff_style == COEFF_AB) {
    A_one = force->numeric(FLERR,arg[2]);
    B_one = force->numeric(FLERR,arg[3]);
    A14_one = A_one;
    B14_one = B_one;
    #ifdef ENABLE_INTER_DIF
    ADIF_one = A_one;
    BDIF_one = B_one;
    #endif
    if (narg >= 4+2) {
      A14_one = force->numeric(FLERR,arg[4]);
      B14_one = force->numeric(FLERR,arg[5]);
      if (narg == 6+2) {
        #ifdef ENABLE_INTER_DIF
        ADIF_one = force->numeric(FLERR,arg[6]);
        BDIF_one = force->numeric(FLERR,arg[7]);
        #else
        sprintf(err_msg, "This version of LAMMPS was compiled without support for\n"
                "custom inter-vs-intra-molecular pairwise forces.  To fix this,\n"
                "the \"ENABLE_INTER_DIF\" macro must be defined in source\n"
                "file \"%s\".\n", __FILE__);
        error->all(FLERR, err_msg);
        #endif //#ifdef ENABLE_INTER_DIF
      }
    }
  }
  else if ((coeff_style == COEFF_EPS_SIG_4n4) ||
           (coeff_style == COEFF_EPS_SIG_1n2))
  {
    if (narg >= 4+2) {
      eps14_one = force->numeric(FLERR,arg[4]);
      sigma14_one = force->numeric(FLERR,arg[5]);
      if (narg == 6+2) {
        #ifdef ENABLE_INTER_DIF
        epsDIF_one = force->numeric(FLERR,arg[6]);
        sigmaDIF_one = force->numeric(FLERR,arg[7]);
        #else
        sprintf(err_msg, "This version of LAMMPS was compiled without support for\n"
                "custom inter-vs-intra-molecular pairwise forces.  To fix this,\n"
                "the \"ENABLE_INTER_DIF\" macro must be defined in source\n"
                "file \"%s\".\n", __FILE__);
        error->all(FLERR, err_msg);
        #endif //#ifdef ENABLE_INTER_DIF
      }
    }
  }
  else if ((coeff_style == COEFF_EPS_SIG_X_4n4) ||
           (coeff_style == COEFF_EPS_SIG_X_1n2))
  { 
    kappa_one = force->numeric(FLERR,arg[4]);
    lambda_one = 1.0;

    kappa14_one = force->numeric(FLERR,arg[4]);
    lambda14_one = 1.0;

    #ifdef ENABLE_INTER_DIF
    kappaDIF_one = force->numeric(FLERR,arg[4]);
    lambdaDIF_one = 1.0;
    #endif

    if (narg >= 6+2) {
      eps14_one = force->numeric(FLERR,arg[5]);
      sigma14_one = force->numeric(FLERR,arg[6]);
      kappa14_one = force->numeric(FLERR,arg[7]);
      if (narg == 9+2) {
        #ifdef ENABLE_INTER_DIF
        epsDIF_one = force->numeric(FLERR,arg[8]);
        sigmaDIF_one = force->numeric(FLERR,arg[9]);
        kappaDIF_one = force->numeric(FLERR,arg[10]);
        #else
        sprintf(err_msg, "This version of LAMMPS was compiled without support for\n"
                "custom inter-vs-intra-molecular pairwise forces.  To fix this,\n"
                "the \"ENABLE_INTER_DIF\" macro must be defined in source\n"
                "file \"%s\".\n", __FILE__);
        error->all(FLERR, err_msg);
        #endif //#ifdef ENABLE_INTER_DIF
      }
    }
  }
  else if ((coeff_style == COEFF_EPS_SIG_KL) ||
           (coeff_style == COEFF_EPS_SIG_4K4L) ||
           (coeff_style == COEFF_EPS_SIG_1K2L) ||
           (coeff_style == COEFF_EPS_SIG_X_11L) ||
           (coeff_style == COEFF_EPS_SIG_X_44L) ||
           (coeff_style == COEFF_EPS_SIG_X_12L))
  {
    kappa_one = force->numeric(FLERR,arg[4]);
    lambda_one = force->numeric(FLERR,arg[5]);

    kappa14_one = kappa_one;
    lambda14_one = lambda_one;

    #ifdef ENABLE_INTER_DIF
    kappaDIF_one = kappa_one;
    lambdaDIF_one = lambda_one;
    #endif

    if (narg >= 8+2) {
      eps14_one = force->numeric(FLERR,arg[6]);
      sigma14_one = force->numeric(FLERR,arg[7]);
      kappa14_one = force->numeric(FLERR,arg[8]);
      lambda14_one = force->numeric(FLERR,arg[9]);
      if (narg == 12+2) {
        #ifdef ENABLE_INTER_DIF
        epsDIF_one = force->numeric(FLERR,arg[10]);
        sigmaDIF_one = force->numeric(FLERR,arg[11]);
        kappaDIF_one = force->numeric(FLERR,arg[12]);
        lambdaDIF_one = force->numeric(FLERR,arg[13]);
        #else
        sprintf(err_msg, "This version of LAMMPS was compiled without support for\n"
                "custom inter-vs-intra-molecular pairwise forces.  To fix this,\n"
                "the \"ENABLE_INTER_DIF\" macro must be defined in source\n"
                "file \"%s\".\n", __FILE__);
        error->all(FLERR, err_msg);
        #endif //#ifdef ENABLE_INTER_DIF
      }
    }
  }
  else 
    assert(false); // should not be reached

  int count = 0;
  for (int i = ilo; i <= ihi; i++) {
    for (int j = MAX(jlo,i); j <= jhi; j++) {
      if (coeff_style == COEFF_AB) {
        lj3[i][j] = A_one;
        lj4[i][j] = B_one;
        lj14_3[i][j] = A14_one;
        lj14_4[i][j] = B14_one;
        #ifdef ENABLE_INTER_DIF
        ljDIF_3[i][j] = ADIF_one;
        ljDIF_4[i][j] = BDIF_one;
        #endif
      }
      else{
        epsilon[i][j] = epsilon_one;
        sigma[i][j] = sigma_one;
        kappa[i][j] = kappa_one;
        lambda[i][j] = lambda_one;
        eps14[i][j] = eps14_one;
        sigma14[i][j] = sigma14_one;
        kappa14[i][j] = kappa14_one;
        lambda14[i][j] = lambda14_one;
        #ifdef ENABLE_INTER_DIF
        epsDIF[i][j] = epsDIF_one;
        sigmaDIF[i][j] = sigmaDIF_one;
        kappaDIF[i][j] = kappaDIF_one;
        lambdaDIF[i][j] = lambdaDIF_one;
        #endif
      }
      setflag[i][j] = 1;
      count++;
    }
  }


  if (count == 0) error->all(FLERR,"Incorrect args for pair coefficients");
} //PairLJCharmmCoulCharmmInter::coeff()


/* ----------------------------------------------------------------------
   init specific to this pair style
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::init_style()
{
  if (!atom->q_flag)
    error->all(FLERR,
               "Pair style lj/charmm/coul/charmm/inter requires atom attribute q");

  neighbor->request(this);

  // require cut_lj_inner < cut_lj, cut_coul_inner < cut_coul

  if (((cut_lj_inner >= cut_lj) && (cut_lj_inner > 0.0))
      || 
      ((cut_coul_inner >= cut_coul) && (cut_coul_inner > 0.0)))
  {
    error->all(FLERR,"Pair inner cutoff >= Pair outer cutoff");
  }

  cut_lj_innersq = cut_lj_inner * cut_lj_inner;
  cut_ljsq = cut_lj * cut_lj;
  cut_coul_innersq = cut_coul_inner * cut_coul_inner;
  cut_coulsq = cut_coul * cut_coul;
  cut_bothsq = MAX(cut_ljsq,cut_coulsq);

  denom_lj_inv  = 1.0 / ((cut_ljsq-cut_lj_innersq)*
                         (cut_ljsq-cut_lj_innersq)*
                         (cut_ljsq-cut_lj_innersq));
  denom_coul_inv = 1.0 / ((cut_coulsq-cut_coul_innersq)*
                          (cut_coulsq-cut_coul_innersq)*
                          (cut_coulsq-cut_coul_innersq));
}




/* ----------------------------------------------------------------------
   init for one type pair i,j and corresponding j,i
------------------------------------------------------------------------- */

double PairLJCharmmCoulCharmmInter::init_one(int i, int j)
{
  // Update the intra-molecular Lennard-Jones parameter arrays:
  PairLJRemix::init_one(i, j,
                        epsilon, sigma, kappa, lambda,
                        lj1, lj2, lj3, lj4);

  // Update the intra-molecular 1-4 Lennard-Jones parameter arrays:
  PairLJRemix::init_one(i, j,
                        eps14, sigma14, kappa14, lambda14,
                        lj14_1, lj14_2, lj14_3, lj14_4);

  #ifdef ENABLE_INTER_DIF
  // Update the inter-molecular Lennard-Jones parameter arrays:
  PairLJRemix::init_one(i, j, 
                        epsDIF, sigmaDIF, kappaDIF, lambdaDIF,
                        ljDIF_1, ljDIF_2, ljDIF_3, ljDIF_4);
  #endif



  #ifdef ENABLE_SOFTCORE2

  if ((coeff_style == COEFF_EPS_SIG_X_4n4) ||
      (coeff_style == COEFF_EPS_SIG_X_1n2) ||
      (coeff_style == COEFF_EPS_SIG_X_11L) ||
      (coeff_style == COEFF_EPS_SIG_X_44L) ||
      (coeff_style == COEFF_EPS_SIG_X_12L)) {
    // Then let PairLJSoftcore2::init_one() know
    // that core_offset[i][j] was set manually by the user,
    // and to choose the rest of the parameters accordingly...
    cut_core_sqd[i][j] = -1.0; // an impossible value
    core_offset[i][j] = kappa[i][j]; //note: kappa[i][j] currently stores U(r=0)
    #ifdef ENABLE_INTER_DIF
    cut_coreDIF_sqd[i][j] = -1.0; // an impossible value
    core_offsetDIF[i][j] = kappaDIF[i][j]; //note: kappa[i][j] currently = U(r=0)
    #endif
  }

  PairLJSoftcore2::init_one(i, j, 
                            sigma,
                            cut_core_sqd,
                            core_coeff,
                            core_D_coeff,
                            core_offset,
                            lj3,
                            lj4);

  #ifdef ENABLE_INTER_DIF
  PairLJSoftcore2::init_one(i, j, 
                            sigmaDIF,
                            cut_coreDIF_sqd,
                            core_coeffDIF,
                            core_D_coeffDIF,
                            core_offsetDIF,
                            ljDIF_3,
                            ljDIF_4);
  #endif

  // There's no way to modify 14 behavior without changing dihedral_charmm.cpp
  // Disabling the cutoff behavior for 1-4 interactions, and commenting out:
  //PairLJSoftcore2::init_one(i, j, 
  //                          sigma14,
  //                          cut_core14_sqd,
  //                          core_coeff14,
  //                          core_D_coeff14,
  //                          core_offset14);
  //                          lj14_3,
  //                          lj14_4);


  if ((cut_core_sigma > 0.0) && (coeff_style != COEFF_AB))
  {
    // Here we try to insure that 1-4 interactions are explicitly 
    // turned off by the user whenever "core" cutoffs are in use.
    if ((lj14_3[i][j] > 1.0e-7 * fabs(lj3[i][j])) ||
        (lj14_4[i][j] > 1.0e-7 * fabs(lj4[i][j]))) {
      error->all(FLERR, 
                 "If you plan to use \"soft\" U0-k*r^2 or U0+k/r^2 potentials at small r,\n"
                 "then you must explicitly set all 1-4 interaction parameters to 0.\n"
                 "(Reason: These interactions are calculated by dihedral_charmm.cpp, which was\n"
                 "         written by someone else.  That code does not know about the \n"
                 "         U0-k*r^2 or U0+k/r^2 approximations at small r.)\n"
                 "\n"
                 "YOU CAN NOT OMIT THE 1-4 PARAMETERS.  You must set them to 0.\n"
                 "\n"
                 "IF YOU NEED 1-4 INTERACTIONS, then use the \"special_bonds\" command.\n"
                 "(For example \"special_bonds lj/coul 0.0 0.0 0.5\". This will use the ordinary\n"
                 "epsilon and sigma parameters, and weight the final interaction by 0.5.)\n"
                 "\n"
                 "I left it this way intentionally so that you would see this error message\n"
                 "and use the \"special_bonds\" command to customize 1-4 interactions\n");
    }
  }

  //   I can't think of any reason the core cutoff should
  //   ever be allowed to exceed any of the other lennard-jones cutoffs.
  //   I might change my mind later and uncomment this check below:
  //   -Andrew 2014-6-12
  //
  if (cut_core_sqd[i][j] > cut_lj_innersq) {
    char err_msg[4096];
    sprintf(err_msg, "Core cuttoff for atom types %d & %d exceeds the inner LJ switch cutoff radius\n", i,j);
    error->all(FLERR, err_msg);
  }

  #endif //#ifdef ENABLE_SOFTCORE2

  double cut = MAX(cut_lj, cut_coul);

  return cut;
} //PairLJCharmmCoulCharmmInter::init_one(int, int)



/* ----------------------------------------------------------------------
  proc 0 writes to restart file
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::write_restart(FILE *fp)
{
  write_restart_settings(fp);

  //Just write the raw lennard jones AB parameters.  Don't write the epsilon,
  //sigma, and lambda parameters (because these are interpreted in a way which 
  //depends on "coeff_style").

  int i,j;
  for (i = 1; i <= atom->ntypes; i++) {
    for (j = i; j <= atom->ntypes; j++) {
      fwrite(&setflag[i][j],sizeof(int),1,fp);

      if (setflag[i][j]) {
        PairLJRemix::write_restart_pair(fp,
                                        i, j, 
                                        epsilon, sigma, 
                                        kappa, lambda,
                                        lj3, lj4);

        PairLJRemix::write_restart_pair(fp,
                                        i, j, 
                                        eps14, sigma14, 
                                        kappa14, lambda14,
                                        lj14_3, lj14_4);

        #ifdef ENABLE_INTER_DIF
        PairLJRemix::write_restart_pair(fp,
                                        i, j, 
                                        epsDIF, sigmaDIF, 
                                        kappaDIF, lambdaDIF,
                                        ljDIF_3, ljDIF_4);
        #endif


        #ifdef ENABLE_SOFTCORE2
        PairLJSoftcore2::write_restart_pair(fp,
                                            i, j,
                                            cut_core_sqd,
                                            core_coeff,
                                            core_D_coeff,
                                            core_offset);
        #ifdef ENABLE_INTER_DIF
        PairLJSoftcore2::write_restart_pair(fp,
                                            i, j,
                                            cut_coreDIF_sqd,
                                            core_coeffDIF,
                                            core_D_coeffDIF,
                                            core_offsetDIF);
        #endif //#ifdef ENABLE_INTER_DIF
        #endif //#ifdef ENABLE_SOFTCORE2
        // There's no way to modify 14 behavior without changing 
        // dihedral_charmm.cpp.  Disabling the cutoff behavior for 1-4 
        // interactions, and commenting out:
        //PairLJSoftcore2::write_restart_pair(fp,
        //                                    i, j,
        //                                    cut_core14_sqd,
        //                                    core_coeff14,
        //                                    core_D_coeff14,
        //                                    core_offset14);

      } //if (setflag[i][j])
    } //for (j = i; j <= atom->ntypes; j++)
  } //for (i = 1; i <= atom->ntypes; i++)
} //PairLJCharmmCoulCharmmInter::write_restart(FILE *)



/* ----------------------------------------------------------------------
  proc 0 reads from restart file, bcasts
------------------------------------------------------------------------- */


void PairLJCharmmCoulCharmmInter::read_restart(FILE *fp)
{
  read_restart_settings(fp);

  allocate();

  //Just read the raw lennard jones AB parameters.  Don't read the epsilon,
  //sigma, and lambda parameters (because these are interpreted in a way which 
  //depends on "coeff_style").

  int i,j;
  int me = comm->me;
  for (i = 1; i <= atom->ntypes; i++) {
    for (j = i; j <= atom->ntypes; j++) {
      if (me == 0) fread(&setflag[i][j],sizeof(int),1,fp);
      MPI_Bcast(&setflag[i][j],1,MPI_INT,0,world);
      if (setflag[i][j]) {
          PairLJRemix::read_restart_pair(fp,
                                         i, j,
                                         epsilon, sigma, kappa, lambda,
                                         lj3, lj4);
          PairLJRemix::read_restart_pair(fp,
                                         i, j,
                                         eps14, sigma14, kappa14, lambda14,
                                         lj14_3, lj14_4);
          #ifdef ENABLE_INTER_DIF
          PairLJRemix::read_restart_pair(fp,
                                         i, j,
                                         epsDIF, sigmaDIF, kappaDIF, lambdaDIF,
                                         ljDIF_3, ljDIF_4);
          #endif //#ifdef ENABLE_INTER_DIF



          #ifdef ENABLE_SOFTCORE2
          PairLJSoftcore2::read_restart_pair(fp,
                                             i, j,
                                             cut_core_sqd,
                                             core_coeff,
                                             core_D_coeff,
                                             core_offset);

          #ifdef ENABLE_INTER_DIF
          PairLJSoftcore2::read_restart_pair(fp,
                                             i, j,
                                             cut_coreDIF_sqd,
                                             core_coeffDIF,
                                             core_D_coeffDIF,
                                             core_offsetDIF);
          #endif
          // There's no way to modify 14 behavior without changing 
          // dihedral_charmm.cpp.  Disabling the cutoff behavior for 1-4 
          // interactions, and commenting out:
          //PairLJSoftcore2::read_restart_pair(fp,
          //                                   i, j,
          //                           cut_coreDIF_sqd,
          //                           core_coeffDIF,
          //                           core_D_coeffDIF,
          //                           core_offsetDIF);
          #endif //#ifdef ENABLE_SOFTCORE2

      } //if (setflag[i][j])
    } //for (j = i; j <= atom->ntypes; j++)
  } //for (i = 1; i <= atom->ntypes; i++)
} //PairLJCharmmCoulCharmmInter::read_restart(FILE *)

/* ----------------------------------------------------------------------
  proc 0 writes to restart file
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::write_restart_settings(FILE *fp)
{
  PairLJRemix::write_restart_settings(fp);
  #ifdef ENABLE_SOFTCORE2
  PairLJSoftcore2::write_restart_settings(fp);
  #endif

  fwrite(&cut_lj_inner,sizeof(double),1,fp);
  fwrite(&cut_lj,sizeof(double),1,fp);
  fwrite(&cut_coul_inner,sizeof(double),1,fp);
  fwrite(&cut_coul,sizeof(double),1,fp);
  fwrite(&offset_flag,sizeof(int),1,fp);
  fwrite(&mix_flag,sizeof(int),1,fp);
  fwrite(&cut_coul_and_lj_differ,sizeof(int),1,fp);
  fwrite(&invert_selection,sizeof(int),1,fp);
  fwrite(&either_irange_or_jrange,sizeof(int),1,fp);
  fwrite(&ignore_imol_equals_jmol,sizeof(int),1,fp);
  fwrite(&mol_lo,sizeof(bigint),1,fp);
  fwrite(&mol_hi,sizeof(bigint),1,fp);
  fwrite(&jmol_lo,sizeof(bigint),1,fp);
  fwrite(&jmol_hi,sizeof(bigint),1,fp);
}


/* ----------------------------------------------------------------------
  proc 0 reads from restart file, bcasts
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::read_restart_settings(FILE *fp)
{
  PairLJRemix::read_restart_settings(fp);
  #ifdef ENABLE_SOFTCORE2
  PairLJSoftcore2::read_restart_settings(fp);
  #endif

  int me = comm->me;
  if (me == 0) {
    fread(&cut_lj_inner,sizeof(double),1,fp);
    fread(&cut_lj,sizeof(double),1,fp);
    fread(&cut_coul_inner,sizeof(double),1,fp);
    fread(&cut_coul,sizeof(double),1,fp);
    fread(&offset_flag,sizeof(int),1,fp);
    fread(&mix_flag,sizeof(int),1,fp);
    fread(&cut_coul_and_lj_differ,sizeof(int),1,fp);
    fread(&invert_selection,sizeof(int),1,fp);
    fread(&either_irange_or_jrange,sizeof(int),1,fp);
    fread(&ignore_imol_equals_jmol,sizeof(int),1,fp);
    fread(&mol_lo,sizeof(bigint),1,fp);
    fread(&mol_hi,sizeof(bigint),1,fp);
    fread(&jmol_lo,sizeof(bigint),1,fp);
    fread(&jmol_hi,sizeof(bigint),1,fp);
  } //if (me == 0)
  MPI_Bcast(&cut_lj_inner,1,MPI_DOUBLE,0,world);
  MPI_Bcast(&cut_lj,1,MPI_DOUBLE,0,world);
  MPI_Bcast(&cut_coul_inner,1,MPI_DOUBLE,0,world);
  MPI_Bcast(&cut_coul,1,MPI_DOUBLE,0,world);
  MPI_Bcast(&offset_flag,1,MPI_INT,0,world);
  MPI_Bcast(&mix_flag,1,MPI_INT,0,world);
  MPI_Bcast(&cut_coul_and_lj_differ,1,MPI_INT,0,world);
  MPI_Bcast(&invert_selection,1,MPI_INT,0,world);
  MPI_Bcast(&either_irange_or_jrange,1,MPI_INT,0,world);
  MPI_Bcast(&ignore_imol_equals_jmol,1,MPI_INT,0,world);
  MPI_Bcast(&mol_lo,1,MPI_LMP_BIGINT,0,world);
  MPI_Bcast(&mol_hi,1,MPI_LMP_BIGINT,0,world);
  MPI_Bcast(&jmol_lo,1,MPI_LMP_BIGINT,0,world);
  MPI_Bcast(&jmol_hi,1,MPI_LMP_BIGINT,0,world);
} //PairLJSoftcore2::read_restart_pair()

/* ----------------------------------------------------------------------
   proc 0 writes to data file
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::write_data(FILE *fp)
{
  if (UsesABcoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      fprintf(fp,"%d  %g %g  %g %g  %g %g\n",
              i,
              lj3[i][i], lj4[i][i], 
              lj14_3[i][i], lj14_4[i][i], 
              ljDIF_3[i][i], ljDIF_4[i][i]);
    }
  }
  else if (UsesEScoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      fprintf(fp,"%d  %g %g  %g %g  %g %g\n",
              i,
              epsilon[i][i], sigma[i][i],
              eps14[i][i], sigma14[i][i],
              epsDIF[i][i], sigmaDIF[i][i]);
    }
  }
  else if (UsesESKcoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      fprintf(fp,"%d  %g %g %g  %g %g %g  %g %g %g\n",
              i,
              epsilon[i][i], sigma[i][i], kappa[i][i],
              eps14[i][i], sigma14[i][i], kappa14[i][i],
              epsDIF[i][i], sigmaDIF[i][i], kappaDIF[i][i]);
    }
  }
  else if (UsesESKLcoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      fprintf(fp,"%d  %g %g %g %g  %g %g %g %g  %g %g %g %g\n",
              i,
              epsilon[i][i], sigma[i][i], kappa[i][i], lambda[i][i],
              eps14[i][i], sigma14[i][i], kappa14[i][i], lambda14[i][i],
              epsDIF[i][i], sigmaDIF[i][i], kappaDIF[i][i], lambdaDIF[i][i]);
    }
  }
} //PairLJCharmmCoulCharmm::write_data_all()

/* ----------------------------------------------------------------------
   proc 0 writes all pairs to data file
------------------------------------------------------------------------- */

void PairLJCharmmCoulCharmmInter::write_data_all(FILE *fp)
{
  if (UsesABcoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      for (int j=i; j <= atom->ntypes; j++) {
        fprintf(fp,"%d %d  %g %g  %g %g  %g %g\n",
                i, j,
                lj3[i][j], lj4[i][j], 
                lj14_3[i][j], lj14_4[i][j], 
                ljDIF_3[i][j], ljDIF_4[i][j]);
      }
    }
  }
  else if (UsesEScoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      for (int j=i; j <= atom->ntypes; j++) {
        fprintf(fp,"%d %d  %g %g  %g %g  %g %g\n",
                i, j,
                epsilon[i][j], sigma[i][j],
                eps14[i][j], sigma14[i][j],
                epsDIF[i][j], sigmaDIF[i][j]);
      }
    }
  }
  else if (UsesESKcoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      for (int j=i; j <= atom->ntypes; j++) {
        fprintf(fp,"%d %d  %g %g %g  %g %g %g  %g %g %g\n",
                i, j,
                epsilon[i][j], sigma[i][j], kappa[i][j],
                eps14[i][j], sigma14[i][j], kappa14[i][j],
                epsDIF[i][j], sigmaDIF[i][j], kappaDIF[i][j]);
      }
    }
  }
  else if (UsesESKLcoeffs()) {
    for (int i=1; i <= atom->ntypes; i++) {
      for (int j=i; j <= atom->ntypes; j++) {
        fprintf(fp,"%d %d  %g %g %g %g  %g %g %g %g  %g %g %g %g\n",
                i, j,
                epsilon[i][j], sigma[i][j], kappa[i][j], lambda[i][j],
                eps14[i][j], sigma14[i][j], kappa14[i][j], lambda14[i][j],
                epsDIF[i][j], sigmaDIF[i][j], kappaDIF[i][j], lambdaDIF[i][j]);
      }
    }
  }
} //PairLJCharmmCoulCharmm::write_data_all()



/* ---------------------------------------------------------------------- */

double PairLJCharmmCoulCharmmInter::single(int i, int j, int itype, int jtype,
                                           double rsq,
                                           double factor_coul, double factor_lj,
                                           double &fforce)
{
  double r2inv,r6inv,forcecoul,forcelj,phicoul,philj;
  double switch1_coul, switch2_coul, switch1_lj, switch2_lj;

  double _lj1, _lj2, _lj3, _lj4;
  double _cut_core_sqd, _core_coeff, _core_D_coeff, _core_offset;

  #ifdef ENABLE_INTER_DIF
  if (MolPairUseDIF(atom->molecule[i],
		    atom->molecule[j]))
  {
    _lj1 = ljDIF_1[itype][jtype];
    _lj2 = ljDIF_2[itype][jtype];
    _lj3 = ljDIF_3[itype][jtype];
    _lj4 = ljDIF_4[itype][jtype];
    #ifdef ENABLE_SOFTCORE2
    _cut_core_sqd = cut_coreDIF_sqd[itype][jtype];
    _core_coeff   =   core_coeffDIF[itype][jtype];
    _core_D_coeff = core_D_coeffDIF[itype][jtype];
    _core_offset  =  core_offsetDIF[itype][jtype];
    #endif //#ifdef ENABLE_SOFTCORE2
  }
  else
  #endif //#ifdef ENABLE_INTER_DIF
  {
    _lj1 = lj1[itype][jtype];
    _lj2 = lj2[itype][jtype];
    _lj3 = lj3[itype][jtype];
    _lj4 = lj4[itype][jtype];

    #ifdef ENABLE_SOFTCORE2
    _cut_core_sqd = cut_core_sqd[itype][jtype];
    _core_coeff   =   core_coeff[itype][jtype];
    _core_D_coeff = core_D_coeff[itype][jtype];
    _core_offset  =  core_offset[itype][jtype];
    #endif
  }


  r2inv = 1.0/rsq;
  forcecoul = 0.0;
  phicoul = 0.0;
  if (rsq < cut_coulsq) {
    forcecoul = force->qqrd2e * atom->q[i]*atom->q[j]*sqrt(r2inv);
    phicoul   = force->qqrd2e * atom->q[i]*atom->q[j]*sqrt(r2inv);
    if (rsq > cut_coul_innersq) {
      double cutcoulsq_minus_rsq = (cut_coulsq-rsq);
      double cutcoulsq_minus_rsq_sq = cutcoulsq_minus_rsq * cutcoulsq_minus_rsq;
      switch1_coul = cutcoulsq_minus_rsq_sq *
          (cut_coulsq + 2.0*rsq - 3.0*cut_coul_innersq) * denom_coul_inv;
      switch2_coul = 12.0*rsq * cutcoulsq_minus_rsq * 
          (rsq-cut_coul_innersq) * denom_coul_inv;
      forcecoul *= switch1_coul + switch2_coul;
      phicoul   *= switch1_coul;
    }
  }

  if (rsq < cut_ljsq) {
    r6inv = r2inv*r2inv*r2inv;

    #ifdef ENABLE_SOFTCORE2
    if (rsq > _cut_core_sqd) { //--> Ulj(r)=(_lj3/r^12- _lj4/r^6)*switch1_lj
    #endif

      forcelj = r6inv * (_lj1*r6inv - _lj2);
      philj   = r6inv * (_lj3*r6inv - _lj4);

    #ifdef ENABLE_SOFTCORE2
    }
    else { // otherwise Ulj(r) = (_core_coeff/r^2  + _core_offset)*switch1lj
      if (core_singular) {
        //    Ulj(r) = (_core_coeff/r^2  + _core_offset) * switch1_lj(r)
        forcelj = _core_D_coeff * r2inv; //(will be divided by rij later)
        philj   = _core_coeff * r2inv + _core_offset;
      }
      else {
        //    Ulj(r) = (_core_coeff*r^2  + _core_offset) * switch1_lj(r)
        forcelj = _core_D_coeff * rsq;   //(will be divided by rij later)
        philj   = _core_coeff * rsq + _core_offset;
      }
      //fprintf(stderr,"ACCESS ERROR: reached %s:%d\n",__FILE__,__LINE__);
      //exit(-1);
    } //else clause for "if (rsq > _cut_core_sqd)"
    #endif
    if (rsq > cut_lj_innersq) {
      double switch1_lj = switch1_coul;
      double switch2_lj = switch2_coul;
      if (cut_coul_and_lj_differ) { // then recompute switch1 & switch2
        double cutljsq_minus_rsq = (cut_ljsq-rsq);
        double cutljsq_minus_rsq_sq = cutljsq_minus_rsq * cutljsq_minus_rsq;
        switch1_lj = cutljsq_minus_rsq_sq *
                  (cut_ljsq + 2.0*rsq - 3.0*cut_lj_innersq) * denom_lj_inv;
        switch2_lj = 12.0*rsq * cutljsq_minus_rsq * 
                  (rsq-cut_lj_innersq) * denom_lj_inv;
      }
      forcelj = forcelj*switch1_lj + philj*switch2_lj;
      philj  *= switch1_lj;
    }
  } else forcelj = 0.0;

  fforce = (factor_coul*forcecoul + factor_lj*forcelj) * r2inv;
  double eng = factor_coul*phicoul + factor_lj*philj;
  return eng;
} //PairLJCharmmCoulCharmmInter::single()

/* ---------------------------------------------------------------------- */

void *PairLJCharmmCoulCharmmInter::extract(const char *str, int &dim)
{
  dim = 2;
  if (strcmp(str,"lj14_1") == 0) return (void *) lj14_1;
  if (strcmp(str,"lj14_2") == 0) return (void *) lj14_2;
  if (strcmp(str,"lj14_3") == 0) return (void *) lj14_3;
  if (strcmp(str,"lj14_4") == 0) return (void *) lj14_4;
  // There's no way to modify 14 behavior without changing dihedral_charmm.cpp.
  // Disabling the cutoff behavior for 1-4 interactions, and commenting out:
  //if (strcmp(str,"cut_core14_sqd") == 0) return (void *) cut_core14_sqd;
  //if (strcmp(str,"core_coeff14") == 0) return (void *) core_coeff14;
  //if (strcmp(str,"core_D_coeff14") == 0) return (void *) core_D_coeff14;
  //if (strcmp(str,"core_offset14") == 0) return (void *) core_offset14;

  dim = 0;
  if (strcmp(str,"implicit") == 0) return (void *) &implicit;

  return NULL;
}


