#include "colormodifier.h"
#include "../system.h"
#include "../atoms.h"
#include "../groups.h"
ColorModifier::ColorModifier()
{

}

void ColorModifier::apply(AtomData &atomData)
{
    if(!enabled()) return;
    QVector<AtomStyle*> &atomStyles = m_system->atoms()->atomStyles();
    for(int i=0; i<atomData.size(); i++) {
        // TODO: Validate all atomdata first
        int atomType = atomData.types[i];
        const QColor &color = atomStyles[atomType]->color;
        if(!atomData.radiiFromLAMMPS) {
            atomData.radii[i] = atomStyles[atomType]->radius*0.2;
        }
        atomData.colors[i] = QVector3D(color.redF(), color.greenF(), color.blueF());
    }
}
