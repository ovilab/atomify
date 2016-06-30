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
//    qDebug() << "System: " << m_system;
//    qDebug() << "Atoms: " << m_system->atoms();
    bool groupHovered = false;
    int groupBit = 0;
    QList<CPGroup*> groups = m_system->groups()->groups();
    for(CPGroup* group : groups) {
        if(group->hovered()) {
            groupHovered = true;
            groupBit = group->bitmask();
            break;
        }
    }

    QVector<AtomStyle*> &atomStyles = m_system->atoms()->atomStyles();
    for(int i=0; i<atomData.size(); i++) {
        // TODO: Validate all atomdata first
        int atomType = atomData.types[i];
        const QColor &color = atomStyles[atomType]->color;
        atomData.radii[i] = atomStyles[atomType]->radius*m_scale; // factor because it looks better
        // atomData.radii[i] = 0.15;
        if(groupHovered) {
            bool isMemberOfGroup = atomData.bitmask[i] & groupBit;
            if(isMemberOfGroup) {
                atomData.colors[i] = QVector3D(1.0, 0.0, 0.0);
            } else {
                atomData.colors[i] = QVector3D(color.redF(), color.greenF(), color.blueF());
            }
        } else atomData.colors[i] = QVector3D(color.redF(), color.greenF(), color.blueF());
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
