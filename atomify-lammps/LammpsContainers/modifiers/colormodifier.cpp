#include "colormodifier.h"

ColorModifier::ColorModifier(Atoms *atoms, System *system) : Modifier(atoms, system)
{

}


void ColorModifier::apply(AtomData &atomData)
{
    QVector<AtomStyle*> &atomStyles = m_atoms->atomStyles();
    for(int i=0; i<atomData.positions.size(); i++) {
        // TODO: Validate all atomdata first
        int atomType = atomData.types[i];
        const QColor &color = atomStyles[atomType]->color;

        atomData.radii[i] = atomStyles[atomType]->scale;
        atomData.colors[i] = QVector3D(color.redF(), color.greenF(), color.blueF());
    }
}
