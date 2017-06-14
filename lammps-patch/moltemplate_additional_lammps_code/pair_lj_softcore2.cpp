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

#include <cstdlib>
#include <cstring>
#include <cassert>
using namespace std;
#include "atom.h"
#include "comm.h"
#include "force.h"
#include "memory.h"
#include "error.h"
#include "pair_lj_softcore2.h"
#define NDEBUG //(disable assert())
#define ENABLE_SOFTCORE2

using namespace LAMMPS_NS;


PairLJSoftcore2::
PairLJSoftcore2(LAMMPS *lmp) : PairLJRemix(lmp)
{
  cut_core_sigma = -1.0;   // Dissable the softcore cutoff (default: disabled)
  core_singular = 0;       // potential energy diverges as r->0 if cut_core_sigma>0 (default: no)
  ignore_large_rcore = 0;  // Generate error messages if cutoff is too large (default: no)
}


PairLJSoftcore2::
~PairLJSoftcore2()
{
  #ifdef ENABLE_SOFTCORE2
  if (allocated) {
    memory->destroy(cut_core_sqd);
    memory->destroy(core_coeff);
    memory->destroy(core_D_coeff);
    memory->destroy(core_offset);
    //fprintf(stderr, "~~~~ invoking ~PairLJSoftcore2 check successful\n");
  }
  #endif
}

void 
PairLJSoftcore2::
allocate() {
  #ifdef ENABLE_SOFTCORE2
  int n = atom->ntypes;
  memory->create(cut_core_sqd,n+1,n+1,"pair:cut_core_sqd");
  memory->create(core_coeff,n+1,n+1,"pair:core_coeff");
  memory->create(core_D_coeff,n+1,n+1,"pair:core_D_coeff");
  memory->create(core_offset,n+1,n+1,"pair:core_offset");

  #ifdef DBG_LJ_REMIX_MEM
  for (int itype=0; itype<=n; itype++) {
    for (int jtype=0; jtype<=n; jtype++) {
      cut_core_sqd[itype][jtype] = 0.0;
      core_coeff[itype][jtype]   = 0.0;
      core_D_coeff[itype][jtype] = 0.0;
      core_offset[itype][jtype]  = 0.0;
    }
  }
  #endif //#ifdef DBG_LJ_REMIX_MEM

  #endif //#ifdef ENABLE_SOFTCORE2
}


void PairLJSoftcore2::
settings(vector<string>& vArgs)
{
  int i=0;
  while (i < vArgs.size())
  {
    int nargs_delete = 0;
    // Is this argument a number?
    char *pc;
    strtod(vArgs[i].c_str(), &pc);
    if (strlen(pc) != 0) // then vArgs[i] is not a number
    { 
      if ((vArgs[i] == "rcore") ||
          (vArgs[i] == "rsoftcore") ||
          (vArgs[i] == "roverride"))
      {

        char err_msg[1024];
        #ifdef ENABLE_SOFTCORE2
        if (i+1 >= vArgs.size()) {
          sprintf(err_msg,"The \"%s\" keyword must be followed by a number.\n"
                  "(the ratio of the core cutoff radius divided by sigma)\n",
                  vArgs[i].c_str());
          error->all(FLERR, err_msg);
        }

        ignore_large_rcore = 0;
        if (vArgs[i] == "roverride")
          ignore_large_rcore = 1;

        if (vArgs[i] == "rcore")
          core_singular = 1;
        else if (vArgs[i] == "rsoftcore")
          core_singular = 0;

        //cut_core_sigma = force->numeric(FLERR,vArgs[i+1]);
        cut_core_sigma = numeric_cppstr(FLERR,vArgs[i+1]);
        nargs_delete = 2;
        #else
        sprintf(err_msg, "This version of LAMMPS was not compiled with support for \"rsoftcore\".\n"
                "To enable softcore potentials, edit the\n"
                "   \"%s\" file, and\n"
                
                "uncomment the line containing \"#define ENABLE_SOFTCORE2\"\n"
                "(near the beginning of the file) and recompile\n", __FILE__);
        error->all(FLERR, err_msg);
        #endif //#ifdef ENABLE_SOFTCORE2
      } //if ((vArgs[i] == "rsoftcore") || ...
      else
        i++;


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

  #ifdef ENABLE_SOFTCORE2
  if ((cut_core_sigma > 0.0) && (coeff_style == COEFF_AB))
    error->all(FLERR, "pair_style arguments clash.  You can not specify a core cutoff distance\n"
               "if you are using the \"ab\" style.  (The core cutoff is in units of sigma.)\n");
  #endif
}


void PairLJSoftcore2::
init_one(int i, int j,
         double **aa_sigma,
         double **aa_cut_core_sqd,
         double **aa_core_coeff,
         double **aa_core_D_coeff,
         double **aa_core_offset,
         double **aa_lj3,
         double **aa_lj4)
{
  #ifdef ENABLE_SOFTCORE2
  if (cut_core_sigma > 0.0) {
    if (coeff_style == COEFF_AB) {
      char err_msg[1024];
      sprintf(err_msg, 
              "You have requested a softcore potential (\"rsoftcore\" or \"roverride\")\n"
              "but you appear to be using an incompatible coeff_style (such as \"ab\").\n"
              "You must use a different coeff_style which allows you to explicitly\n"
              "specify the \"sigma\" parameter, such as \"eskl\", or \"es4-4\".\n"
              "See the documentation for this pair style.\n");
      error->all(FLERR, err_msg);
    }

    if (aa_cut_core_sqd[i][j] >= 0.0) {
      // rc = cut_core_sigma * sigma
      //
      // The potential and derivative should be continuous at rc
      //                 core_coeff                       lj3            lj4
      //    U(rc)    =   ---------- + core_offset   =    ------    -   ------
      //                    rc^2                          rc^a          rc^b
      //
      // - dU(r)|       2*core_coeff                     a * lj3        b * lj4
      //   -----|    =   ------------               =    --------   -   -------
      //    dr  |rc         rc^3                         rc^(a+1)       rc^(b+1)
      //
      //
      // -->  core_coeff  =  (a/2) * (lj3/rc^(a-2))  -  (b/2) * (lj4/rc^(b-2))
      // -->  core_offset =  lj3/rc^a - lj4/rc^b - core_coeff/rc^2
      double rc;
      bool core_is_large = false;

      rc                       = cut_core_sigma * fabs(aa_sigma[i][j]);
      aa_cut_core_sqd[i][j]    = rc * rc;
      aa_core_coeff[i][j]=((exponent_a/2.0)*aa_lj3[i][j]/pow(rc,(exponent_a-2.0)))
                       + ((-exponent_b/2.0)*aa_lj4[i][j]/pow(rc,(exponent_b-2.0)));
      aa_core_offset[i][j]     = (aa_lj3[i][j]/pow(rc,exponent_a))
                               + (-aa_lj4[i][j]/pow(rc,exponent_b)) 
                               + (-aa_core_coeff[i][j]/(rc*rc));
      aa_core_D_coeff[i][j]    = 2 * aa_core_coeff[i][j];    
      if (aa_core_coeff[i][j] < 0.0) 
        core_is_large = true;

      //   Error checks:

      if (core_is_large && (! ignore_large_rcore))
      {
          char err_msg[1024];
          sprintf(err_msg, "You have an attractive singularity at r=0 (between atom types %d & %d).\n"
                  "This is probably because the cutoff radius for your soft core repulsion\n"
                  "(\"rsoftcore\" argument) is too large.  (Yours is %g x sigma=%g)\n"
                  "Remember that the number after the \"rsoftcore\" keyword should be in units\n"
                  "of \"sigma\", not in natural physical units (like Angstroms).\n"
                  "\n"
                  "(If you know what you are doing and wish to override this warming\n"
                  "then replace \"rsoftcore\" with \"roverride\" before the numeric argument.)\n", 
                  i, j, cut_core_sigma, rc);
          error->all(FLERR, err_msg);
      }

    } //if (aa_cut_core_sqd[i][j] > 0.0)
    else 
    {
      // rc = the cutoff radius = ?    (must solve for it)
      //
      // The potential and derivative should be continuous at rc
      //
      //                                                     lj3        lj4
      //    U(rc)  =  core_coeff * rc^2 + core_offset   =   ------  -  ------
      //                                                     rc^a       rc^b
      //
      //
      // - dU(r)|                                  a * lj3        b * lj4
      //   -----|   =  -2*core_coeff * rc    =    ---------   -   --------
      //    dr  |rc                                rc^(a+1)       rc^(b+1)
      //
      //
      // Setting the two expresions for "core_coeff*rc^2" equal yields:
      //
      //     lj3        lj4                      (-a/2) * lj3      (-b/2) * lj4
      //     ----   -   ----  - core_offset  =   ------------   -  ------------
      //     rc^a       rc^b                         rc^a              rc^b
      //
      // Iterate until you find the value of "rc" which satisfies this

      double core_offset = aa_core_offset[i][j]; //(assume the caller set this)
      double rc = cut_core_sigma * fabs(aa_sigma[i][j]);
      double convergence_threshold = 1e-07 * rc;
      double rc_hi = 2.0 * rc;
      double rc_lo = 0.0;
      bool lhs_lt_rhs_first = true;
      bool lhs_lt_rhs = true;
      bool first_iter = true;
      while (rc_hi - rc_lo > convergence_threshold) {
        double rca = pow(rc, exponent_a);
        double rcb = pow(rc, exponent_b);
        double LHS = (aa_lj3[i][j]/rca) + (-aa_lj4[i][j]/rcb) - core_offset;
        double RHS = -0.5 * ((exponent_a * aa_lj3[i][j]/rca) +
                             (-exponent_b * aa_lj4[i][j]/rcb));

        lhs_lt_rhs = (LHS < RHS);
        if (first_iter) {
          lhs_lt_rhs_first = lhs_lt_rhs;
          first_iter = false;
        }

        if (lhs_lt_rhs == lhs_lt_rhs_first) {
          // then rc is too big
          rc_hi = 0.5 * (rc_lo + rc_hi);
          //rc_lo is unchanged;
        }
        else {
          // then rc is too small
          rc_lo = 0.5 * (rc_lo + rc_hi);
          //rc_hi is unchanged;
        }
        assert(rc_hi > rc_lo);
        rc = 0.5 * (rc_lo + rc_hi);
      } //while (rc_hi - rc_lo > convergence_threshold)

      if (rc < convergence_threshold) {
        char err_msg[2048];
        sprintf(err_msg, "Error in \"softcore\" parameters for\n"
                "       interactions between atom types %d and %d.\n"
                "Unable to smoothly join U(r)=C*r^2 + X to the Lennard-Jones curve\n"
                "(to keep U(r) and dU/dr continuous).\n"
                "This is probably a sign error in the other parameters (eg X or L).\n"
                "Also try increasing the \"rsoftcore\" parameter (currently %g),\n"
                "or try changing the \"X\" parameter (currently %g)\n",
                i, j, cut_core_sigma, core_offset);
        error->all(FLERR, err_msg);
      }

      aa_cut_core_sqd[i][j]    = rc * rc;

      // - dU(r)|                                  a * lj3        b * lj4
      //   -----|   =  -2*core_coeff * rc    =    ---------   -   --------
      //    dr  |rc                                rc^(a+1)       rc^(b+1)

      aa_core_coeff[i][j]=((-exponent_a/2.0)*aa_lj3[i][j]/pow(rc,(exponent_a+2.0)))
                        +  ((exponent_b/2.0)*aa_lj4[i][j]/pow(rc,(exponent_b+2.0)));
      aa_core_D_coeff[i][j]    = -2.0 * aa_core_coeff[i][j];    

      char status_msg[2048];
      sprintf(status_msg, "Softcore: Upair_%d_%d(r) = %g * r^2 + %g   when r < %g\n",
              i, j, aa_core_coeff[i][j], aa_core_offset[i][j], rc);
      if (logfile)
        fprintf(logfile, status_msg);
      if (screen)
        fprintf(screen, status_msg);
    } //else clause for "if (aa_cut_core_sqd[i][j] >= 0.0)"

  } //if ((cut_core_sigma > 0.0) && (coeff_style != COEFF_AB))

  else {
    aa_cut_core_sqd[i][j] = -1.0;
  }

  if (cut_core_sigma > 0.0) {
    // i <--> j for params for forces at distances below the "core" cutoff
    aa_cut_core_sqd[j][i]  = aa_cut_core_sqd[i][j];
    aa_core_coeff[j][i]    = aa_core_coeff[i][j];
    aa_core_D_coeff[j][i]  = aa_core_D_coeff[i][j];
    aa_core_offset[j][i]   = aa_core_offset[i][j];
  }
  #endif //#ifdef ENABLE_SOFTCORE2
} //PairLJSoftcore2::init_one(i)


void PairLJSoftcore2::
write_restart_settings(FILE *fp)
{
  #ifdef ENABLE_SOFTCORE2
  fwrite(&cut_core_sigma,sizeof(double),1,fp);
  fwrite(&core_singular,sizeof(int),1,fp);
  fwrite(&ignore_large_rcore,sizeof(int),1,fp);
  #endif
}

void PairLJSoftcore2::
read_restart_settings(FILE *fp)
{
  #ifdef ENABLE_SOFTCORE2
  int me = comm->me;
  if (me == 0) {
    fread(&cut_core_sigma,sizeof(double),1,fp);
    fread(&core_singular,sizeof(int),1,fp);
    fread(&ignore_large_rcore,sizeof(int),1,fp);
  }
  MPI_Bcast(&cut_core_sigma,1,MPI_INT,0,world);
  MPI_Bcast(&core_singular,1,MPI_INT,0,world);
  MPI_Bcast(&ignore_large_rcore,1,MPI_INT,0,world);
  #endif
}



void PairLJSoftcore2::
write_restart_pair(FILE *fp,
                   int i, int j,
                   double **aa_cut_core_sqd,
                   double **aa_core_coeff,
                   double **aa_core_D_coeff,
                   double **aa_core_offset)
{
  #ifdef ENABLE_SOFTCORE2
  fwrite(&aa_cut_core_sqd[i][j], sizeof(double),1,fp);
  fwrite(&aa_core_coeff[i][j], sizeof(double),1,fp);
  fwrite(&aa_core_D_coeff[i][j], sizeof(double),1,fp);
  fwrite(&aa_core_offset[i][j], sizeof(double),1,fp);
  #endif
}


void PairLJSoftcore2::
read_restart_pair(FILE *fp,
                  int i, int j,
                  double **aa_cut_core_sqd,
                  double **aa_core_coeff,
                  double **aa_core_D_coeff,
                  double **aa_core_offset)
{
  #ifdef ENABLE_SOFTCORE2
  int me = comm->me;
  if (me == 0) {
    fread(&aa_cut_core_sqd[i][j],sizeof(double),1,fp);
    fread(&aa_core_coeff[i][j],sizeof(double),1,fp);
    fread(&aa_core_D_coeff[i][j],sizeof(double),1,fp);
    fread(&aa_core_offset[i][j],sizeof(double),1,fp);
  } //if (me == 0)

  MPI_Bcast(&aa_cut_core_sqd[i][j],1,MPI_DOUBLE,0,world);
  MPI_Bcast(&aa_core_coeff[i][j],1,MPI_DOUBLE,0,world);
  MPI_Bcast(&aa_core_D_coeff[i][j],1,MPI_DOUBLE,0,world);
  MPI_Bcast(&aa_core_offset[i][j],1,MPI_DOUBLE,0,world);
  #endif //#ifdef ENABLE_SOFTCORE2
}
