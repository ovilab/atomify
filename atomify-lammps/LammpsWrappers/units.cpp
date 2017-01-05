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
            setTime("");
            setVolume("");
            setDensity("");
            setLength("");
        } else if (strcmp(unit_style,"real") == 0) {
            type = Real;
            setName("Real");
            setTime("fs");
            setVolume("Å<sup>3</sup>");
            setDensity("g/cm<sup>3</sup>");
            setLength("Å");
        } else if (strcmp(unit_style,"metal") == 0) {
            type = Metal;
            setName("Metal");
            setTime("ps");
            setVolume("Å<sup>3</sup>");
            setDensity("g/cm<sup>3</sup>");
            setLength("Å");
        } else if (strcmp(unit_style,"si") == 0) {
            type = SI;
            setName("SI");
            setTime("");
            setVolume("");
            setDensity("");
        } else if (strcmp(unit_style,"cgs") == 0) {
            type = CGS;
            setName("CGS");
            setTime("");
            setVolume("");
            setDensity("");
        } else if (strcmp(unit_style,"electron") == 0) {
            type = Electron;
            setName("Electron");
            setTime("");
            setVolume("");
            setDensity("");
        } else if (strcmp(unit_style,"micro") == 0) {
            type = Micro;
            setName("Micro");
            setTime("");
            setVolume("");
            setDensity("");
        } else if (strcmp(unit_style,"nano") == 0) {
            type = Nano;
            setName("Nano");
            setTime("");
            setVolume("");
            setDensity("");
        }
    }
}

QString Units::name() const
{
    return m_name;
}

QString Units::time() const
{
    return m_time;
}

QString Units::volume() const
{
    return m_volume;
}

void Units::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}

void Units::setTime(QString time)
{
    if (m_time == time)
        return;

    m_time = time;
    emit timeChanged(time);
}

void Units::setVolume(QString volume)
{
    if (m_volume == volume)
        return;

    m_volume = volume;
    emit volumeChanged(volume);
}
