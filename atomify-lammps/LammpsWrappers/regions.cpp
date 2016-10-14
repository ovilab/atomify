#include "regions.h"
#include "../mysimulator.h"
#include "../lammpscontroller.h"
#include <domain.h>
#include <region.h>
#include <group.h>
#include <atom.h>
#include <update.h>
#include <error.h>
Regions::Regions(AtomifySimulator *simulator)
{
    Q_UNUSED(simulator)

}

void Regions::add(QString identifier) {
    if(m_dataMap.contains(identifier)) return;
    CPRegion *region = new CPRegion(this);
    region->setIdentifier(identifier);
    m_data.push_back(region);
    m_dataMap.insert(identifier, region);
}

void Regions::remove(QString identifier) {
    if(!m_dataMap.contains(identifier)) return;

    CPRegion *region = static_cast<CPRegion*>(m_dataMap[identifier]);
    m_data.removeOne(region);
    m_dataMap.remove(identifier);
    delete region;
}

void Regions::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    if(!lammps) {
        reset();
        return;
    }
    Domain *lammpsDomain = lammps->domain;
    Region **regions = lammpsDomain->regions;

    int numRegions = lammpsDomain->nregion;
    setCount(numRegions);

    Group *lammpsGroup = lammps->group;
    /* TODO: NOT SURE IF THIS IS IMPORTANT */
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

    /* END: NOT SURE IF THIS IS IMPORTANT */
    bool anyChanges = false;
    for(int regionIndex=0; regionIndex<numRegions; regionIndex++) {
        Region *lammpsRegion = regions[regionIndex];
        QString identifier = QString::fromUtf8(lammpsRegion->id);
        if(!m_dataMap.contains(identifier)) {
            anyChanges = true;
            add(identifier);
        }
    }

    QList<QString> regionsToBeRemoved;
    for(QObject *obj : m_data) {
        CPRegion *region = static_cast<CPRegion*>(obj);
        if(!lammpsController->regionExists(region->identifier())) {
            anyChanges = true;
            regionsToBeRemoved.append(region->identifier());
        }
    }

    for(QString identifier : regionsToBeRemoved) {
        remove(identifier);
    }

    for(QObject *obj : m_data) {
        CPRegion *region = static_cast<CPRegion*>(obj);
        region->update(lammpsController->lammps());
    }
    if(anyChanges) {
        setModel(QVariant::fromValue(m_data));
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

bool Regions::active() const
{
    return m_active;
}

void Regions::reset()
{
    for(QObject *obj : m_data) {
        CPRegion *region = static_cast<CPRegion*>(obj);
        delete region;
    }
    m_data.clear();
    m_dataMap.clear();
    setModel(QVariant::fromValue(m_data));
}

QList<CPRegion *> Regions::regions()
{
    QList<CPRegion*> regions;
    for(QObject *obj : m_data) {
        CPRegion *region = qobject_cast<CPRegion*>(obj);
        if(region) regions.append(region);
    }

    return regions;
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

void Regions::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    emit activeChanged(active);
}

CPRegion::CPRegion(QObject *parent) : QObject(parent)
{

}

int CPRegion::count() const
{
    return m_count;
}

QString CPRegion::identifier() const
{
    return m_identifier;
}

bool CPRegion::visible() const
{
    return m_visible;
}

bool CPRegion::hovered() const
{
    return m_hovered;
}

void CPRegion::update(LAMMPS *lammps)
{
    QByteArray identifierBytes = m_identifier.toUtf8();
    int index = lammps->domain->find_region(identifierBytes.data());
    if(index < 0) return; // Should really not happen, but crash is bad :p

    Region *region = lammps->domain->regions[index];
    lammps->update->whichflag = 1; // HACK. This tells lammps we're doing dynamics so we can compute values in there.
    try {
        setCount(lammps->group->count(0,index));
        if(hovered() || !visible()) {
            m_containsAtom.resize(lammps->atom->natoms);
            for(int atomIndex=0; atomIndex<lammps->atom->natoms; atomIndex++) {
                double r[3];
                r[0] = lammps->atom->x[atomIndex][0];
                r[1] = lammps->atom->x[atomIndex][1];
                r[2] = lammps->atom->x[atomIndex][2];
                lammps->domain->remap(r);

                bool isInsideRegion = !region->inside(r[0], r[1], r[2])^region->interior;
                m_containsAtom[atomIndex] = isInsideRegion;
            }
            lammps->update->whichflag = 0;
        }
    } catch( LAMMPS_NS::LAMMPSException(&e) ) {
        // TODO: use this to report to user that something was wrong

    }
}

bool CPRegion::containsAtom(int atomIndex)
{
    if(m_containsAtom.size() <= atomIndex) return false; // We have just hovered, but list isn't updated yet. Assume not in region so we don't visualize wrong
    else return m_containsAtom.at(atomIndex);
}

void CPRegion::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}

void CPRegion::setIdentifier(QString identifier)
{
    if (m_identifier == identifier)
        return;

    m_identifier = identifier;
    emit identifierChanged(identifier);
}

void CPRegion::setVisible(bool visible)
{
    if (m_visible == visible)
        return;

    m_visible = visible;
    emit visibleChanged(visible);
}

void CPRegion::setHovered(bool hovered)
{
    if (m_hovered == hovered)
        return;

    m_hovered = hovered;
    emit hoveredChanged(hovered);
}
