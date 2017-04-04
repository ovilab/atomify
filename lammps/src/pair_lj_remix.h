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

#ifndef LMP_PAIR_LJ_REMIX_H
#define LMP_PAIR_LJ_REMIX_H

#include <cmath>
#include <string>
#include <vector>
using namespace std;
#include "error.h"
#include "pair.h"


// Comment out the next line when no longer debugging
#define DBG_LJ_REMIX_MEM


namespace LAMMPS_NS {

class PairLJRemix : public Pair {
  protected:

  PairLJRemix(class LAMMPS *lmp);

  virtual ~PairLJRemix();

  double exponent_a, exponent_b;

  double **lj1, **lj2, **lj3, **lj4;
  double **epsilon, **sigma;
  double **kappa,   **lambda;

  // Various conventions for coefficients in Lennard Jones potentials:
  // (Note: "enum" types may be unsafe with old MPI routines.)
  enum CoeffStyle {COEFF_AB,              // U(r) =     A / r^a   -   B / r^6
                   COEFF_EPS_SIG_KL,      // U(r) =  e*(K*(s/r)^a +   L*(s/r)^b)
                   COEFF_EPS_SIG_4n4,     // U(r) =4*e*( (s/r)^a  -     (s/r)^b)
                   COEFF_EPS_SIG_1n2,     // U(r) =  e*( (s/r)^a  - 2 * (s/r)^b)
                   COEFF_EPS_SIG_4K4L,    // U(r) =4*e*(K*(s/r)^a +   L*(s/r)^b)
                   COEFF_EPS_SIG_1K2L,    // U(r) =  e*(K*(s/r)^a + 2*L*(s/r)^b)

                   COEFF_EPS_SIG_X_4n4,   // U(r) =4*e*( (s/r)^a  -     (s/r)^b)
                   COEFF_EPS_SIG_X_1n2,   // U(r) =  e*( (s/r)^a  -   2*(s/r)^b)
                   COEFF_EPS_SIG_X_11L,   // U(r) =  e*( (s/r)^a  +   L*(s/r)^b)
                   COEFF_EPS_SIG_X_44L,   // U(r) =4*e*( (s/r)^a  +   L*(s/r)^b)
                   COEFF_EPS_SIG_X_12L};  // U(r) =  e*( (s/r)^a  + 2*L*(s/r)^b)
  CoeffStyle coeff_style;

  inline bool UsesABcoeffs() const {
    return (coeff_style==COEFF_AB);
  }

  inline bool UsesEScoeffs() const {
    return ((coeff_style==COEFF_EPS_SIG_4n4) || 
            (coeff_style==COEFF_EPS_SIG_1n2));
  }

  inline bool UsesESKcoeffs() const {
    return ((coeff_style==COEFF_EPS_SIG_X_4n4) ||
            (coeff_style==COEFF_EPS_SIG_X_1n2));
  }

  inline bool UsesESKLcoeffs() const {
    return ((coeff_style == COEFF_EPS_SIG_KL) ||
	    (coeff_style == COEFF_EPS_SIG_4K4L) ||
            (coeff_style == COEFF_EPS_SIG_1K2L) ||
	    (coeff_style == COEFF_EPS_SIG_X_11L) ||
	    (coeff_style == COEFF_EPS_SIG_X_44L) ||
	    (coeff_style == COEFF_EPS_SIG_X_12L));
  }

  /* ----------------------------------------------------------------------
   mixing of pair potential distances (sigma, cutoff)
  ------------------------------------------------------------------------- */

  // Rules for mixing the K and L (dimensionless) coefficients
  // which appear in the forumulas above for U(r).

  enum SignMixStyle {MIX_MAX_MAX, // Kij = MAX(Ki,Kj),  Lij = MAX(Li,Lj)
                     MIX_MAX_MIN, // Kij = MAX(Ki,Kj),  Lij = MIN(Li,Lj)
                     MIX_MIN_MAX, // Kij = MIN(Ki,Kj),  Lij = MAX(Li,Lj)
                     MIX_MIN_MIN, // Kij = MIN(Ki,Kj),  Lij = MIN(Li,Lj)
                     MIX_ARITH,   // Kij = 0.5(Ki+Kj),  Lij = 0.5(Li+Lj)
                     MIX_GEOM,    // Kij = sqrt(Ki*Kj), Lij = sqrt(Li*Lj)
                     MIX_ES_FROM_AB};//<- infer eps and sig from AB params
                                     //(only use MIX_ES_FROM_AB with COEFF_AB)
  SignMixStyle sign_mix_style;



  // Mixing eij and sij, Kij, Lij when coefficients K and L are specified:
  void 
  MixKL(double Ki, double Kj, 
        double Li, double Lj, 
        double &Kij, double &Lij) const;



  // --------------------------------------------------
  // -----  Mixing eij and sij when the K and L   -----
  // -----   coefficients are NOT specified:      -----
  // --------------------------------------------------
  //
  // If the K and L coefficients are ommitted, they are assumed to be 1,0,-1
  // and 
  //   Uij(r) = |eij| * ( Ca*Kij*(|sij|/r)^a + Cb*Lij*(|sij|/r)^b ) 
  //   (Ca and Cb depend on coeff_style as explained below)
  //
  // mix_ES_infer_KL() is a variant of Pair::mix_energy() and 
  // Pair::mix_distance().  These functions mix the epsilon and sigma 
  // parameters, storing the result in eij, and sij.  But this function also
  // handles cases where epsilon and sigma are negative, interpreting the 
  // sign of these parameters as modifiers in the sign of the 1/r^p and 1/r^q
  // terms.
  // sign of e,s            Uii(r) formula                          Ki and Li
  //  ------------------------------------------------------------------------
  //  ei>0, si>0 | Uii(r) = |ei|*(Ca*(|si|/r)^p - Cb*(|si|/r)^q) | Ki=1, Li=-1
  //  ei>0, si<0 | Uii(r) = |ei| *Ca*(|si|/r)^p                  | Ki=1, Li=0
  //  ei<0, si<0 | Uii(r) = |ei|*(Ca*(|si|/r)^p + Cb*(|si|/r)^q) | Ki=1, Li=1
  //  ei<0, si>0 | Uii(r) =                  |ei|*Cb*(|si|/r)^q  | Ki=0, Li=1
  //
  //  (The "i" subscript denotes atom type i. "Ca" and "Cb" are explained below)
  //
  // Ki and Li, Kj and Lj are chosen according to the table above.
  // Kij and Lij parameters are chosen according to Ki, Kj, Li, Lj according
  // to the "mixKLstyle".  After Kij and Lij have been determined, then 
  // appropriate signs for eij and sij are chosen accordingly.  Whether positive
  // or negative, the magnitude of eij is calculated using Pair:mix_energy()
  //          and the magnitude of sij is calculated using Pair:mix_distance().
  //
  // Once the dimensionless K and L coefficients are determined, the 
  // interaction is calculated in a way which depends on the coeff_style:
  //        If coeff_style==COEFF_EPS_SIG_4n4, then Ca=4, and Cb=4, and
  //   U(r) = 4*|eij|*(Kij*(|sij|/r)^a +   Lij*(|sij|/r)^b) 
  //        
  //        If coeff_style==COEFF_EPS_SIG_1n2, then Ca=1, and Cb=2, and
  //   U(r) =   |eij|*(Kij*(|sij|/r)^a + 2*Lij*(|sij|/r)^b)
  //
  // These tables and their inverses are returned by these two functions:


  void mix_ES_infer_KL(double ei, double ej, 
                       double si, double sj,
                       double &eij, double &sij); //<--should be "const"
  //The mixing functions should all end in "const" because they do not
  //modify the state of the class when called, however I can't do that until
  //Steve "const"s the Pair::mix_energy() Pair::mix_distance() functions.


  //   Functions which translate between different 
  //   conventions for Lennard-Jones coefficients:
  inline void 
  EpsSigKLtoAB(double epsilon, 
               double sigma, 
               double kappa, 
               double lambda, 
               double &A, double &B) const
  {
    //         A       B
    // U(r) = ---  -  --- =  e * (K*(s/r)^a + L*(s/r)^b)
    //        r^a     r^b 
    A =   kappa  * epsilon * pow(sigma, exponent_a);
    B =  -lambda * epsilon * pow(sigma, exponent_b);
  }

  inline void 
  EpsSig4K4LtoAB(double epsilon, 
                 double sigma, 
                 double kappa, 
                 double lambda, 
                 double &A, double &B) const
  {
    EpsSigKLtoAB(epsilon, sigma, 4.0*kappa, 4.0*lambda, A, B);
  }

  inline void 
  EpsSig1K2LtoAB(double epsilon, 
                 double sigma, 
                 double kappa, 
                 double lambda, 
                 double &A, double &B) const
  {
    EpsSigKLtoAB(epsilon, sigma, kappa, 2.0*lambda, A, B);
  }


  inline void 
  EpsSig4n4toAB(double epsilon, double sigma, double &A, double &B) const
  {
    EpsSigKLtoAB(epsilon, sigma, 4.0, -4.0, A, B);
  }

  inline void 
  EpsSig1n2toAB(double epsilon, double sigma, double &A, double &B) const
  {
    EpsSigKLtoAB(epsilon, sigma, 1.0, -2.0, A, B);
  }

  inline void 
  ABKLtoEpsSig(double A, double B, 
               double kappa, double lambda,
               double &epsilon, double &sigma) const
  {
    //         A       B
    // U(r) = ---  -  --- =  e * (K*(s/r)^a + L*(s/r)^b)
    //        r^a     r^b 
    //  ---->
    //    A =  K*e*s^a
    //    B = -L*e*s^b
    //  ---->
    //    s = ((-L*A)/(K*B))^(1/(a-b))
    //    e = -B / (L*s^b) 
    //      = (-B/L) * ((-L*A)/(K*B))^(-b/(a-b))
    double nlA_over_kB = (-lambda*A) / (kappa*B);
    chksgn(nlA_over_kB);
    sigma   = 
      pow(nlA_over_kB, 1.0/(exponent_a-exponent_b));
    epsilon = (-B/lambda) *
      pow(nlA_over_kB, (-exponent_b/(exponent_a-exponent_b)));
  }

  inline void 
  ABtoEpsSig4n4(double A, double B, 
                double &epsilon, double &sigma) const
  {
    ABKLtoEpsSig(A, B, 4.0, -4.0, epsilon, sigma);
  }

  inline void 
  ABtoEpsSig1n2(double A, double B, 
                double &epsilon, double &sigma) const
  {
    ABKLtoEpsSig(A, B, 1.0, -2.0, epsilon, sigma);
  }

  static void signES_to_signKL(double sign_epsilon, double sign_sigma, 
                               int &K, int &L);


  static void signKL_to_signES(double K, double L, 
                               int &sign_e, int &sign_s);

  // Make sure the numeric argument is positive (or negative).
  // If so, return the original number.  Otherwise, print an error.
  double chksgn(double x, int expected_sign=1) const;



  // General functions:


  virtual void allocate();

  //virtual void settings(int &narg, char **arg);
  virtual void settings(vector<string>&);

  virtual void init_one(int i, int j,
                        double **aa_eps, double **aa_sigma,
                        double **aa_kappa, double **aa_lambda,
                        double **aa_lj1, double **aa_lj2, 
                        double **aa_lj3, double **aa_lj4);

  virtual void write_restart_pair(FILE *fp,
                                  int i, int j, 
                                  double **aa_eps, double **aa_sigma, 
                                  double **aa_kappa, double **aa_lambda,
                                  double **aa_lj3, double **aa_lj4);

  virtual void read_restart_pair(FILE *fp,
                                 int i, int j,
                                 double **aa_eps, double **aa_sigma, 
                                 double **aa_kappa, double **aa_lambda,
                                 double **aa_lj3, double **aa_lj4);

  virtual void write_restart_settings(FILE *fp);
  virtual void read_restart_settings(FILE *fp);

 public:
  // It's much more elegant to use C++ vector<string> to do argument-list
  // parsing (compared to int, char**), especially for inherited classes.
  // Alas, as of 2014-6-6, none of these parsing functions work with 
  // const char * pointers:
  //   Force::bounds()
  //   Force::boundsbig()
  //   Force::numeric()
  //   Force::inumeric()
  //   Force::bnumeric()
  //   Force::tnumeric()
  // (and std::basic_string::c_str() returns const char * pointers)
  // I wish I had permission to replace "char *str" with "const char *str"
  // in force.h and force.cpp.  As an alternative it makes sense to
  // write C++ wrappers for all of these parsing functions.
  void bounds_cppstr(const char *file, int line,
                     string s, int nmax,
                     int &nlo, int &nhi, int nmin=1) const;
  void boundsbig_cppstr(const char *file, int line,
                        string s, bigint nmax,
                        bigint &nlo, bigint &nhi, bigint nmin=1) const;
  double numeric_cppstr(const char *file, int line, string s) const;
  int inumeric_cppstr(const char *file, int line, string s) const;
  bigint bnumeric_cppstr(const char *file, int line, string s) const;
  tagint tnumeric_cppstr(const char *file, int line, string s) const;

}; //class PairLJRemix


} //namespace LAMMPS_NS

#endif //#ifndef LMP_PAIR_LJ_REMIX_H
