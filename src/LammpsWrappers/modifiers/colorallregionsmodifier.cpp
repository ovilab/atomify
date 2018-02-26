#include "colorallregionsmodifier.h"
#include "../regions.h"
#include "../system.h"
#include "../atomdata.h"

ColorAllRegionsModifier::ColorAllRegionsModifier()
{

}

void ColorAllRegionsModifier::apply(AtomData &atomData)
{
    QList<CPRegion*> regions = m_system->regions()->regions();
    for(CPRegion *region : regions) {
        region->setDoUpdate(false);
    }

    if(!enabled()) return;

    for(CPRegion *region : regions) {
        region->setDoUpdate(true);
    }

    for(int i=0; i<atomData.size(); i++) {
        for(int j=0; j<regions.size(); j++) {
            CPRegion* region = regions[j];
            bool isInsideRegion = region->containsAtom(i); // Each group in LAMMPS is represented as a bit in an int. Bitwise or to check membership.
            if(isInsideRegion) {
                int colorIndex = j % m_colors.size(); // in case users have ridiculously many regions
                atomData.colors[i] = m_colors[colorIndex];
            }
        }
    }
}
