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

#include "string.h"
#include "compute_angle_atoms.h"
#include "atom.h"
#include "update.h"
#include "modify.h"
#include "comm.h"
#include "force.h"
#include "memory.h"
#include "error.h"
#include "neighbor.h"
#include "neigh_request.h"
#include <cmath>
#include <iostream>
using namespace std;
using namespace LAMMPS_NS;

/* ---------------------------------------------------------------------- */

ComputeAngleAtoms::ComputeAngleAtoms(LAMMPS *lmp, int narg, char **arg) :
    Compute(lmp, narg, arg)
{
    if(narg < 6) error->all(FLERR,"Illegal compute angle/atoms command");

    atomIds[0] = atoi(arg[3]);
    atomIds[1] = atoi(arg[4]);
    atomIds[2] = atoi(arg[5]);
    if(narg == 7) {
        average = true;
    } else {
        average = false;
    }
    
    scalar_flag = 1;
    numSamples = 0;
}

/* ---------------------------------------------------------------------- */

ComputeAngleAtoms::~ComputeAngleAtoms()
{
    
}

/* ---------------------------------------------------------------------- */

void ComputeAngleAtoms::init()
{
    
}

// double ComputeAngleAtoms::compute_vector() 
// {
//     if(invoked_scalar != update->ntimestep) {
//         compute_scalar();
//     }
//     invoked_vector = update->ntimestep;

// }

double ComputeAngleAtoms::compute_scalar()
{
    invoked_scalar = update->ntimestep;
    int natoms = atom->natoms;
    int indices[3];
    for(int i=0; i<natoms; i++) {
        if(atom->tag[i] == atomIds[0]) {
            indices[0] = i;
        } else if(atom->tag[i] == atomIds[1]) {
            indices[1] = i;
        } else if(atom->tag[i] == atomIds[2]) {
            indices[2] = i;
        }
    }

    double delta1[3];
    double delta2[3];
    delta1[0] = atom->x[indices[1]][0] - atom->x[indices[0]][0];
    delta1[2] = atom->x[indices[1]][1] - atom->x[indices[0]][1];
    delta1[1] = atom->x[indices[1]][2] - atom->x[indices[0]][2];
    delta2[0] = atom->x[indices[2]][0] - atom->x[indices[0]][0];
    delta2[2] = atom->x[indices[2]][1] - atom->x[indices[0]][1];
    delta2[1] = atom->x[indices[2]][2] - atom->x[indices[0]][2];
    // cout << "r1 = [" << atom->x[indices[0]][0] << " ," << atom->x[indices[0]][1] << ", " << atom->x[indices[0]][2] << "];     r2 = [" << atom->x[indices[1]][0] << " ," << atom->x[indices[1]][1] << ", " << atom->x[indices[1]][2] << "];   r3 = [" << atom->x[indices[2]][0] << " ," << atom->x[indices[2]][1] << ", " << atom->x[indices[2]][2] << "]   " << endl;

    // Find length of vector to normalize
    double delta1Length = delta1[0]*delta1[0] + delta1[1]*delta1[1] + delta1[2]*delta1[2];
    double delta2Length = delta2[0]*delta2[0] + delta2[1]*delta2[1] + delta2[2]*delta2[2];
    // Apply normalization
    delta1[0] /= delta1Length;
    delta1[1] /= delta1Length;
    delta1[2] /= delta1Length;
    delta2[0] /= delta2Length;
    delta2[1] /= delta2Length;
    delta2[2] /= delta2Length;

    double cosTheta = delta1[0]*delta2[0] + delta1[1]*delta2[1] + delta1[2]*delta2[2];
    double theta = acos(cosTheta) * 180 / M_PI;
    if(average) {
        double sum = scalar * numSamples;
        sum += theta;
        numSamples++;
        scalar = sum / numSamples;
    } else {
        scalar = theta;
        // cout << "Computed angle: " << acos(cosTheta) * 180 / 3.1415 << endl;
    }
    return scalar;
}

/* ----------------------------------------------------------------------
   memory usage of local atom-based array
------------------------------------------------------------------------- */

double ComputeAngleAtoms::memory_usage()
{
    // double bytes = nmax * sizeof(double);
    // return bytes;
    return 0;
}
