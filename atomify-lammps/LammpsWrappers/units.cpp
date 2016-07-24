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
    }
}
