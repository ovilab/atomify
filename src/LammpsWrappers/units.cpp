#include "units.h"
#include <update.h>
#include <domain.h>
#include <QDateTime>
using namespace LAMMPS_NS;
Units::Units(QObject *parent) : QObject(parent)
{
    setName("None");
}

Units::~Units()
{
    delete [] unit_style; // TODO consider using QByteArray instead of char*
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

    setDimensions(lammps->domain->dimension);

    if(doUpdate) {
        if(unit_style) {
            delete [] unit_style;
        }
        int n = strlen(lammps->update->unit_style) + 10;
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
            setVolume( QString("Å<sup>%1</sup>").arg(m_dimensions));
            setDensity(QString("g/cm<sup>%1</sup>").arg(m_dimensions));
            setLength("Å");
        } else if (strcmp(unit_style,"metal") == 0) {
            type = Metal;
            setName("Metal");
            setTime("ps");
            setVolume(QString("Å<sup>%1</sup>").arg(m_dimensions));
            setDensity(QString("g/cm<sup>%1</sup>").arg(m_dimensions));
            setLength("Å");
        } else if (strcmp(unit_style,"si") == 0) {
            type = SI;
            setName("SI");
            setTime("s");
            setVolume(QString("m<sup>%1</sup>").arg(m_dimensions));
            setDensity(QString("kg/m<sup>%1</sup>").arg(m_dimensions));
            setLength("m");
        } else if (strcmp(unit_style,"cgs") == 0) {
            type = CGS;
            setName("CGS");
            setLength("cm");
            setTime("s");
            setVolume(QString("cm<sup>%1</sup>").arg(m_dimensions));
            setDensity(QString("g/cm<sup>%1</sup>").arg(m_dimensions));
        } else if (strcmp(unit_style,"electron") == 0) {
            type = Electron;
            setName("Electron");
            setLength("Bohr");
            setTime("fs");
            setVolume(QString("Bohr<sup>%1</sup>").arg(m_dimensions));
            setDensity("");
        } else if (strcmp(unit_style,"micro") == 0) {
            type = Micro;
            setName("Micro");
            setTime("μs");
            setVolume(QString("μm<sup>%1</sup>").arg(m_dimensions));
            setDensity(QString("pg/μm<sup>%1</sup>").arg(m_dimensions));
            setLength("μm");
        } else if (strcmp(unit_style,"nano") == 0) {
            type = Nano;
            setName("Nano");
            setLength("nm");
            setTime("ns");
            setVolume(QString("nm<sup>%1</sup>").arg(m_dimensions));
            setDensity(QString("ag/nm<sup>%1</sup>").arg(m_dimensions));
        }
    }
}

void Units::reset()
{

    setName("None");
    setTime("");
    setVolume("");
    setDensity("");
    setLength("");

    if(unit_style) {
        delete [] unit_style;
    }

    unit_style = nullptr;
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

QString Units::density() const
{
    return m_density;
}

QString Units::length() const
{
    return m_length;
}

int Units::dimensions() const
{
    return m_dimensions;
}

QString Units::timeToFormattedString(int seconds, QString format)
{
    return QDateTime::fromTime_t(seconds).toUTC().toString(format);
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

void Units::setDensity(QString density)
{
    if (m_density == density)
        return;

    m_density = density;
    emit densityChanged(density);
}

void Units::setLength(QString length)
{
    if (m_length == length)
        return;

    m_length = length;
    emit lengthChanged(length);
}

void Units::setDimensions(int dimensions)
{
    if (m_dimensions == dimensions)
        return;

    m_dimensions = dimensions;
    emit dimensionsChanged(dimensions);
}
