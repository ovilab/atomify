#include "groups.h"
#include "../mysimulator.h"
#include <group.h>
using namespace LAMMPS_NS;

Groups::Groups(AtomifySimulator *simulator)
{
    Q_UNUSED(simulator)
}

void Groups::update(Group *group)
{
    for(QObject *obj : m_data) delete obj; // Clean up the old ones
    m_dataMap.clear();
    m_data.clear();

    if(group == nullptr) return;

    int numGroups = group->ngroup;
    for(int groupIndex=0; groupIndex<numGroups; groupIndex++) {
        QString name = QString::fromUtf8(group->names[groupIndex]);
        int count = group->count(groupIndex);

        CPGroup *newGroup = new CPGroup(this);
        newGroup->setName(name);
        newGroup->setCount(count);
        m_data.push_back(newGroup);
        m_dataMap.insert(name, newGroup);
    }
    setCount(m_data.size());
    setModel(QVariant::fromValue(m_data));
}

void Groups::synchronize(LAMMPSController *lammpsController)
{
    LAMMPS *lammps = lammpsController->lammps();
    if(!lammps || !lammps->group) {
        update(nullptr);
        return;
    }

    Group *lammpsGroup = lammps->group;
    int numGroups = lammpsGroup->ngroup;
    setCount(numGroups);

    if(!m_active) return;

    if(m_data.count() != numGroups) {
        update(lammpsGroup);
        return;
    }

    for(int groupIndex=0; groupIndex<numGroups; groupIndex++) {
        QString groupName = QString::fromUtf8(lammpsGroup->names[groupIndex]);
        int count = lammpsGroup->count(groupIndex);

        if(!m_dataMap.contains(groupName)) {
            update(lammpsGroup);
            return;
        }
        CPGroup *group = qobject_cast<CPGroup*>(m_dataMap[groupName]);
        if(group && group->count() != count) {
            update(lammpsGroup);
            return;
        }
    }
}

QVariant Groups::model() const
{
    return m_model;
}

int Groups::count() const
{
    return m_count;
}

bool Groups::active() const
{
    return m_active;
}

void Groups::setModel(QVariant model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}

void Groups::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}

void Groups::setActive(bool active)
{
    if (m_active == active)
        return;

    m_active = active;
    emit activeChanged(active);
}


CPGroup::CPGroup(QObject *parent) : QObject(parent) { }

QString CPGroup::name() const
{
    return m_name;
}

int CPGroup::count() const
{
    return m_count;
}

void CPGroup::setName(QString name)
{
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(name);
}

void CPGroup::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}
