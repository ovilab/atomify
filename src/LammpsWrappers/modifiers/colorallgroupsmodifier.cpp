#include "colorallgroupsmodifier.h"
#include "../groups.h"
#include "../system.h"
#include "../atomdata.h"

ColorAllGroupsModifier::ColorAllGroupsModifier()
{

}

void ColorAllGroupsModifier::apply(AtomData &atomData)
{
    if(!enabled()) return;

    QList<CPGroup*> groups = m_system->groups()->groups();
    for(int i=0; i<atomData.size(); i++) {
        for(int j=0; j<groups.size(); j++) {
            CPGroup *group = groups[j];

            bool isMemberOfGroup = atomData.bitmask[i] & group->bitmask(); // Each group in LAMMPS is represented as a bit in an int. Bitwise or to check membership.
            if(isMemberOfGroup) {
                atomData.colors[i] = m_colors.at(j);
            }
        }
    }
}
