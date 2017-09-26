#include "cpfixindent.h"
#include "../../lammpscontroller.h"
#include <input.h>
#include <domain.h>
#include <fix_indent.h>

CPFixIndent::CPFixIndent(Qt3DCore::QNode *parent) : CPFix(parent)
{

}

CPFixIndent::~CPFixIndent()
{

}


void CPFixIndent::copyData(LAMMPSController *lammpsController)
{
    LAMMPS_NS::FixIndent *lmp_fix = dynamic_cast<LAMMPS_NS::FixIndent *>(lammpsController->findFixByIdentifier(identifier()));
    if(lmp_fix == nullptr) return;
    int dim;
    int    *istyle = static_cast<int*>(lmp_fix->extract("istyle", dim));
    int    *cdim   = static_cast<int*>(lmp_fix->extract("cdim", dim));
    int    *xvar = static_cast<int*>(lmp_fix->extract("xvar", dim));
    int    *yvar   = static_cast<int*>(lmp_fix->extract("yvar", dim));
    int    *zvar   = static_cast<int*>(lmp_fix->extract("zvar", dim));
    int    *rvar   = static_cast<int*>(lmp_fix->extract("rvar", dim));
    int    *pvar   = static_cast<int*>(lmp_fix->extract("pvar", dim));
    double *xvalue = static_cast<double*>(lmp_fix->extract("xvalue", dim));
    double *yvalue = static_cast<double*>(lmp_fix->extract("yvalue", dim));
    double *zvalue = static_cast<double*>(lmp_fix->extract("zvalue", dim));
    double *rvalue = static_cast<double*>(lmp_fix->extract("rvalue", dim));
    double *pvalue = static_cast<double*>(lmp_fix->extract("pvalue", dim));
    char   *xstr   = static_cast<char*>(lmp_fix->extract("xstr", dim));
    char   *ystr   = static_cast<char*>(lmp_fix->extract("ystr", dim));
    char   *zstr   = static_cast<char*>(lmp_fix->extract("zstr", dim));
    char   *rstr   = static_cast<char*>(lmp_fix->extract("rstr", dim));
    char   *pstr   = static_cast<char*>(lmp_fix->extract("pstr", dim));
    if (*istyle == SPHERE) {
        // ctr = current indenter center
        // remap into periodic box

        double ctr[3];
        if (xstr) ctr[0] = lammpsController->lammps()->input->variable->compute_equal(*xvar);
        else ctr[0] = *xvalue;
        if (ystr) ctr[1] = lammpsController->lammps()->input->variable->compute_equal(*yvar);
        else ctr[1] = *yvalue;
        if (zstr) ctr[2] = lammpsController->lammps()->input->variable->compute_equal(*zvar);
        else ctr[2] = *zvalue;
        lammpsController->lammps()->domain->remap(ctr);
        double radius;
        if (rstr) radius = lammpsController->lammps()->input->variable->compute_equal(*rvar);
        else radius = *rvalue;
        qDebug() << "Found sphere with (x,y,z,r)=(" << ctr[0] << ", " << ctr[1] << ", " << ctr[2] << ", " << radius << ")";
    } else if (*istyle == CYLINDER) {
        // ctr = current indenter axis
        // remap into periodic box
        // 3rd coord is just near box for remap(), since isn't used

        double ctr[3];
        if (*cdim == 0) {
            ctr[0] = lammpsController->lammps()->domain->boxlo[0];
            if (ystr) ctr[1] = lammpsController->lammps()->input->variable->compute_equal(*yvar);
            else ctr[1] = *yvalue;
            if (zstr) ctr[2] = lammpsController->lammps()->input->variable->compute_equal(*zvar);
            else ctr[2] = *zvalue;
        } else if (*cdim == 1) {
            if (xstr) ctr[0] = lammpsController->lammps()->input->variable->compute_equal(*xvar);
            else ctr[0] = *xvalue;
            ctr[1] = lammpsController->lammps()->domain->boxlo[1];
            if (zstr) ctr[2] = lammpsController->lammps()->input->variable->compute_equal(*zvar);
            else ctr[2] = *zvalue;
        } else {
            if (xstr) ctr[0] = lammpsController->lammps()->input->variable->compute_equal(*xvar);
            else ctr[0] = *xvalue;
            if (ystr) ctr[1] = lammpsController->lammps()->input->variable->compute_equal(*yvar);
            else ctr[1] = *yvalue;
            ctr[2] = lammpsController->lammps()->domain->boxlo[2];
        }
        lammpsController->lammps()->domain->remap(ctr);

        double radius;
        if (rstr) radius = lammpsController->lammps()->input->variable->compute_equal(*rvar);
        else radius = *rvalue;
        qDebug() << "Found cylinder along " << *cdim << " at (x,y,r)=(" << ctr[0] << ", " << ctr[1] << ", " << radius << ")";
    } else if (*istyle == PLANE) {
        // plane = current plane position

        double plane;
        if (pstr) plane = lammpsController->lammps()->input->variable->compute_equal(*pvar);
        else plane = *pvalue;
        qDebug() << "Found plane along " << *cdim << " at (p)=(" << plane;
    }
}
