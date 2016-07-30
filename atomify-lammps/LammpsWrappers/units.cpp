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
            setName("LJ");
        } else if (strcmp(unit_style,"real") == 0) {
            type = Real;
            setName("Real");
        } else if (strcmp(unit_style,"metal") == 0) {
            type = Metal;
            setName("Metal");
        } else if (strcmp(unit_style,"si") == 0) {
            type = SI;
            setName("SI");
        } else if (strcmp(unit_style,"cgs") == 0) {
            type = CGS;
            setName("CGS");
        } else if (strcmp(unit_style,"electron") == 0) {
            type = Electron;
            setName("Electron");
        } else if (strcmp(unit_style,"micro") == 0) {
            type = Micro;
            setName("Micro");
        } else if (strcmp(unit_style,"nano") == 0) {
            type = Nano;
            setName("Nano");
        }
    }
}

QString Units::name() const
{
    return m_name;
}

void Units::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}
