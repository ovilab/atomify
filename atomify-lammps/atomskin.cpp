#include "atomskin.h"

AtomSkin::AtomSkin(QObject *parent) : QObject(parent)
{
    m_colorsVector << QColor(255,0,0) << QColor(255,255,0) << QColor(255,255,255) << QColor(0,255,255)  << QColor(0,255,0) << QColor(0,0,255);
}

AtomSkin::~AtomSkin()
{
    m_colors.clear();
    m_scales.clear();
}

void AtomSkin::setColorsAndScales(QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes)
{
    int numberOfAtoms = colors.size();

    // If we have any scales, we need to provide scales for all atoms
    if(m_scales.size() > 0) scales.resize(numberOfAtoms);
    QColor defaultColor = QColor(255.0, 0.0, 0.0);
    double defaultScale = 1.0;

    for(unsigned int i=0; i<numberOfAtoms; i++) {
        int atomType = atomTypes[i];

        if(atomType > m_colorsVector.size()) colors[i] = defaultColor;
        else colors[i] = m_colorsVector[atomType-1]; // LAMMPS atom types start at 1

        if(m_scalesVector.size()) {
            if(atomType > m_scalesVector.size()) scales[i] = defaultScale;
            else scales[i] = m_scalesVector[atomType-1]; // LAMMPS atom types start at 1
        }
    }
}

