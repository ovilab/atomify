#include "selectionmodifier.h"

#include "../system.h"
#include "../atomdata.h"
#include "../../atomselectiondata.h"

SelectionModifier::SelectionModifier()
{

}

void SelectionModifier::apply(AtomData &atomData)
{
    if(!enabled()) return;

    auto list = m_system->selection().value<QList<QObject*>>();
    if (list.size() == 0)
        return;

    bool anyHoveredOrMarked = false;
    for (QObject *object : list) {
        AtomSelectionData *selectionData = qobject_cast<AtomSelectionData*>(object);
        anyHoveredOrMarked = anyHoveredOrMarked || selectionData->hovered() || selectionData->marked();
    }

    if (!anyHoveredOrMarked)
        return;

    for(int i=0; i<atomData.size(); i++) {
        bool selected = false;
        bool hovered = false;
        bool marked = false;
        for (QObject *object : list) {
            AtomSelectionData *selectionData = qobject_cast<AtomSelectionData*>(object);
            if (selectionData->id() == atomData.lammpsParticleId.at(i)) {
                selected = true;
                hovered = selectionData->hovered();
                marked = selectionData->marked();
            }
        }

        if (!selected) {
            // Not selected particles should be very dark
            atomData.colors[i][0] *= 0.2;
            atomData.colors[i][1] *= 0.2;
            atomData.colors[i][2] *= 0.2;
        } else if (!hovered && !marked) {
            // Selected, but not hovered should be darker, but still brighter than
            // not selected ones
            atomData.colors[i][0] *= 0.4;
            atomData.colors[i][1] *= 0.4;
            atomData.colors[i][2] *= 0.4;
        }
    }
}
