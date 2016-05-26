#include "regions.h"
#include "../mysimulator.h"
#include <domain.h>
#include <region.h>
#include <group.h>

Regions::Regions(AtomifySimulator *simulator)
{
    Q_UNUSED(simulator)

}

void Regions::update(LAMMPS *lammps) {
    for(QObject *obj : m_data) delete obj; // Clean up the old ones
    m_dataMap.clear();
    m_data.clear();

    if(lammps == nullptr) return;

    Domain *lammpsDomain = lammps->domain;
    Region **regions = lammpsDomain->regions;
    int numRegions = lammpsDomain->nregion;

    for(int regionIndex=0; regionIndex<numRegions; regionIndex++) {
        Region *lammpsRegion = regions[regionIndex];
        int count = lammps->group->count(0,regionIndex);
        QString name = QString::fromUtf8(lammpsRegion->id);
        CPRegion *region = new CPRegion(this);
        region->setName(name);
        region->setCount(count);
        m_data.push_back(region);
        m_dataMap.insert(name, region);
    }

    setCount(m_data.size());
    setModel(QVariant::fromValue(m_data));
}

void Regions::synchronize(LAMMPS *lammps)
{
    if(!lammps) {
        update(nullptr);
        return;
    }
    Domain *lammpsDomain = lammps->domain;
    Region **regions = lammpsDomain->regions;

    int numRegions = lammpsDomain->nregion;
    Group *lammpsGroup = lammps->group;
    bool firstGroupIsAll = false;

    if(lammpsGroup->ngroup>0) {
        QString firstGroupName = QString::fromUtf8(lammpsGroup->names[0]);
        if(firstGroupName.compare("all") == 0) {
            firstGroupIsAll = true;
        }
    }

    if(!firstGroupIsAll) {
        qDebug() << "Error, group all doesn't exist?";
        return;
    }

    for(int regionIndex=0; regionIndex<numRegions; regionIndex++) {
        Region *lammpsRegion = regions[regionIndex];
        int count = lammpsGroup->count(0,regionIndex);
        QString name = QString::fromUtf8(lammpsRegion->id);
        qDebug() << "Found region with name: " << name << " and count: " << count;
        if(!m_dataMap.contains(name)) {
            update(lammps);
            return;
        }

        CPRegion *region = qobject_cast<CPRegion*>(m_dataMap[name]);
        if(region && region->count() != count) {
            update(lammps);
            return;
        }
    }
}

QVariant Regions::model() const
{
    return m_model;
}

int Regions::count() const
{
    return m_count;
}

void Regions::setModel(QVariant model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}

void Regions::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}

CPRegion::CPRegion(QObject *parent) : QObject(parent)
{

}

QString CPRegion::name() const
{
    return m_name;
}

int CPRegion::count() const
{
    return m_count;
}

void CPRegion::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}

void CPRegion::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}
