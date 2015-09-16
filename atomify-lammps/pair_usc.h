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

#ifdef PAIR_CLASS

PairStyle(usc,PairUSC)

#else

#ifndef LMP_PAIR_USC_H
#define LMP_PAIR_USC_H

#include "pair.h"
#include "compute.h"
#include <iostream>
#include <vector>

namespace LAMMPS_NS {

class PairUSC : public Pair {
public:
    PairUSC(class LAMMPS *);
    virtual ~PairUSC();
    virtual void compute(int, int);
    void settings(int, char **);
    virtual void coeff(int, char **);
    virtual double init_one(int, int);
    virtual void init_style();

    struct Param {
        int eta;
        double H, D, W;
        double r1s, r4s;
        double Zi, Zj;
        double costheta;
        double r0, r0sq;
        double rc, rcsq;
        double ZiZj;
        double oneOverR1s, oneOverR4s;

        double ksi,B,twoTimesB;
        double HTimesEta,ZiZjOverR1s,twoTimesD,DOverR4sHalf;
        int ielement,jelement,kelement;
        void print() {
            std::cout << "H=" << H << std::endl;
            std::cout << "eta=" << eta << std::endl;
            std::cout << "D=" << D << std::endl;
            std::cout << "W=" << W << std::endl;
            std::cout << "r1s=" << r1s << std::endl;
            std::cout << "r4s=" << r4s << std::endl;
            std::cout << "Zi=" << Zi << std::endl;
            std::cout << "Zj=" << Zj << std::endl;
            std::cout << "r0=" << r0 << std::endl;
            std::cout << "rc=" << rc << std::endl;
            std::cout << "B=" << B << std::endl;
            std::cout << "costheta=" << costheta << std::endl;
            std::cout << "ksi=" << ksi << std::endl;
        }
    };

    struct PotentialTable {
        double deltaR2;
        double oneOverDeltaR2;
        int numTabulatedEntries;
        double ***force;
        double ***potential;
        double *forceOOWater;
        double *potentialOOWater;
    };

    struct BenchmarkCounts {
        unsigned int pairCountTotal;
        unsigned int pairCountComputed;
        unsigned int tripletCountTotal;
        unsigned int tripletCountComputed;
        unsigned int OOPairCountComputed;
        unsigned int OOPairCountTotal;

        unsigned int pairsComputed;
    };

protected:
    double cutmax;                // max cutoff for all elements
    int nelements;                // # of unique elements
    char *id_coord;               // id of coord/usc compute
    char **elements;              // names of unique elements
    int ***elem2param;            // mapping from element triplets to parameters
    int *map;                     // mapping from atom types to elements
    int nparams;                  // # of stored parameter sets
    int maxparam;                 // max # of parameter sets
    int oxygenType;
    int siliconType;
    int hydrogenType;

    std::vector<double> R;
    std::vector<double> D;
    std::vector<double> dg;
    std::vector<std::vector<double> > gradThetaTable;
    Param *params;                // parameter set for an I-J-K interaction
    Param paramsOOWater;        // Special case for silica+water system
    PotentialTable potentialTable;

    virtual void allocate();
    void read_file(char *);
    virtual void setup();
    void updatePotentialParameters(Param &param);
    void computeForcesAndEnergyForOO(int i, int j, int potentialTableIndex, double fractionalRest, int evflag, Compute *computeCoord);
    void createForceAndPotentialTables(Param *param, int element1, int element2);
    void twobody(Param *param, double rSquared, double &force, double &energy);
    void threebody(Param *, Param *, Param *, double, double, double *, double *,
                   double *, double *, int, double &);
    inline double withinRange(const double rSquared, const double R, const double D) { return (rSquared >= (R-D)*(R-D)) && rSquared < (R+D)*(R+D); }
};

}

#endif
#endif

/* ERROR/WARNING messages:

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

E: Incorrect args for pair coefficients

Self-explanatory.  Check the input script or data file.

E: Pair style Stillinger-Weber requires atom IDs

This is a requirement to use the SW potential.

E: Pair style Stillinger-Weber requires newton pair on

See the newton command.  This is a restriction to use the SW
potential.

E: All pair coeffs are not set

All pair coefficients must be set in the data file or by the
pair_coeff command before running a simulation.

E: Cannot open Stillinger-Weber potential file %s

The specified SW potential file cannot be opened.  Check that the path
and name are correct.

E: Incorrect format in Stillinger-Weber potential file

Incorrect number of words per line in the potential file.

E: Illegal Stillinger-Weber parameter

One or more of the coefficients defined in the potential file is
invalid.

E: Potential file has duplicate entry

The potential file has more than one entry for the same element.

E: Potential file is missing an entry

The potential file does not have a needed entry.

*/
