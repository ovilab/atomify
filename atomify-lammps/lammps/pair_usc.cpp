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
            int type = 1;
            if(itype == 0 && jtype == 1) type = 2;
            if(itype == 1 && jtype == 0) type = 2;
            if(itype == 1 && jtype == 1) type = 3;

            ijparam = elem2param[itype][jtype][jtype];
            if (rsq > params[ijparam].rcsq) continue;

            twobody(&params[ijparam],rsq,fpair,eflag,evdwl);
            
            f[i][0] += delx*fpair;
            f[i][1] += dely*fpair;
            f[i][2] += delz*fpair;
            f[j][0] -= delx*fpair;
            f[j][1] -= dely*fpair;
            f[j][2] -= delz*fpair;

            if (evflag) ev_tally(i,j,nlocal,newton_pair,
                                 evdwl,0.0,fpair,delx,dely,delz);
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
                int ktag = tag[k];

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
    
    // set elem2param for all triplet combinations
    // must be a single exact match to lines read from file
    // do not allow for ACB in place of ABC
    
    memory->destroy(elem2param);
    memory->create(elem2param,nelements,nelements,nelements,"pair:elem2param");
    
    for (i = 0; i < nelements; i++)
        for (j = 0; j < nelements; j++)
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
    
    
    // compute parameter values derived from inputs
    
    // set cutsq using shortcut to reduce neighbor list for accelerated
    // calculations. cut must remain unchanged as it is a potential parameter
    // (cut = a*sigma)
    
    for (m = 0; m < nparams; m++) {
        rtmp = params[m].rc;
        params[m].rcsq = rtmp * rtmp;
        params[m].r0sq = params[m].r0*params[m].r0;
        params[m].ZiZj = params[m].Zi*params[m].Zj;
        params[m].oneOverR1s = 1.0 / params[m].r1s;
        params[m].oneOverR4s = 1.0 / params[m].r4s;

        params[m].c1 = params[m].H*params[m].eta;
        params[m].c2 = params[m].ZiZj*params[m].oneOverR1s;
        params[m].c3 = 2.0*params[m].D;
        params[m].c4 = 0.5*params[m].D*params[m].oneOverR4s;
        params[m].B2 = params[m].B*2.0;

        params[m].f_at_cutoff = 0;
        params[m].e_at_cutoff = 0;
        // twobody(&params[m], params[m].rcsq, params[m].f_at_cutoff, 1, params[m].e_at_cutoff);
        // params[m].f_at_cutoff *= params[m].rc;
    }
    
    // set cutmax to max of all params
    
    cutmax = 0.0;
    for (m = 0; m < nparams; m++) {
        rtmp = sqrt(params[m].rcsq);
        if (rtmp > cutmax) cutmax = rtmp;
    }

}

/* ---------------------------------------------------------------------- */

void PairUSC::twobody(Param *param, double rsq, double &fforce,
                      int eflag, double &eng)
{
    double r = sqrt(rsq);
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
        eng = param->H*oneOverREta
                + param->ZiZj*oneOverR*expROverR1s
                - 0.5*param->D*oneOverR4*expROverR4s;
        eng = eng - param->e_at_cutoff + (r - param->rc)*param->f_at_cutoff;
    }
}

/* ---------------------------------------------------------------------- */

void PairUSC::threebody(Param *paramij, Param *paramik, Param *paramijk,
                        double rsq1, double rsq2,
                        double *delr1, double *delr2,
                        double *fj, double *fk, int eflag, double &eng)
{
    double r1,rinvsq1,rainv1,gsrainv1,gsrainvsq1,expgsrainv1;
    double r2,rinvsq2,rainv2,gsrainv2,gsrainvsq2,expgsrainv2;
    double rinv12,cs,delcs,delcssq,facexp,facrad,frad1,frad2;
    double facang,facang12,csfacang,csfac1,csfac2;
    
    r1 = sqrt(rsq1);
    rinvsq1 = 1.0/rsq1;
    rainv1 = 1.0/(r1 - paramij->r0);
    gsrainv1 = paramij->ksi * rainv1;
    gsrainvsq1 = gsrainv1*rainv1/r1;
    expgsrainv1 = exp(gsrainv1); // exp(ksi/(r_ij - r0))
    
    r2 = sqrt(rsq2);
    rinvsq2 = 1.0/rsq2;
    rainv2 = 1.0/(r2 - paramik->r0);
    gsrainv2 = paramik->ksi * rainv2;
    gsrainvsq2 = gsrainv2*rainv2/r2;
    expgsrainv2 = exp(gsrainv2); // exp(ksi/(r_ik - r0))
    
    rinv12 = 1.0/(r1*r2);
    cs = (delr1[0]*delr2[0] + delr1[1]*delr2[1] + delr1[2]*delr2[2]) * rinv12; // cos(theta) = dot(r_ij, r_ik)/(|r_ij|*|r_ik|)
    delcs = cs - paramijk->costheta;
    delcssq = delcs*delcs; // (cos(theta) - cos(theta_0))^2
    
    facexp = expgsrainv1*expgsrainv2; // exp( ksi/(r_ij - r0) + ksi/(r_ik - r0) )

    facrad = paramijk->B * facexp*delcssq; // B*exp( ksi/(r_ij - r0) + ksi/(r_ik - r0) )*(cos(theta) - cos(theta_0))^2, energy of 3-particle forces
    frad1 = facrad*gsrainvsq1;
    frad2 = facrad*gsrainvsq2;
    facang = paramijk->B2 * facexp*delcs; // 2B*exp( ksi/(r_ij - r0) + ksi/(r_ik - r0) )*(cos(theta) - cos(theta_0))
    facang12 = rinv12*facang;
    csfacang = cs*facang;
    csfac1 = rinvsq1*csfacang;
    
    fj[0] = delr1[0]*(frad1+csfac1)-delr2[0]*facang12;
    fj[1] = delr1[1]*(frad1+csfac1)-delr2[1]*facang12;
    fj[2] = delr1[2]*(frad1+csfac1)-delr2[2]*facang12;
    
    csfac2 = rinvsq2*csfacang;
    
    fk[0] = delr2[0]*(frad2+csfac2)-delr1[0]*facang12;
    fk[1] = delr2[1]*(frad2+csfac2)-delr1[1]*facang12;
    fk[2] = delr2[2]*(frad2+csfac2)-delr1[2]*facang12;
    
    if (eflag) eng = facrad;
}
