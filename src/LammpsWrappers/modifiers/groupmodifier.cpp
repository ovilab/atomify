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
    for(int i=0; i<atomData.size(); i++) {
        for(CPGroup* group : groups) {
            bool isMemberOfGroup = atomData.bitmask[i] & group->bitmask(); // Each group in LAMMPS is represented as a bit in an int. Bitwise or to check membership.
            if(isMemberOfGroup) {
                if(group->hovered()) atomData.colors[i] = QVector3D(1.0, 0.0, 0.0);
                if(!group->visible()) atomData.visible[i] = false;
            } else {
                if(group->hovered()) {
                    // Remove atoms if alt also is pressed
                    if(modifiers &= Qt::AltModifier) {
                         atomData.visible[i] = false;
                    } else {
                        atomData.colors[i] = QVector3D(1.0, 1.0, 1.0);
                    }
                }
            }
        }
    }
}
