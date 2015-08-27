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
        void print();
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
    int ***moleculeMap;           // mapping from element triplets to parameters
    int *atomTypeMap;             // mapping from atom types to elements
    int nparams;                  // # of stored parameter sets
    int maxparam;                 // max # of parameter sets
    int oxygenType;
    int siliconType;
    int hydrogenType;

    std::vector<double> R;  // Values for interpolating between water OO and silica OO
    std::vector<double> D;  // Values for interpolating between water OO and silica OO
    std::vector<double> dg; // Temporary table for storing the gradient of theta for single OO pairs
    std::vector<std::vector<double> > gradThetaTable; // Table of the gradient of the theta function
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
