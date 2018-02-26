#include <QApplication>
#include "groupmodifier.h"
#include "LammpsWrappers/groups.h"
#include "../system.h"
#include "../atomdata.h"
GroupModifier::GroupModifier()
{

}


void GroupModifier::apply(AtomData &atomData)
{
    if(!enabled()) return;
    auto modifiers = QApplication::queryKeyboardModifiers();
    QList<CPGroup*> groups = m_system->groups()->groups();
    for (int atomIndex = 0; atomIndex < atomData.size(); atomIndex++) {
        for (int groupIndex = 0; groupIndex < groups.size(); groupIndex++) {
            CPGroup* group = groups.at(groupIndex);

            bool isMemberOfGroup = atomData.bitmask[atomIndex] & group->bitmask(); // Each group in LAMMPS is represented as a bit in an int. Bitwise or to check membership.
            if(isMemberOfGroup) {
                if(group->marked() || group->hovered()) atomData.colors[atomIndex] = m_colors.at(groupIndex);
                if(!group->visible()) atomData.visible[atomIndex] = false;
            } else {
                if(group->hovered()) {
                    // Remove atoms if alt also is pressed
                    if(modifiers &= Qt::AltModifier) {
                         atomData.visible[atomIndex] = false;
                    } else {
                        atomData.colors[atomIndex] = QVector3D(1.0, 1.0, 1.0);
                    }
                }
            }
        }
    }
}
