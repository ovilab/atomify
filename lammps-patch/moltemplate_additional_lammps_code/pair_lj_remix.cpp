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
#include "pair_lj_remix.h"
#define NDEBUG //(disable assert())
using namespace LAMMPS_NS;

PairLJRemix::
PairLJRemix(LAMMPS *lmp) : Pair(lmp)
{
                                   // The a and b exponents depend on
                                   // the pair style derived from this class
  exponent_a = 12.0;               // By default U(r) = is the sum of two 
  exponent_b = 6.0;                // terms with 1/r^12 & 1/r^6  r dependence.
  coeff_style = COEFF_EPS_SIG_4n4; // By default U(r) = 4*e*((s/r)^a-(s/r)^b)
                                   // (unless the child pair_style overrides 
                                   // choice) and K and L are not specified
                                   // explicitly by the user.
  sign_mix_style = MIX_MAX_MAX;    // By default use "repulsion wins" 
                                   // mixing rules for K and L.
}


PairLJRemix::
~PairLJRemix()
{
  if (allocated) {
    memory->destroy(epsilon);
    memory->destroy(sigma);
    memory->destroy(kappa);
    memory->destroy(lambda);
    memory->destroy(lj1);
    memory->destroy(lj2);
    memory->destroy(lj3);
    memory->destroy(lj4);
    //fprintf(stderr, "~~~~ invoking ~PairLJRemix check successful\n");
  }
}

void 
PairLJRemix::
allocate() {
  int n = atom->ntypes;
  memory->create(epsilon,n+1,n+1,"pair:epsilon");
  memory->create(sigma,n+1,n+1,"pair:sigma");
  memory->create(kappa,n+1,n+1,"pair:kappa");
  memory->create(lambda,n+1,n+1,"pair:lambda");
  memory->create(lj1,n+1,n+1,"pair:lj1");
  memory->create(lj2,n+1,n+1,"pair:lj2");
  memory->create(lj3,n+1,n+1,"pair:lj3");
  memory->create(lj4,n+1,n+1,"pair:lj4");
  #ifdef DBG_LJ_REMIX_MEM
  for (int itype=0; itype<=n; itype++) {
    for (int jtype=0; jtype<=n; jtype++) {
      epsilon[itype][jtype] = 0.0;
      sigma[itype][jtype] = 0.0;
      kappa[itype][jtype] = 0.0;
      lambda[itype][jtype] = 0.0;
      lj1[itype][jtype] = 0.0;
      lj2[itype][jtype] = 0.0;
      lj3[itype][jtype] = 0.0;
      lj4[itype][jtype] = 0.0;
    }
  }
  #endif //#ifdef DBG_LJ_REMIX_MEM
}


void PairLJRemix::
MixKL(double Ki, double Kj, 
      double Li, double Lj, 
      double &Kij, double &Lij) const
{
  switch (sign_mix_style) {
  case MIX_MAX_MAX:
    Kij = MAX(Ki, Kj);  Lij = MAX(Li, Lj);
    break;
  case MIX_MAX_MIN:
    Kij = MAX(Ki, Kj);  Lij = MIN(Li, Lj);
    break;
  case MIX_MIN_MAX:
    Kij = MIN(Ki, Kj);  Lij = MAX(Li, Lj);
    break;
  case MIX_MIN_MIN:
    Kij = MIN(Ki, Kj);  Lij = MIN(Li, Lj);
    break;
  case MIX_ARITH:
    Kij = 0.5 * (Ki + Kj);
    Lij = 0.5 * (Li + Lj);
    break;
  case MIX_GEOM:
    // To use this option, the Ki, Kj, Li, and Lj arguments must be positive.
    Kij = sqrt(chksgn(Ki) * chksgn(Kj));
    Lij = sqrt(chksgn(Li) * chksgn(Lj));
    break;
  case MIX_ES_FROM_AB:
    // The MIX_ES_FROM_AB bypasses the need for Kij and Lij parameters.
    // It is handled elsewhere.
    assert(false);
    break;
  default:
    assert(false);
    break;
  } //switch (sign_mix_style)
} //MixKL()


void PairLJRemix::
mix_ES_infer_KL(double ei, double ej, 
                double si, double sj,
                double &eij, double &sij)
{
  assert((coeff_style==COEFF_EPS_SIG_4n4) || 
         (coeff_style==COEFF_EPS_SIG_1n2) ||
         (coeff_style==COEFF_EPS_SIG_X_4n4) ||
         (coeff_style==COEFF_EPS_SIG_X_1n2));

  // First figure out the sign of eij and sij and infer KL
  int sign_ei = copysign(1.0, ei);       // = ei / |ei| (see math.h)
  int sign_si = copysign(1.0, si);       // = si / |si| (see math.h)
  int sign_Ki, sign_Li;
  signES_to_signKL(sign_ei, sign_si, sign_Ki, sign_Li);
  int sign_ej = copysign(1.0, ej);       // = ej / |ej|
  int sign_sj = copysign(1.0, sj);       // = sj / |sj|
  int sign_Kj, sign_Lj;
  signES_to_signKL(sign_ej, sign_sj, sign_Kj, sign_Lj);
  // Calculate Kij, Lij
  double Kij, Lij;
  // Then figure out Kij and Lij.
  double Ki = sign_Ki;
  double Kj = sign_Kj;
  double Li = sign_Li;
  double Lj = sign_Lj;
  MixKL(Ki, Kj, Li, Lj, Kij, Lij);
  // Make sure their integers. 
  // (Sometimes you get fractions like 0.5 when "arithmetic" sign-mixing is used.)
  if ((Kij != floor(Kij)) or (Lij != floor(Lij)))
    error->all(FLERR,
               "Illegal coeff-style and sign-mix-style combination for pair_style. The style\n"
               "you have selected requires the ability to represent fractional K and L\n"
               "coefficients, which can't be done using negative epsilon or sigma.\n"
               "You must explicitly set K, L for each atom (using \"eskl\" or \"eskl2\").\n");
  // Make sure the result can be encoded by one of the 4 choices in table above
  // For example, there is no way to represent Lennard-Jones forces with 
  // attractive singularities using epsilon and sigma alone.  If the resulting
  // Kij=0 and Lij=-1 or 0, then print an error message.
  if ((Kij <= 0) && (Lij <=0))
    error->all(FLERR,
               "Illegal coeff-style & sign-mix-style combination for pair_style. The style\n"
               "you have selected resulted in K<=0 and L<=0 (attractive singularity) for some\n"
               "atom pairs. You are safer using the maxmax, maxmin or minmax sign-mix-style.\n");
  int sign_eij, sign_sij;
  // Translate that back into epsilon and sigma
  signKL_to_signES(Kij, Lij, sign_eij, sign_sij);

  si = fabs(si);
  sj = fabs(sj);
  ei = fabs(ei);
  ej = fabs(ej);

  // Mixing rules for the magnitudes of eij and sij are the same as before
  eij = sign_eij * mix_energy(ei, ej, si, sj);
  sij = sign_sij * mix_distance(si, sj);
} //mix_ES_infer_KL()


void PairLJRemix::
signES_to_signKL(double sign_epsilon, double sign_sigma, 
                 int &K, int &L)
{
  if (sign_epsilon >= 0) {
    if (sign_sigma >= 0) {
      K = 1; L = -1;
    }
    else {
      K = 1; L = 0;
    }
  }
  else {
    if (sign_sigma >= 0) {
      K = 0; L = 1;
    }
    else {
      K = 1; L = 1;
    }
  }
} //signES_to_signKL()


void PairLJRemix::
signKL_to_signES(double K, double L, 
                 int &sign_e, int &sign_s)
{
  // sign of e,s|  K and L    (same table as above)
  // -----------|------------
  //  e>0, s>0  |  K=1, L=-1
  //  e>0, s<0  |  K=1, L=0
  //  e<0, s<0  |  K=1, L=1
  //  e<0, s>0  |  K=0, L=1
  if (K > 0) {
    if (L < 0) {
      sign_e = 1; sign_s = 1;
    }
    else if (L == 0) {
      sign_e = 1; sign_s = -1;
    }
    else {
      sign_e = -1; sign_s = -1;
    }
  }
  else {
    sign_e = -1; sign_s = 1;
  }
} //signKL_to_signES()


double PairLJRemix::
chksgn(double x, int expected_sign) const
{
  char err_msg[2048];
  if ((expected_sign > 0) && (x < 0)) {
    sprintf(err_msg, "Negative pair coeff (%g) forbidden under these conditions.", x);
    error->all(FLERR, err_msg);
  }
  else if ((expected_sign < 0) && (x > 0)) {
    sprintf(err_msg, "Positive pair coeff (%g) forbidden under these conditions.", x);
    error->all(FLERR, err_msg);
  }
  return x;
}




void PairLJRemix::
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
      // First process the argument
      if ((vArgs[i] == "AB") ||
          (vArgs[i] == "ab")) {
        coeff_style = COEFF_AB;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "eskl") {
        coeff_style = COEFF_EPS_SIG_KL;
        nargs_delete = 1;
      }
      else if ((vArgs[i] == "es") ||
               (vArgs[i] == "es4-4")) {
        coeff_style = COEFF_EPS_SIG_4n4;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "es1-2") {
        coeff_style = COEFF_EPS_SIG_1n2;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "es4k4l") {
        coeff_style = COEFF_EPS_SIG_4K4L;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "es1k2l") {
        coeff_style = COEFF_EPS_SIG_1K2L;
        nargs_delete = 1;
      }
      else if ((vArgs[i] == "esx") ||
               (vArgs[i] == "esx4-4")) {
        coeff_style = COEFF_EPS_SIG_X_4n4;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "esx1-2") {
        coeff_style = COEFF_EPS_SIG_X_1n2;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "esx11l") {
        coeff_style = COEFF_EPS_SIG_X_11L;
        nargs_delete = 1;
      }
      else if ((vArgs[i] == "esxl") ||
               (vArgs[i] == "esx44l")) {
        coeff_style = COEFF_EPS_SIG_X_44L;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "esx12l") {
        coeff_style = COEFF_EPS_SIG_X_12L;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "arith") {
        sign_mix_style = MIX_ARITH;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "maxmax") {
        sign_mix_style = MIX_MAX_MAX;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "maxmin") {
        sign_mix_style = MIX_MAX_MIN;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "minmax") {
        sign_mix_style = MIX_MIN_MAX;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "minmin") {
        sign_mix_style = MIX_MIN_MIN;
        nargs_delete = 1;
      }
      else if (vArgs[i] == "mix_es_from_ab") {
        sign_mix_style = MIX_ES_FROM_AB;
        nargs_delete = 1;
      }
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

} //void PairLJCharmmCoulCharmmInter::settings()




/* ----------------------------------------------------------------------
   init for one type pair i,j and corresponding j,i
------------------------------------------------------------------------- */

void PairLJRemix::
init_one(int i, int j,
         double **aa_eps, double **aa_sigma,
         double **aa_kappa, double **aa_lambda,
         double **aa_lj1, double **aa_lj2, double **aa_lj3, double **aa_lj4)
{
  // Did the user explicitly specify epsilon and sigma?
  bool ES_specified = true;
  // Did the user explicitly specify the "K" and "L" coefficients?
  bool KL_specified = true;
  if (coeff_style == COEFF_AB) { // In this case the user specified the A and B
    ES_specified=false; // coefficients directly, bypassing the need to 
    KL_specified=false; // specify epsilon, sigma, K or L
  }

  else if ((coeff_style == COEFF_EPS_SIG_4n4) ||
           (coeff_style == COEFF_EPS_SIG_1n2))

           //(coeff_style == COEFF_EPS_SIG_X_4n4) || <-- Confusing. These coeff_styles DO
           //(coeff_style == COEFF_EPS_SIG_X_1n2))   <-- specify kappa ("X"), but not lambda
           //                                         ...although kappa means something else,
           //                                            I still want kappa mixing in this case.

    KL_specified = false; // <-- This means the "kappa" and "lambda" arrays
                          //     should not be "mixed"


  if (setflag[i][j] == 0) {
    if (ES_specified) {
      if (KL_specified) {
        aa_eps[i][j]=mix_energy(chksgn(aa_eps[i][i]), chksgn(aa_eps[j][j]),
                                chksgn(aa_sigma[i][i]), chksgn(aa_sigma[j][j]));
        aa_sigma[i][j] = mix_distance(chksgn(aa_sigma[i][i]), 
                                      chksgn(aa_sigma[j][j]));
        MixKL(aa_kappa[i][i], aa_kappa[j][j], aa_lambda[i][i], aa_lambda[j][j],
              aa_kappa[i][j], aa_lambda[i][j]);
      } //if (KL_specified)
      else {
        // Infer the values K and L from the signs of E and S
        //  (This assumes the K and L take on integer values -1,0,1.)
        mix_ES_infer_KL(aa_eps[i][i], aa_eps[j][j],
                        aa_sigma[i][i], aa_sigma[j][j],
                        aa_eps[i][j], aa_sigma[i][j]);
      } //else clause for if (KL_specified)
    } //if (ES_specified)
    else {
      if (sign_mix_style == MIX_ARITH) {
        // A = 0.5 * (Ai + Aj)
        // B = 0.5 * (Bi + Bj)
        // where A parameters correspond to lj3[]
        //   and B parameters correspond to lj4[]
        aa_lj3[i][j]  =  0.5 * (aa_lj3[i][i] + aa_lj3[j][j]);
        aa_lj4[i][j]  =  0.5 * (aa_lj4[i][i] + aa_lj4[j][j]);
      }
      else if (sign_mix_style == MIX_ES_FROM_AB) {
        ES_specified = true;
        ABtoEpsSig4n4(chksgn(aa_lj3[i][i]), chksgn(aa_lj4[i][i]),
                      aa_eps[i][i], aa_sigma[i][i]);
        ABtoEpsSig4n4(chksgn(aa_lj3[j][j]), chksgn(aa_lj4[j][j]), 
                      aa_eps[j][j], aa_sigma[j][j]);

        // Now compute the mixed epsilon and sigma
        aa_eps[i][j] = mix_energy(aa_eps[i][i], aa_eps[j][j],
                                  aa_sigma[i][i], aa_sigma[j][j]);
        aa_sigma[i][j] = mix_distance(aa_sigma[i][i], aa_sigma[j][j]);

      } //if (sign_mix_style == MIX_ES_FROM_AB}
      else {
        error->all(FLERR,
                   "A pair_style with \"ab\" coeffs requires either \"arith\" or \"mix_es_from_ab\"");
      }
    } //else clause for if (ES_specified)
  } //if (setflag[i][j] == 0)


  if (ES_specified) {
    switch (coeff_style) {
    case COEFF_AB:
      assert(sign_mix_style == MIX_ES_FROM_AB);
      EpsSig4n4toAB(aa_eps[i][j], aa_sigma[i][j], aa_lj3[i][j], aa_lj4[i][j]);
      break;

    case COEFF_EPS_SIG_4n4:
      EpsSig4n4toAB(aa_eps[i][j], aa_sigma[i][j], aa_lj3[i][j], aa_lj4[i][j]);
      break;

    case COEFF_EPS_SIG_1n2:
      EpsSig1n2toAB(aa_eps[i][j], aa_sigma[i][j], aa_lj3[i][j], aa_lj4[i][j]);
      break;

    case COEFF_EPS_SIG_KL:
      EpsSigKLtoAB(aa_eps[i][j], aa_sigma[i][j], 
                   aa_kappa[i][j], aa_lambda[i][j], 
                   aa_lj3[i][j], aa_lj4[i][j]);
      break;

    case COEFF_EPS_SIG_4K4L:
      EpsSig4K4LtoAB(aa_eps[i][j], aa_sigma[i][j], 
                     aa_kappa[i][j], aa_lambda[i][j], 
                     aa_lj3[i][j], aa_lj4[i][j]);
      break;
    case COEFF_EPS_SIG_1K2L:
      EpsSig1K2LtoAB(aa_eps[i][j], aa_sigma[i][j], 
                     aa_kappa[i][j], aa_lambda[i][j], 
                     aa_lj3[i][j], aa_lj4[i][j]);
      break;
    case COEFF_EPS_SIG_X_4n4:
      EpsSig4n4toAB(aa_eps[i][j], aa_sigma[i][j], aa_lj3[i][j], aa_lj4[i][j]);
      break;
    case COEFF_EPS_SIG_X_1n2:
      EpsSig1n2toAB(aa_eps[i][j], aa_sigma[i][j], aa_lj3[i][j], aa_lj4[i][j]);
      break;
    case COEFF_EPS_SIG_X_11L:
      // In that case, aa_kappa[i][j] used to store something else...
      // (Let the child class decide what to do with it.)
      EpsSigKLtoAB(aa_eps[i][j], aa_sigma[i][j], 
                   //aa_kappa[i][j], aa_lambda[i][j], 
                   1.0, aa_lambda[i][j], 
                   aa_lj3[i][j], aa_lj4[i][j]);
      break;
    case COEFF_EPS_SIG_X_44L:
      // In that case, aa_kappa[i][j] used to store something else...
      // (Let the child class decide what to do with it.)
      EpsSig4K4LtoAB(aa_eps[i][j], aa_sigma[i][j], 
                     //aa_kappa[i][j], aa_lambda[i][j], 
                     1.0, aa_lambda[i][j],
                     aa_lj3[i][j], aa_lj4[i][j]);
      break;
    case COEFF_EPS_SIG_X_12L:
      // In that case, aa_kappa[i][j] used to store something else...
      // (Let the child class decide what to do with it.)
      EpsSig1K2LtoAB(aa_eps[i][j], aa_sigma[i][j], 
                     //aa_kappa[i][j], aa_lambda[i][j], 
                     1.0, aa_lambda[i][j], 
                     aa_lj3[i][j], aa_lj4[i][j]);
      break;
    default:
      assert(false);
    } // switch (coeff_style)
  } //if (! ignore_eps_sig)


  // Coefficients used to calculate the force
  aa_lj1[i][j]     = exponent_a * aa_lj3[i][j];
  aa_lj2[i][j]     = exponent_b * aa_lj4[i][j];

  // i <--> j
  aa_lj1[j][i]     = aa_lj1[i][j];
  aa_lj2[j][i]     = aa_lj2[i][j];
  aa_lj3[j][i]     = aa_lj3[i][j];
  aa_lj4[j][i]     = aa_lj4[i][j];
  // For consistency we also swap i<-->j for eps, sigma, kappa and lambda
  aa_eps[j][i]     = aa_eps[i][j];
  aa_sigma[j][i]   = aa_sigma[i][j];
  aa_kappa[j][i]   = aa_kappa[i][j];
  aa_lambda[j][i]  = aa_lambda[i][j];

} //PairLJRemix::init_one(int, int)




void PairLJRemix::
write_restart_pair(FILE *fp,
                   int i, int j, 
                   double **aa_eps, double **aa_sigma, 
                   double **aa_kappa, double **aa_lambda,
                   double **aa_lj3, double **aa_lj4)
{
  if (coeff_style == COEFF_AB) {
    fwrite(&aa_lj3[i][j],sizeof(double),1,fp);
    fwrite(&aa_lj4[i][j],sizeof(double),1,fp);
  }
  else {
    fwrite(&aa_eps[i][j],sizeof(double),1,fp);
    fwrite(&aa_sigma[i][j],sizeof(double),1,fp);
    fwrite(&aa_kappa[i][j],sizeof(double),1,fp);
    fwrite(&aa_lambda[i][j],sizeof(double),1,fp);
  }
}





void PairLJRemix::
read_restart_pair(FILE *fp,
                  int i, int j,
                  double **aa_eps, double **aa_sigma, 
                  double **aa_kappa, double **aa_lambda,
                  double **aa_lj3, double **aa_lj4)
{
  int me = comm->me;
  if (me == 0) {
    if (coeff_style == COEFF_AB) {
      // lj3 and lj4 store the "A" and "B" coefficients
      fread(&aa_lj3[i][j],sizeof(double),1,fp);
      fread(&aa_lj4[i][j],sizeof(double),1,fp);
    }
    else {
      fread(&aa_eps[i][j],sizeof(double),1,fp);
      fread(&aa_sigma[i][j],sizeof(double),1,fp);
      fread(&aa_kappa[i][j],sizeof(double),1,fp);
      fread(&aa_lambda[i][j],sizeof(double),1,fp);
    } //else clause for "if (coeff_style == COEFF_AB)"
  } //if (me == 0)

  if (coeff_style == COEFF_AB) {
    MPI_Bcast(&aa_lj3[i][j],1,MPI_DOUBLE,0,world);
    MPI_Bcast(&aa_lj4[i][j],1,MPI_DOUBLE,0,world);
  }
  else {
    MPI_Bcast(&aa_eps[i][j],1,MPI_DOUBLE,0,world);
    MPI_Bcast(&aa_sigma[i][j],1,MPI_DOUBLE,0,world);
    MPI_Bcast(&aa_kappa[i][j],1,MPI_DOUBLE,0,world);
    MPI_Bcast(&aa_lambda[i][j],1,MPI_DOUBLE,0,world);
  } //else clause for "if (coeff_style == COEFF_AB)"

} //PairLJRemix::read_restart_pair()



void PairLJRemix::
write_restart_settings(FILE *fp)
{
  fwrite(&coeff_style,sizeof(int),1,fp);
  fwrite(&sign_mix_style,sizeof(int),1,fp);
}

void PairLJRemix::
read_restart_settings(FILE *fp)
{
  int me = comm->me; 
  if (me == 0) {
    fread(&coeff_style,sizeof(int),1,fp);
    fread(&sign_mix_style,sizeof(int),1,fp);
  }
  MPI_Bcast(&coeff_style,1,MPI_INT,0,world);
  MPI_Bcast(&sign_mix_style,1,MPI_INT,0,world);
}


// It's much more elegant to use C++ vector<string> to do argument-list
// parsing (compared to int, char**), especially for inherited classes.
// Alas, force->bounds() does not work with with strings or const char *
// arrays.  This prevents me from using force->bounds(s.c_str(), ...).
// So I define a new version of force->bounds() which works with C++ strings
// This just copies "s" to a non-const char array and invokes force->bounds().

void PairLJRemix::
bounds_cppstr(const char *file, int line, string s, int nmax, int &nlo, int &nhi, int nmin) const {
  char *str = new char [s.size()+1];
  strncpy(str, s.c_str(), s.size()+1);
  force->bounds(file, line, str, nmax, nlo, nhi, nmin);
  delete [] str;
}

void PairLJRemix::
boundsbig_cppstr(const char *file, int line, string s, bigint nmax, bigint &nlo, bigint &nhi, bigint nmin) const {
  char *str = new char [s.size()+1];
  strncpy(str, s.c_str(), s.size()+1);
  force->boundsbig(file, line, str, nmax, nlo, nhi, nmin);
  delete [] str;
}

double PairLJRemix::
numeric_cppstr(const char *file, int line, string s) const {
  char *str = new char [s.size()+1];
  strncpy(str, s.c_str(), s.size()+1);
  double x = force->numeric(file, line, str);
  delete [] str;
  return x;
}

int PairLJRemix::
inumeric_cppstr(const char *file, int line, string s) const {
  char *str = new char [s.size()+1];
  strncpy(str, s.c_str(), s.size()+1);
  int i = force->inumeric(file, line, str);
  delete [] str;
  return i;
}

bigint PairLJRemix::
bnumeric_cppstr(const char *file, int line, string s) const {
  char *str = new char [s.size()+1];
  strncpy(str, s.c_str(), s.size()+1);
  bigint i = force->bnumeric(file, line, str);
  delete [] str;
  return i;
}

tagint PairLJRemix::
tnumeric_cppstr(const char *file, int line, string s) const {
  char *str = new char [s.size()+1];
  strncpy(str, s.c_str(), s.size()+1);
  tagint i = force->tnumeric(file, line, str);
  delete [] str;
  return i;
}
