#include "groups.h"
#include "../mysimulator.h"
#include <group.h>
using namespace LAMMPS_NS;

Groups::Groups(AtomifySimulator *simulator)
{
    Q_UNUSED(simulator)
}

void Groups::synchronize(LAMMPS *lammps)
{
    if(!lammps || !lammps->group) return;
    Group *group = lammps->group;
    int numGroups = group->ngroup;
    QList<QString> newGroups;

    for(int i=0; i<numGroups; i++) {
        QString groupName = QString::fromUtf8(group->names[i]);
        newGroups.push_back(groupName);
    }

    if(numGroups != m_groups.size()) {
        setGroups(newGroups);
    } else {
        for(const QString &groupName : newGroups) {
            if(!m_groups.contains(groupName)) {
                setGroups(newGroups);
                return;
            }
        }
    }
}

QList<QString> Groups::groups() const
{
    return m_groups;
}

void Groups::setGroups(QList<QString> groups)
{
    if (m_groups == groups)
        return;

    m_groups = groups;
    emit groupsChanged(groups);
}

