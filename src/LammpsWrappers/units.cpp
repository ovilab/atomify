#include "units.h"
#include <update.h>

using namespace LAMMPS_NS;
Units::Units(QObject *parent) : QObject(parent)
{

}

void Units::synchronize(LAMMPS *lammps)
{
    bool doUpdate = false;
    if(!unit_style) {
        doUpdate = true;
    }

    if(unit_style && strcmp(unit_style, lammps->update->unit_style)!=0) {
        doUpdate = true;
    }

    if(doUpdate) {
        if(unit_style) {
            delete [] unit_style;
        }
        int n = strlen(lammps->update->unit_style) + 1;
        unit_style = new char[n];
        strcpy(unit_style,lammps->update->unit_style);

        // enum Type { LJ, Real, Metal, SI, CGS, Electron, Micro, Nano };
        if (strcmp(unit_style,"lj") == 0) {
            type = LJ;
        } else if (strcmp(unit_style,"real") == 0) {
            type = Real;
        } else if (strcmp(unit_style,"metal") == 0) {
            type = Metal;
        } else if (strcmp(unit_style,"si") == 0) {
            type = SI;
        } else if (strcmp(unit_style,"cgs") == 0) {
            type = CGS;
        } else if (strcmp(unit_style,"electron") == 0) {
            type = Electron;
        } else if (strcmp(unit_style,"micro") == 0) {
            type = Micro;
        } else if (strcmp(unit_style,"nano") == 0) {
            type = Nano;
        }
    }
}
