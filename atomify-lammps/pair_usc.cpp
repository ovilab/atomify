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
   Contributing author: Anders Hafreager (UiO)
   Based on Stillinger-Weber implementation by Aidan Thompson (SNL).
------------------------------------------------------------------------- */

#include <iostream>

#include "pair_usc.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "atom.h"
#include "neighbor.h"
#include "neigh_request.h"
#include "force.h"
#include "comm.h"
#include "memory.h"
#include "neighbor.h"
#include "neigh_list.h"
#include "memory.h"
#include "error.h"
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace LAMMPS_NS;

#define MAXLINE 1024
#define DELTA 4
// #define DEBUGUSC
/* ---------------------------------------------------------------------- */

PairUSC::PairUSC(LAMMPS *lmp) : Pair(lmp)
{
    single_enable = 0;
    restartinfo = 0;
    one_coeff = 1;
    manybody_flag = 1;

    map = NULL;
    nelements = 0;
    elements = NULL;
    nparams = maxparam = 0;
    oxygenType = -1;
    hydrogenType = -1;
    siliconType = -1;
    params = NULL;
    elem2param = NULL;
    potentialTable.force = NULL;
    potentialTable.potential = NULL;
    potentialTable.potentialOOWater = NULL;
    potentialTable.forceOOWater = NULL;
    id_coord = NULL;
}

/* ----------------------------------------------------------------------
   check if allocated, since class can be destructed when incomplete
------------------------------------------------------------------------- */

PairUSC::~PairUSC()
{
    if (copymode) return;

    if (elements)
        for (int i = 0; i < nelements; i++) delete [] elements[i];
    delete [] elements;
    memory->destroy(params);
    memory->destroy(elem2param);
    memory->destroy(potentialTable.force);
    memory->destroy(potentialTable.potential);

    if (allocated) {
        memory->destroy(setflag);
        memory->destroy(cutsq);
        delete [] map;
    }
}

/* ---------------------------------------------------------------------- */

void PairUSC::compute(int eflag, int vflag)
{
    Compute *computeCoord = NULL;
    if(id_coord != 0) {
        int icompute = modify->find_compute(id_coord);
        if (icompute < 0) {
          error->all(FLERR,"PairUSC: could not find coord/usc compute.");
        }
        computeCoord = modify->compute[icompute];
        computeCoord->compute_peratom();
    }
    int i,j,k,ii,jj,kk,inum,jnum;
    int itype,jtype,ktype,ijparam,ikparam,ijkparam;
    tagint itag,jtag;
    double xi,yi,zi,dx,dy,dz,evdwl;
    double rsq,rsq1,rsq2;
    double delr1[3],delr2[3],fj[3],fk[3];
    int *ilist,*jlist,*numneigh,**firstneigh;

    evdwl = 0.0;
    if (eflag || vflag) ev_setup(eflag,vflag);
    else evflag = vflag_fdotr = 0;

    double **x = atom->x;
    double **f = atom->f;
    tagint *tag = atom->tag;
    int *type = atom->type;
    int nlocal = atom->nlocal;
    int newton_pair = force->newton_pair;

    inum = list->inum;
    ilist = list->ilist;
    numneigh = list->numneigh;
    firstneigh = list->firstneigh;

    std::vector<std::pair<int,int> > computedForces;

    for (ii = 0; ii < inum; ii++) {
        i = ilist[ii];
        itag = tag[i];
        itype = map[type[i]];
        xi = x[i][0];
        yi = x[i][1];
        zi = x[i][2];

        // two-body interactions, skip half of them
        jlist = firstneigh[i];
        jnum = numneigh[i];
        for (jj = 0; jj < jnum; jj++) {
            j = jlist[jj];
            j &= NEIGHMASK;
            jtag = tag[j];
            jtype = map[type[j]];

            bool bothAtomsAreOxygen = (itype == oxygenType && jtype == oxygenType);
            bool computeAdvancedOxygenOxygenInteraction = (nelements > 2) && bothAtomsAreOxygen;
            bool computeBothOxygenPairs = computeAdvancedOxygenOxygenInteraction && j>=nlocal;

            // If this is an oxygen pair and we need to calculate advanced force field,
            // we have to compute forces for atom i on both processors, so don't use newton's third
            // law here.
            if (itag > jtag) {
                if ((itag+jtag) % 2 == 0 && !computeBothOxygenPairs) continue;
            } else if (itag < jtag) {
                if ((itag+jtag) % 2 == 1 && !computeBothOxygenPairs) continue;
            } else {
                if (x[j][2] < zi && !computeBothOxygenPairs) continue;
                if (x[j][2] == zi && x[j][1] < yi && !computeBothOxygenPairs) continue;
                if (x[j][2] == zi && x[j][1] == yi && x[j][0] < xi && !computeBothOxygenPairs) continue;
            }

            dx = xi - x[j][0];
            dy = yi - x[j][1];
            dz = zi - x[j][2];
            rsq = dx*dx + dy*dy + dz*dz;

            ijparam = elem2param[itype][jtype][jtype];

            if (rsq > params[ijparam].rcsq) continue;

            int potentialTableIndex = rsq*potentialTable.oneOverDeltaR2;
            double fractionalRest = rsq*potentialTable.oneOverDeltaR2 - potentialTableIndex; // double - int will only keep the 0.xxxx part

            if(potentialTableIndex < 0 || potentialTableIndex >= potentialTable.numTabulatedEntries) {
                std::cout << "Error, potential table out of bounds." << std::endl;
                std::cout << "r(i) = [" << x[i][0] << ", " << x[i][1] << ", " << x[i][2] << "]" << std::endl;
                std::cout << "r(j) = [" << x[j][0] << ", " << x[j][1] << ", " << x[j][2] << "]" << std::endl;

                exit(1);
            }

            computedForces.push_back(std::make_pair(itag, jtag));

            if(computeAdvancedOxygenOxygenInteraction) {
                computeForcesAndEnergyForOO(i, j, potentialTableIndex, fractionalRest, evflag, computeCoord);
            } else {
                double force0 = potentialTable.force[itype][jtype][potentialTableIndex];
                double force1 = potentialTable.force[itype][jtype][potentialTableIndex+1];
                double force = (1.0 - fractionalRest)*force0 + fractionalRest*force1;

                f[i][0] += dx*force;
                f[i][1] += dy*force;
                f[i][2] += dz*force;
                f[j][0] -= dx*force;
                f[j][1] -= dy*force;
                f[j][2] -= dz*force;

                if (evflag) {
                    double energy0 = potentialTable.potential[itype][jtype][potentialTableIndex];
                    double energy1 = potentialTable.potential[itype][jtype][potentialTableIndex+1];
                    double energy = (1.0 - fractionalRest)*energy0 + fractionalRest*energy1;
                    ev_tally(i,j,nlocal,newton_pair,energy,0.0,force,dx,dy,dz);
                }
            }
        }

        for (jj = 0; jj < jnum - 1; jj++) {
            j = jlist[jj];
            j &= NEIGHMASK;
            jtag = tag[j];
            jtype = map[type[j]];
            ijparam = elem2param[itype][jtype][jtype];
            delr1[0] = x[j][0] - xi;
            delr1[1] = x[j][1] - yi;
            delr1[2] = x[j][2] - zi;
            rsq1 = delr1[0]*delr1[0] + delr1[1]*delr1[1] + delr1[2]*delr1[2];
            if (rsq1 > params[ijparam].r0sq) continue;
            for (kk = jj+1; kk < jnum; kk++) {
                k = jlist[kk];
                k &= NEIGHMASK;

                ktype = map[type[k]];
                ikparam = elem2param[itype][ktype][ktype];
                ijkparam = elem2param[itype][jtype][ktype];

                delr2[0] = x[k][0] - xi;
                delr2[1] = x[k][1] - yi;
                delr2[2] = x[k][2] - zi;
                rsq2 = delr2[0]*delr2[0] + delr2[1]*delr2[1] + delr2[2]*delr2[2];
                if (rsq2 > params[ikparam].r0sq) continue;

                threebody(&params[ijparam],&params[ikparam],&params[ijkparam],
                          rsq1,rsq2,delr1,delr2,fj,fk,eflag,evdwl);
                f[i][0] -= fj[0] + fk[0];
                f[i][1] -= fj[1] + fk[1];
                f[i][2] -= fj[2] + fk[2];
                f[j][0] += fj[0];
                f[j][1] += fj[1];
                f[j][2] += fj[2];
                f[k][0] += fk[0];
                f[k][1] += fk[1];
                f[k][2] += fk[2];

                if (evflag) ev_tally3(i,j,k,evdwl,0.0,fj,fk,delr1,delr2);
            }
        }
    }

    if (vflag_fdotr) virial_fdotr_compute();
    //std::cout << "We computed " << benchmarkCounts.pairsComputed << " pairs." << std::endl;
    //std::cout << "This is " << benchmarkCounts.pairsComputed*(6+4+4+6)/1e6 << " megabytes of memory." << std::endl;
}

/* ---------------------------------------------------------------------- */

void PairUSC::allocate()
{
    allocated = 1;
    int n = atom->ntypes;

    memory->create(setflag,n+1,n+1,"pair:setflag");
    memory->create(cutsq,n+1,n+1,"pair:cutsq");

    map = new int[n+1];
}

/* ----------------------------------------------------------------------
   global settings
------------------------------------------------------------------------- */

void PairUSC::settings(int narg, char **arg)
{
    if (narg != 0) error->all(FLERR,"Illegal pair_style command");
}

/* ----------------------------------------------------------------------
   set coeffs for one or more type pairs
------------------------------------------------------------------------- */

void PairUSC::coeff(int narg, char **arg)
{
    int i,j,n;

    if (!allocated) allocate();
    if (narg != 3 + atom->ntypes) {
        error->all(FLERR,"Incorrect args for pair coefficients");
    }

    // insure I,J args are * *
    if (strcmp(arg[0],"*") != 0 || strcmp(arg[1],"*") != 0) {
        error->all(FLERR,"Incorrect args for pair coefficients");
    }

    // read args that map atom types to elements in potential file
    // map[i] = which element the Ith atom type is, -1 if NULL
    // nelements = # of unique elements
    // elements = list of element names

    if (elements) {
        for (i = 0; i < nelements; i++) delete [] elements[i];
        delete [] elements;
    }
    elements = new char*[atom->ntypes];
    for (i = 0; i < atom->ntypes; i++) elements[i] = NULL;

    nelements = 0;
    for (i = 3; i < narg; i++) {
        if (strcmp(arg[i],"NULL") == 0) {
            map[i-2] = -1;
            continue;
        }

        for (j = 0; j < nelements; j++) {
            if (strcmp(arg[i], elements[j]) == 0) break;
        }

        map[i-2] = j;
        if (j == nelements) {
            n = strlen(arg[i]) + 1;
            elements[j] = new char[n];
            strcpy(elements[j],arg[i]);
            // Also store which atomtypes that are O, Si and H so we recognize them later
            if(strcmp(elements[j], "O") == 0) oxygenType = j;
            else if(strcmp(elements[j], "Si") == 0) siliconType = j;
            else if(strcmp(elements[j], "H") == 0) hydrogenType = j;
            nelements++;
        }
    }

    // read potential file and initialize potential parameters

    read_file(arg[2]);
    setup();

    // clear setflag since coeff() called once with I,J = * *

    n = atom->ntypes;
    for (int i = 1; i <= n; i++)
        for (int j = i; j <= n; j++)
            setflag[i][j] = 0;

    // set setflag i,j for type pairs where both are mapped to elements

    int count = 0;
    for (int i = 1; i <= n; i++)
        for (int j = i; j <= n; j++)
            if (map[i] >= 0 && map[j] >= 0) {
                setflag[i][j] = 1;
                count++;
            }

    if (count == 0) error->all(FLERR,"Incorrect args for pair coefficients");
}

/* ----------------------------------------------------------------------
   init specific to this pair style
------------------------------------------------------------------------- */

void PairUSC::init_style()
{
    if (atom->tag_enable == 0)
        error->all(FLERR,"Pair style USC requires atom IDs");
    if (force->newton_pair == 0)
        error->all(FLERR,"Pair style USC requires newton pair on");

    // need a full neighbor list

    int irequest = neighbor->request(this,instance_me);
    neighbor->requests[irequest]->half = 0;
    neighbor->requests[irequest]->full = 1;
}

/* ----------------------------------------------------------------------
   init for one type pair i,j and corresponding j,i
------------------------------------------------------------------------- */

double PairUSC::init_one(int i, int j)
{
    if (setflag[i][j] == 0) error->all(FLERR,"All pair coeffs are not set");

    return cutmax;
}

/* ---------------------------------------------------------------------- */

void PairUSC::read_file(char *file)
{
    int params_per_line = 16;
    char **words = new char*[params_per_line+10];

    memory->sfree(params);
    params = NULL;
    nparams = maxparam = 0;

    // open file on proc 0

    FILE *fp;
    if (comm->me == 0) {
        fp = force->open_potential(file);
        if (fp == NULL) {
            char str[128];
            sprintf(str,"Cannot open USC potential file %s",file);
            error->one(FLERR,str);
        }
    }

    // read each set of params from potential file
    // one set of params can span multiple lines
    // store params if all 3 element tags are in element list

    int n,nwords,ielement,jelement,kelement;
    char line[MAXLINE],*ptr;
    int eof = 0;

    while (1) {
        if (comm->me == 0) {
            ptr = fgets(line,MAXLINE,fp);
            if (ptr == NULL) {
                eof = 1;
                fclose(fp);
            } else n = strlen(line) + 1;
        }
        MPI_Bcast(&eof,1,MPI_INT,0,world);
        if (eof) break;
        MPI_Bcast(&n,1,MPI_INT,0,world);
        MPI_Bcast(line,n,MPI_CHAR,0,world);

        // strip comment, skip line if blank

        if ((ptr = strchr(line,'#'))) *ptr = '\0';
        nwords = atom->count_words(line);
        if (nwords == 0) continue;

        if (nwords != params_per_line && nwords != params_per_line+1) {
            std::cout << "Error in line: " << line << std::endl;
            std::cout << "Expected " << params_per_line << " or " << params_per_line+1 << " words but found " << nwords << ". " << std::endl;
            error->all(FLERR,"Incorrect format in USC potential file");
        }

        bool oo_water = false;
        if(nwords == params_per_line+1) {
            // If so, the last word in the line is specifying whether or not this is water
            // Then put the numbers in the oo_water parameter struct.
            oo_water = true;
        }

        // words = ptrs to all words in line

        nwords = 0;
        words[nwords++] = strtok(line," \t\n\r\f");
        while ((words[nwords++] = strtok(NULL," \t\n\r\f"))) continue;

        // ielement,jelement,kelement = 1st args
        // if all 3 args are in element list, then parse this line
        // else skip to next entry in file

        for (ielement = 0; ielement < nelements; ielement++)
            if (strcmp(words[0],elements[ielement]) == 0) break;
        if (ielement == nelements) continue;
        for (jelement = 0; jelement < nelements; jelement++)
            if (strcmp(words[1],elements[jelement]) == 0) break;
        if (jelement == nelements) continue;
        for (kelement = 0; kelement < nelements; kelement++)
            if (strcmp(words[2],elements[kelement]) == 0) break;
        if (kelement == nelements) continue;

        // load up parameter settings and error check their values

        if (nparams == maxparam) {
            maxparam += DELTA;
            params = (Param *) memory->srealloc(params,maxparam*sizeof(Param),
                                                "pair:params");
        }

        Param *parameters = &params[nparams];
        if(oo_water) {
            // The last word should be oo_water
            if(strcmp(words[params_per_line], "oo_water") != 0) {
                error->all(FLERR,"Expected keyword oo_water in the coefficients file.");
            }
            parameters = &paramsOOWater;
        }

        parameters->ielement = ielement;
        parameters->jelement = jelement;
        parameters->kelement = kelement;
        parameters->r1s = atof(words[3]);
        parameters->r4s = atof(words[4]);
        parameters->rc = atof(words[5]);
        parameters->Zi = atof(words[6]);
        parameters->Zj = atof(words[7]);
        parameters->eta = atof(words[8]);
        parameters->H = atof(words[9]);
        parameters->D = atof(words[10]);
        parameters->B = atof(words[11]);
        parameters->costheta = atof(words[12]);
        parameters->ksi = atof(words[13]);
        parameters->r0 = atof(words[14]);
        parameters->W = atof(words[15]);

        if(!oo_water) nparams++;
    }

    delete [] words;
}

/* ---------------------------------------------------------------------- */

void PairUSC::setup()
{
    int i,j,k,m,n;
    double rtmp;
    potentialTable.numTabulatedEntries = 8192;
    // set elem2param for all triplet combinations
    // must be a single exact match to lines read from file
    // do not allow for ACB in place of ABC

    memory->destroy(potentialTable.forceOOWater);
    memory->destroy(potentialTable.potentialOOWater);
    memory->destroy(potentialTable.force);
    memory->destroy(potentialTable.potential);
    memory->destroy(elem2param);

    memory->create(elem2param,nelements,nelements,nelements, "pair:elem2param");
    memory->create(potentialTable.force, nelements, nelements, potentialTable.numTabulatedEntries, "pair:forceTabulated");
    memory->create(potentialTable.potential, nelements, nelements, potentialTable.numTabulatedEntries, "pair:potentialTabulated");
    memory->create(potentialTable.forceOOWater, potentialTable.numTabulatedEntries, "pair:forceTabulated:oo_water");
    memory->create(potentialTable.potentialOOWater, potentialTable.numTabulatedEntries, "pair:potentialTabulated:oo_water");

    for (i = 0; i < nelements; i++) {
        for (j = 0; j < nelements; j++) {
            for (k = 0; k < nelements; k++) {
                n = -1;
                for (m = 0; m < nparams; m++) {
                    if (i == params[m].ielement && j == params[m].jelement && k == params[m].kelement) {
                        if (n >= 0) {
                            std::cout << "Error, found duplicate entry for types " << elements[i] << " " << elements[j] << " " << elements[k] << std::endl;
                            error->all(FLERR,"Potential file has duplicate entry");
                        }
                        n = m;
                    }
                }
                if (n < 0) error->all(FLERR,"Potential file is missing an entry");
                elem2param[i][j][k] = n;
            }
        }
    }


    // compute parameter values derived from inputs
    // Unit conversion: https://en.wikipedia.org/wiki/Atomic_units
    // Metal units use angstroms as distance and eV as energy.

    for (m = 0; m < nparams; m++) {
        updatePotentialParameters(params[m]);
    }
    updatePotentialParameters(paramsOOWater);

    // set cutmax to max of all params

    cutmax = 0.0;
    for (m = 0; m < nparams; m++) {
        rtmp = sqrt(params[m].rcsq);
        if (rtmp > cutmax) cutmax = rtmp;
    }

    for (i = 0; i < nelements; i++) {
        for (j = 0; j < nelements; j++) {
            int ijparam = elem2param[i][j][j];
            createForceAndPotentialTables(&params[ijparam], i, j);
        }
    }

    // itype and jtype not used in this case, they are only used to place values
    // in correct arrays.
    createForceAndPotentialTables(&paramsOOWater, 0, 0);

    if(nelements > 2) {
        if(oxygenType < 0) {
            error->all(FLERR,"Oxygen atom type not found");
        }

        if(siliconType < 0) {
            error->all(FLERR,"Silicon atom type not found");
        }

        if(hydrogenType < 0) {
            error->all(FLERR,"Hydrogen atom type not found");
        }
        // If we have more than two elements, we will need to interpolate OO pairs between
        // Silica parameters and Water parameters. Create the coord/usc compute.
        id_coord = "coord_usc";
        char **newarg = new char*[11];
        newarg[0] = id_coord;
        newarg[1] = (char *) "all";
        newarg[2] = (char *) "coord/usc";

        newarg[3] = new char[10];
        // The +1 is because all the types are relabeled with -1 since they start at 1
        // in the data file whereas we here use them as array indices.
        // The coord/usc expects the real atom types as they appear in the data file.

        // TODO: Let the 2.0, 0.3, 1.4 and 0.3 values be input values from coefficients file
        // Maybe let the user create the coord/usc manually instead?
        sprintf(newarg[3], "%d", oxygenType+1);
        newarg[4] = new char[10];
        sprintf(newarg[4], "%d", siliconType+1);
        newarg[5] = (char *) "2.0";
        newarg[6] = (char *) "0.3";

        newarg[7] = new char[10];
        sprintf(newarg[7], "%d", oxygenType+1);
        newarg[8] = new char[10];
        sprintf(newarg[8], "%d", hydrogenType+1);
        newarg[9] = (char *) "1.4";
        newarg[10] = (char *) "0.3";

        // 11 arguments above
        modify->add_compute(11,newarg);
        // Manually delete these (created with new char[10]).
        delete newarg[3];
        delete newarg[4];
        delete newarg[7];
        delete newarg[8];
        delete [] newarg;
    }

    R.resize(nelements, 0.0);
    D.resize(nelements, 0.0);
    dg.resize(nelements, 0.0);
    R[siliconType] = 2.0; R[hydrogenType] = 1.4;
    D[siliconType] = 0.3; D[hydrogenType] = 0.3;

    int numberOfActiveTabulatedEntries = potentialTable.numTabulatedEntries-1;
    double rMinSquared = 0;
    double rMaxSquared = cutmax*cutmax;
    potentialTable.deltaR2 = (rMaxSquared - rMinSquared) / (numberOfActiveTabulatedEntries-1);
    potentialTable.oneOverDeltaR2 = 1.0/potentialTable.deltaR2;

    gradThetaTable.resize(nelements, std::vector<double>(potentialTable.numTabulatedEntries,0));

    // Now create V and F, and add/subtact V0 and F0 as specified above
    for(int element = 0; element < nelements; element++) {
        for(int i=0; i<potentialTable.numTabulatedEntries; i++) {
            double rSquared = i*potentialTable.deltaR2;
            double r = sqrt(rSquared);
            double oneOverD = 1.0/D[element];
            double value = 0.5*oneOverD*(cos(M_PI*(D[element]-R[element]+r)*oneOverD) - 1.0)/r;
            gradThetaTable[element][i] = value;
        }
    }

}

void PairUSC::updatePotentialParameters(PairUSC::Param &param)
{
    double energyConversionFactor = 27.2116; // atomic units to eV
    double lengthConversionFactor = 0.5291772; // atomic units [bohr] to angstroms

    param.rcsq = param.rc*param.rc;
    param.r0sq = param.r0*param.r0;
    param.ZiZj = param.Zi*param.Zj;

    // Unit conversion between paper units and metal units
    param.ZiZj *= lengthConversionFactor*energyConversionFactor;
    param.D *= lengthConversionFactor*energyConversionFactor;

    param.oneOverR1s = 1.0 / param.r1s;
    param.oneOverR4s = 1.0 / param.r4s;

    param.HTimesEta = param.H*param.eta;
    param.ZiZjOverR1s = param.ZiZj*param.oneOverR1s;
    param.twoTimesD = 2.0*param.D;
    param.DOverR4sHalf = 0.5*param.D*param.oneOverR4s;
    param.twoTimesB = param.B*2.0;
}

void PairUSC::computeForcesAndEnergyForOO(int i, int j, int potentialTableIndex, double fractionalRest, int evflag, Compute *computeCoord)
{
    double **x = atom->x;
    double **f = atom->f;
    int *type = atom->type;
    int *numneigh = list->numneigh;
    int **firstneigh = list->firstneigh;

    tagint itag = atom->tag[i];
    tagint jtag = atom->tag[j];

    // Position of atom i
    double xi = x[i][0];
    double yi = x[i][1];
    double zi = x[i][2];

    // Position of atom j
    double xj = x[j][0];
    double yj = x[j][1];
    double zj = x[j][2];

    double dxij = xi - xj;
    double dyij = yi - yj;
    double dzij = zi - zj;

    // This is Newton's thir law
    bool computeIJForces = true;
    if (itag > jtag) {
        if ((itag+jtag) % 2 == 0) computeIJForces = false;
    } else if (itag < jtag) {
        if ((itag+jtag) % 2 == 1) computeIJForces = false;
    } else {
        if (x[j][2] < zi) computeIJForces = false;
        if (x[j][2] == zi && x[j][1] < yi) computeIJForces = false;
        if (x[j][2] == zi && x[j][1] == yi && x[j][0] < xi) computeIJForces = false;
    }

    // Shorthand notation for fractionalRest. This is the interpolation weight between two table values for force and energy.
    double fr = fractionalRest;

    // First calculate the force for oxygen in water
    double force0 = potentialTable.forceOOWater[potentialTableIndex];
    double force1 = potentialTable.forceOOWater[potentialTableIndex+1];
    const double forceWater = (1.0 - fr)*force0 + fr*force1;

    // Then calculate the force for oxygen in silica
    force0 = potentialTable.force[oxygenType][oxygenType][potentialTableIndex];
    force1 = potentialTable.force[oxygenType][oxygenType][potentialTableIndex+1];
    const double forceSilica = (1.0 - fr)*force0 + fr*force1;

    // Now calculate the energy for oxygen in water
    double potential0 = potentialTable.potentialOOWater[potentialTableIndex];
    double potential1 = potentialTable.potentialOOWater[potentialTableIndex+1];
    const double potentialWater = (1.0 - fr)*potential0 + fr*potential1;

    potential0 = potentialTable.potential[oxygenType][oxygenType][potentialTableIndex];
    potential1 = potentialTable.potential[oxygenType][oxygenType][potentialTableIndex+1];
    const double potentialSilica = (1.0 - fr)*potential0 + fr*potential1;

    double NiSi = computeCoord->array_atom[i][0];
    double NiH = computeCoord->array_atom[i][1];
    double NjSi = computeCoord->array_atom[j][0];
    double NjH = computeCoord->array_atom[j][1];
    if(NiSi == 0 && NiH == 0 && NjSi == 0 && NjH == 0) {
        NjSi = 1.0;
        NiSi = 1.0;
    }
    const double oneOverCoordinateNumbers = 1.0 / (NiH + NjH + NiSi + NjSi);

    // This is the interpolation weight between silica and water
    const double g = 1.0 - (NiH + NjH) * oneOverCoordinateNumbers;

    dg[siliconType] = -(NiH + NjH)*oneOverCoordinateNumbers*oneOverCoordinateNumbers;
    dg[hydrogenType] = (NiSi + NjSi)*oneOverCoordinateNumbers*oneOverCoordinateNumbers;

    // Interpolated potential and force between silica and water
    const double V0 = g*potentialSilica+(1.0-g)*potentialWater;
    const double F0 = g*forceSilica+(1.0-g)*forceWater;

    if(computeIJForces) {
        f[i][0] += F0*dxij;
        f[i][1] += F0*dyij;
        f[i][2] += F0*dzij;
        f[j][0] -= F0*dxij;
        f[j][1] -= F0*dyij;
        f[j][2] -= F0*dzij;

        if(evflag) ev_tally(i,j,atom->nlocal,force->newton_pair,V0,0.0,F0,dxij,dyij,dzij);
    }

    // Neighbor list for atom i
    const int *neighborlist = firstneigh[i];
    const int numNeighbors = numneigh[i];

    //#pragma simd reduction(+: forceFromNeighborX, forceFromNeighborY, forceFromNeighborZ)
    for (int kk = 0; kk < numNeighbors; kk++) {
        const int k = neighborlist[kk] & NEIGHMASK;

        int ktype = type[k];
        if(ktype != (hydrogenType+1) && ktype != (siliconType+1)) {
            continue;
        }

        ktype = map[ktype];
        const double dx = xi - x[k][0];
        const double dy = yi - x[k][1];
        const double dz = zi - x[k][2];
        const double rSquared = dx*dx + dy*dy + dz*dz;
        
        // The derivative of the interpolation function theta(r)
        if(withinRange(rSquared, R[ktype], D[ktype])) {
            const int tableIndex = rSquared*potentialTable.oneOverDeltaR2;
            const double fractionalRest = rSquared*potentialTable.oneOverDeltaR2 - tableIndex; // double - int will only keep the 0.xxxx part
            const double value0 = gradThetaTable[ktype][tableIndex];
            const double value1 = gradThetaTable[ktype][tableIndex+1];
            const double value = (1.0 - fractionalRest)*value0 + fractionalRest*value1;

            const double dgTimesGradTheta = dg[ktype]*value;
            const double F = dgTimesGradTheta*(potentialSilica - potentialWater);

            f[i][0] += F*dx;
            f[i][1] += F*dy;
            f[i][2] += F*dz;
            f[k][0] -= F*dx;
            f[k][1] -= F*dy;
            f[k][2] -= F*dz;
        }
    }

    if(j < atom->nlocal && computeIJForces) {
        // Neighbor list for atom j
        const int *neighborlist = firstneigh[j];
        const int numNeighbors = numneigh[j];

        //#pragma simd reduction(+: forceFromNeighborX, forceFromNeighborY, forceFromNeighborZ)
        for (int kk = 0; kk < numNeighbors; kk++) {
            const int k = neighborlist[kk] & NEIGHMASK;
            int ktype = type[k];
            if(ktype != (hydrogenType+1) && ktype != (siliconType+1)) {
                continue;
            }

            ktype = map[ktype];
            const double dx = xj - x[k][0];
            const double dy = yj - x[k][1];
            const double dz = zj - x[k][2];
            const double rSquared = dx*dx + dy*dy + dz*dz;
            
//            The derivative of the interpolation function theta(r)
            if(withinRange(rSquared, R[ktype], D[ktype])) {
                const int tableIndex = rSquared*potentialTable.oneOverDeltaR2;
                const double fractionalRest = rSquared*potentialTable.oneOverDeltaR2 - tableIndex; // double - int will only keep the 0.xxxx part
                const double value0 = gradThetaTable[ktype][tableIndex];
                const double value1 = gradThetaTable[ktype][tableIndex+1];
                const double value = (1.0 - fractionalRest)*value0 + fractionalRest*value1;

                const double dgTimesGradTheta = dg[ktype]*value;

                const double F = dgTimesGradTheta*(potentialSilica - potentialWater);
                f[j][0] += F*dx;
                f[j][1] += F*dy;
                f[j][2] += F*dz;
                f[k][0] -= F*dx;
                f[k][1] -= F*dy;
                f[k][2] -= F*dz;
            }
        }
    }
}

void PairUSC::createForceAndPotentialTables(Param *param, int element1, int element2)
{
    // Strategy:
    // We compute V0 and F0 which are unshifted potentials and forces.
    // We then introduce a new potential
    // V(r) = V0(r) - V0(rCut) + (r-rCut)*F0(rCut)
    // which gives a new force
    // F(r) = grad(V(r)) = F0(r) - F0(rCut)
    // Now both V and F are zero at rCut.

    // We need that extra point for interpolation
    int numberOfActiveTabulatedEntries = potentialTable.numTabulatedEntries-1;

    double rMinSquared = 0;
    double rMaxSquared = cutmax*cutmax;
    potentialTable.deltaR2 = (rMaxSquared - rMinSquared) / (numberOfActiveTabulatedEntries-1);
    potentialTable.oneOverDeltaR2 = 1.0/potentialTable.deltaR2;

    // If we are working on the special case ooWater, use the other table
    // Therefore we use two pointers that will point to the correct array
    bool ooWater = param == &paramsOOWater;
    double *potentialArray = NULL;
    double *forceArray = NULL;

    if(ooWater) {
        potentialArray = potentialTable.potentialOOWater;
        forceArray = potentialTable.forceOOWater;
    } else {
        potentialArray = potentialTable.potential[element1][element2];
        forceArray = potentialTable.force[element1][element2];
    }

    // First compute force and energy at rCut
    double rCut = param->rc;
    double rCutSquared = rCut*rCut;
    double forceAtRCut = 0;
    double energyAtRCut = 0;
    twobody(param, rCutSquared, forceAtRCut, energyAtRCut);

    // Now create V and F, and add/subtact V0 and F0 as specified above
    for(int i=0; i<potentialTable.numTabulatedEntries; i++) {
        double rsq = i*potentialTable.deltaR2;
        double r = sqrt(rsq);
        double oneOverR = 1.0 / r;

        double energy = 0;
        double force = 0;
        twobody(param, rsq, force, energy);

        energy = energy - energyAtRCut + (r-rCut)*forceAtRCut;
        potentialArray[i] = energy;

        force = (force - forceAtRCut);
        forceArray[i] = force*oneOverR;
        if(element1 == hydrogenType && element2 == oxygenType) {
            // std::cout << r << "   " << energy << "    " << force*oneOverR << std::endl;
        }
    }
}

/* ---------------------------------------------------------------------- */

void PairUSC::twobody(Param *param, double rSquared, double &force, double &energy)
{
    double r=sqrt(rSquared);
    double oneOverR = 1.0/r;

    double rep = param->H*pow(oneOverR,param->eta);
    double col = param->ZiZj*oneOverR*exp(-r*param->oneOverR1s);
    double pol =  -0.5*param->D*pow(oneOverR,4)*exp(-r*param->oneOverR4s);
    if(param->D == 0) pol = 0.0;

    double vdw = -param->W*pow(oneOverR,6);
    force = 0.0;
    energy = 0;
    if(param->H != 0) {
        force += rep*param->eta*oneOverR;
        energy += rep;
    }
    if(param->ZiZj != 0) {
        force += col*(oneOverR+param->oneOverR1s);
        energy += col;
    }
    if(param->D != 0) {
        force += pol*(4.0*oneOverR+param->oneOverR4s);
        energy += pol;
    }
    if(param->W != 0) {
        force += vdw*6.0*oneOverR;
        energy += vdw;
    }
}

/* ---------------------------------------------------------------------- */

void PairUSC::threebody(Param *paramij, Param *paramik, Param *paramijk,
                        double rsq1, double rsq2,
                        double *delr1, double *delr2,
                        double *fj, double *fk, int eflag, double &eng)
{
    double r1 = sqrt(rsq1);
    double oneOverR1 = 1.0/r1;
    double rinvsq1 = oneOverR1*oneOverR1;
    double rainv1 = 1.0/(r1 - paramij->r0);
    double gsrainv1 = paramij->ksi * rainv1;
    double gsrainvsq1 = gsrainv1*rainv1*oneOverR1;

    double r2 = sqrt(rsq2);
    double oneOverR2 = 1.0/r2;
    double rinvsq2 = oneOverR2*oneOverR2;
    double rainv2 = 1.0/(r2 - paramik->r0);
    double gsrainv2 = paramik->ksi * rainv2;
    double gsrainvsq2 = gsrainv2*rainv2*oneOverR2;

    double rinv12 = oneOverR1*oneOverR2;
    double cs = (delr1[0]*delr2[0] + delr1[1]*delr2[1] + delr1[2]*delr2[2]) * rinv12; // cos(theta) = dot(r_ij, r_ik)/(|r_ij|*|r_ik|)
    double delcs = cs - paramijk->costheta;
    double delcssq = delcs*delcs; // (cos(theta) - cos(theta_0))^2

    double facexp = exp(gsrainv1+gsrainv2); // exp( ksi/(r_ij - r0) + ksi/(r_ik - r0) )

    double facrad = paramijk->B * facexp*delcssq; // B*exp( ksi/(r_ij - r0) + ksi/(r_ik - r0) )*(cos(theta) - cos(theta_0))^2, energy of 3-particle forces
    double frad1 = facrad*gsrainvsq1;
    double frad2 = facrad*gsrainvsq2;
    double facang = paramijk->twoTimesB * facexp*delcs; // 2B*exp( ksi/(r_ij - r0) + ksi/(r_ik - r0) )*(cos(theta) - cos(theta_0))
    double facang12 = rinv12*facang;
    double csfacang = cs*facang;
    double csfac1 = rinvsq1*csfacang;

    fj[0] = delr1[0]*(frad1+csfac1)-delr2[0]*facang12;
    fj[1] = delr1[1]*(frad1+csfac1)-delr2[1]*facang12;
    fj[2] = delr1[2]*(frad1+csfac1)-delr2[2]*facang12;

    double csfac2 = rinvsq2*csfacang;

    fk[0] = delr2[0]*(frad2+csfac2)-delr1[0]*facang12;
    fk[1] = delr2[1]*(frad2+csfac2)-delr1[1]*facang12;
    fk[2] = delr2[2]*(frad2+csfac2)-delr1[2]*facang12;

    if (eflag) eng = facrad;
}
