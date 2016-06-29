#include "colormodifier.h"
#include "../system.h"
#include "../atoms.h"

ColorModifier::ColorModifier()
{

}


void ColorModifier::apply(AtomData &atomData)
{
    if(!enabled()) return;
//    qDebug() << "System: " << m_system;
//    qDebug() << "Atoms: " << m_system->atoms();
    QVector<AtomStyle*> &atomStyles = m_system->atoms()->atomStyles();
    for(int i=0; i<atomData.size(); i++) {
        // TODO: Validate all atomdata first
        int atomType = atomData.types[i];
        const QColor &color = atomStyles[atomType]->color;

        atomData.radii[i] = atomStyles[atomType]->radius*m_scale; // factor because it looks better
        // atomData.radii[i] = 0.15;
        atomData.colors[i] = QVector3D(color.redF(), color.greenF(), color.blueF());
    }
}

float ColorModifier::scale() const
{
    return m_scale;
}

void ColorModifier::setScale(float scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;
    emit scaleChanged(scale);
}
