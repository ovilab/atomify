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

using namespace LAMMPS_NS;
#define TABULATED

#define MAXLINE 1024
#define DELTA 4

/* ---------------------------------------------------------------------- */

PairUSC::PairUSC(LAMMPS *lmp) : Pair(lmp)
{
    single_enable = 0;
    restartinfo = 0;
    one_coeff = 1;
    manybody_flag = 1;

    nelements = 0;
    elements = NULL;
    nparams = maxparam = 0;
    params = NULL;
    elem2param = NULL;
    potentialTable.force = NULL;
    potentialTable.potential = NULL;
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
    int i,j,k,ii,jj,kk,inum,jnum,jnumm1;
    int itype,jtype,ktype,ijparam,ikparam,ijkparam;
    tagint itag,jtag;
    double xtmp,ytmp,ztmp,delx,dely,delz,evdwl,fpair;
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

    // loop over full neighbor list of my atoms
    for (ii = 0; ii < inum; ii++) {
        i = ilist[ii];
        itag = tag[i];
        itype = map[type[i]];
        xtmp = x[i][0];
        ytmp = x[i][1];
        ztmp = x[i][2];

        // two-body interactions, skip half of them

        jlist = firstneigh[i];
        jnum = numneigh[i];

        for (jj = 0; jj < jnum; jj++) {
            j = jlist[jj];
            j &= NEIGHMASK;
            jtag = tag[j];

            if (itag > jtag) {
                if ((itag+jtag) % 2 == 0) continue;
            } else if (itag < jtag) {
                if ((itag+jtag) % 2 == 1) continue;
            } else {
                if (x[j][2] < ztmp) continue;
                if (x[j][2] == ztmp && x[j][1] < ytmp) continue;
                if (x[j][2] == ztmp && x[j][1] == ytmp && x[j][0] < xtmp) continue;
            }

            jtype = map[type[j]];

            delx = xtmp - x[j][0];
            dely = ytmp - x[j][1];
            delz = ztmp - x[j][2];
            rsq = delx*delx + dely*dely + delz*delz;

            ijparam = elem2param[itype][jtype][jtype];
            if (rsq > params[ijparam].rcsq) continue;

#ifdef TABULATED
            int potentialTableIndex = rsq*potentialTable.oneOverDeltaR2;
            double fractionalRest = rsq*potentialTable.oneOverDeltaR2 - potentialTableIndex; // double - int will only keep the 0.xxxx part

            if(potentialTableIndex >= potentialTable.numTabulatedEntries) {
                std::cout << "Error, potential table out of bounds." << std::endl;
                exit(1);
            }

            double force0 = potentialTable.force[itype][jtype][potentialTableIndex];
            double force1 = potentialTable.force[itype][jtype][potentialTableIndex+1];
            double force = (1.0 - fractionalRest)*force0 + fractionalRest*force1;

            f[i][0] += delx*force;
            f[i][1] += dely*force;
            f[i][2] += delz*force;
            f[j][0] -= delx*force;
            f[j][1] -= dely*force;
            f[j][2] -= delz*force;


#else
            twobody(&params[ijparam],rsq,fpair,eflag,evdwl);
            f[i][0] += delx*fpair;
            f[i][1] += dely*fpair;
            f[i][2] += delz*fpair;
            f[j][0] -= delx*fpair;
            f[j][1] -= dely*fpair;
            f[j][2] -= delz*fpair;
#endif

            if (evflag) {
#ifdef TABULATED
                double energy0 = potentialTable.potential[itype][jtype][potentialTableIndex];
                double energy1 = potentialTable.potential[itype][jtype][potentialTableIndex+1];
                double energy = (1.0 - fractionalRest)*energy0 + fractionalRest*energy1;
                ev_tally(i,j,nlocal,newton_pair,energy,0.0,fpair,delx,dely,delz);
#else
                ev_tally(i,j,nlocal,newton_pair,
                                 evdwl,0.0,fpair,delx,dely,delz);
#endif
            }
        }

        jnumm1 = jnum - 1;

        for (jj = 0; jj < jnumm1; jj++) {
            j = jlist[jj];
            j &= NEIGHMASK;
            jtag = tag[j];
            jtype = map[type[j]];
            ijparam = elem2param[itype][jtype][jtype];
            delr1[0] = x[j][0] - xtmp;
            delr1[1] = x[j][1] - ytmp;
            delr1[2] = x[j][2] - ztmp;
            rsq1 = delr1[0]*delr1[0] + delr1[1]*delr1[1] + delr1[2]*delr1[2];
            if (rsq1 > params[ijparam].r0sq) continue;

            for (kk = jj+1; kk < jnum; kk++) {
                k = jlist[kk];
                k &= NEIGHMASK;

                ktype = map[type[k]];
                ikparam = elem2param[itype][ktype][ktype];
                ijkparam = elem2param[itype][jtype][ktype];

                delr2[0] = x[k][0] - xtmp;
                delr2[1] = x[k][1] - ytmp;
                delr2[2] = x[k][2] - ztmp;
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

    if (narg != 3 + atom->ntypes)
        error->all(FLERR,"Incorrect args for pair coefficients");

    // insure I,J args are * *

    if (strcmp(arg[0],"*") != 0 || strcmp(arg[1],"*") != 0)
        error->all(FLERR,"Incorrect args for pair coefficients");

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
        for (j = 0; j < nelements; j++)
            if (strcmp(arg[i],elements[j]) == 0) break;
        map[i-2] = j;
        if (j == nelements) {
            n = strlen(arg[i]) + 1;
            elements[j] = new char[n];
            strcpy(elements[j],arg[i]);
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
    char **words = new char*[params_per_line+1];

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

        // concatenate additional lines until have params_per_line words

        while (nwords < params_per_line) {
            n = strlen(line);
            if (comm->me == 0) {
                ptr = fgets(&line[n],MAXLINE-n,fp);
                if (ptr == NULL) {
                    eof = 1;
                    fclose(fp);
                } else n = strlen(line) + 1;
            }
            MPI_Bcast(&eof,1,MPI_INT,0,world);
            if (eof) break;
            MPI_Bcast(&n,1,MPI_INT,0,world);
            MPI_Bcast(line,n,MPI_CHAR,0,world);
            if ((ptr = strchr(line,'#'))) *ptr = '\0';
            nwords = atom->count_words(line);
        }

        if (nwords != params_per_line)
            error->all(FLERR,"Incorrect format in USC potential file");

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

        params[nparams].ielement = ielement;
        params[nparams].jelement = jelement;
        params[nparams].kelement = kelement;
        params[nparams].r1s = atof(words[3]);
        params[nparams].r4s = atof(words[4]);
        params[nparams].rc = atof(words[5]);
        params[nparams].Zi = atof(words[6]);
        params[nparams].Zj = atof(words[7]);
        params[nparams].eta = atof(words[8]);
        params[nparams].H = atof(words[9]);
        params[nparams].D = atof(words[10]);
        params[nparams].B = atof(words[11]);
        params[nparams].costheta = atof(words[12]);
        params[nparams].ksi = atof(words[13]);
        params[nparams].r0 = atof(words[14]);
        params[nparams].tol = atof(words[15]);

        nparams++;
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

    memory->destroy(potentialTable.force);
    memory->destroy(potentialTable.potential);
    memory->destroy(elem2param);
    memory->create(elem2param,nelements,nelements,nelements, "pair:elem2param");
    memory->create(potentialTable.force, nelements, nelements, potentialTable.numTabulatedEntries, "pair:force_tabulated");
    memory->create(potentialTable.potential, nelements, nelements, potentialTable.numTabulatedEntries, "pair:potential_tabulated");

    for (i = 0; i < nelements; i++) {
        for (j = 0; j < nelements; j++) {
            for (k = 0; k < nelements; k++) {
                n = -1;
                for (m = 0; m < nparams; m++) {
                    if (i == params[m].ielement && j == params[m].jelement &&
                            k == params[m].kelement) {
                        if (n >= 0) error->all(FLERR,"Potential file has duplicate entry");
                        n = m;
                    }
                }
                if (n < 0) error->all(FLERR,"Potential file is missing an entry");
                elem2param[i][j][k] = n;
            }
        }
    }


    // compute parameter values derived from inputs

    // set cutsq using shortcut to reduce neighbor list for accelerated
    // calculations. cut must remain unchanged as it is a potential parameter
    // (cut = a*sigma)

    // Unit conversion: https://en.wikipedia.org/wiki/Atomic_units
    double energyConversionFactor = 27.2116;
    double lengthConversionFactor = 0.5291772; // bohr
    for (m = 0; m < nparams; m++) {
        rtmp = params[m].rc;
        params[m].rcsq = rtmp * rtmp;
        params[m].r0sq = params[m].r0*params[m].r0;
        params[m].ZiZj = params[m].Zi*params[m].Zj;

        // Unit conversion between paper units and metal units
        params[m].ZiZj *= lengthConversionFactor*energyConversionFactor;
        params[m].D *= lengthConversionFactor*energyConversionFactor;

        params[m].oneOverR1s = 1.0 / params[m].r1s;
        params[m].oneOverR4s = 1.0 / params[m].r4s;

        params[m].c1 = params[m].H*params[m].eta;
        params[m].c2 = params[m].ZiZj*params[m].oneOverR1s;
        params[m].c3 = 2.0*params[m].D;
        params[m].c4 = 0.5*params[m].D*params[m].oneOverR4s;
        params[m].B2 = params[m].B*2.0;

        params[m].e_at_cutoff = 0;

        double energyAtCutoff = 0;
        double tmp = 0;

        twobody(&params[m], params[m].rcsq, tmp, 1, energyAtCutoff);
        params[m].e_at_cutoff = energyAtCutoff;
    }

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

//    for(int itype=0; itype<2; itype++) {
//        for(int jtype=0; jtype<2; jtype++) {
//            Param *param = &params[elem2param[itype][jtype][jtype]];
//            double rcsq = param->rcsq;
//            double force = 0;
//            double energy = 0;

//            std::cout << itype << "   " << jtype << "   ";
//            twobody(param, rcsq, force, 1, energy);
//            // std::cout << itype << "   " << jtype << "    " << energy << "    " << force << std::endl;
//        }
//    }
//    exit(1);

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

    // First compute force and energy at rCut
    double rCut = param->rc;
    double rCutSquared = rCut*rCut;
    double forceAtRCut = 0;
    double energyAtRCut = 0;
    twobody(param, rCutSquared, forceAtRCut, 1, energyAtRCut);

    // Generate V0
    for(int i=0; i<potentialTable.numTabulatedEntries; i++) {
        double rsq = i*potentialTable.deltaR2;
        double r = sqrt(rsq);
        double oneOverR = 1.0 / r;
        double oneOverR2 = oneOverR*oneOverR;
        double oneOverR3 = oneOverR2*oneOverR;
        double oneOverR4 = oneOverR2*oneOverR2;
        double expROverR1s = exp(-r*param->oneOverR1s);
        double expROverR4s = exp(-r*param->oneOverR4s);
        double oneOverREta = pow(r, -param->eta);

        double V0 = param->H*oneOverREta
                    + param->ZiZj*oneOverR*expROverR1s
                    - 0.5*param->D*oneOverR4*expROverR4s;

        V0 = V0 - energyAtRCut + (r - rCut)*forceAtRCut;

        potentialTable.potential[element1][element2][i] = V0;

        double F0 = (param->c1*oneOverREta // H/r^eta term
                      + param->ZiZj*oneOverR*expROverR1s // ZZ term (product rule #1)
                      + param->c2*expROverR1s // ZZ term (product rule #2)
                      - param->c3*oneOverR4*expROverR4s // D term (product rule #1)
                      - param->c4*oneOverR3*expROverR4s)*oneOverR2; // D term (product rule #2)
        F0 = F0 - forceAtRCut*oneOverR;
        potentialTable.force[element1][element2][i] = F0;

    }

    // Generate V and F as described above
    for(int i=0; i<potentialTable.numTabulatedEntries; i++) {
        // Swap so table with indices j,i  equal that of i,j
        potentialTable.force[element2][element1][i] = potentialTable.force[element1][element2][i];
        potentialTable.potential[element2][element1][i] = potentialTable.potential[element1][element2][i];
    }
}

/* ---------------------------------------------------------------------- */

void PairUSC::twobody(Param *param, double rsq, double &fforce,
                      int eflag, double &eng)
{
#ifdef TABULATED
    double r=sqrt(rsq);
    double ri = 1.0/r;
    double r1siv = param->oneOverR1s;
    double r4siv = param->oneOverR4s;
    double det = param->eta;
    double dhh = param->H;
    double facc = param->ZiZj;
    double facp = 0.5*param->D;
    double facw = 0.0;

    double rep = dhh*pow(ri,det);
    double col = facc*ri*exp(-r*r1siv);
    double pol =  -facp*pow(ri,4)*exp(-r*r4siv);
    double vdw = -facw*pow(ri,6);

    fforce = rep*det*ri+col*(ri+r1siv)+pol*(4.0*ri+r4siv)+vdw*6.0*ri;
    eng = rep+col+pol+vdw;

    return;
#else
    // double r = sqrt(rsq);
    double oneOverR = 1.0 / r;
    double oneOverR2 = oneOverR*oneOverR;
    double oneOverR3 = oneOverR2*oneOverR;
    double oneOverR4 = oneOverR2*oneOverR2;
    double expROverR1s = exp(-r*param->oneOverR1s);
    double expROverR4s = exp(-r*param->oneOverR4s);
    double oneOverREta = pow(r, -param->eta);

    fforce = (param->c1*oneOverREta // H/r^eta term
              + param->ZiZj*oneOverR*expROverR1s // ZZ term (product rule #1)
              + param->c2*expROverR1s // ZZ term (product rule #2)
              - param->c3*oneOverR4*expROverR4s // D term (product rule #1)
              - param->c4*oneOverR3*expROverR4s)*oneOverR2; // D term (product rule #2)

    if(eflag) {
        double energy = param->H*oneOverREta
                + param->ZiZj*oneOverR*expROverR1s
                - 0.5*param->D*oneOverR4*expROverR4s;
        eng = energy - param->e_at_cutoff;
    }
#endif
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
    double facang = paramijk->B2 * facexp*delcs; // 2B*exp( ksi/(r_ij - r0) + ksi/(r_ik - r0) )*(cos(theta) - cos(theta_0))
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
