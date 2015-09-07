#include "lennardjonescrystal.h"

LennardJonesCrystal::LennardJonesCrystal()
{
    setScaleAndColorEvaluator();
    initialize(":/scripts/in.lennardjonescrystal", QVector3D(0, 0, 25));
}

void LennardJonesCrystal::setScaleAndColorEvaluator()
{
    m_scaleAndColorEvaluator = [](QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes) {
        for(unsigned int i=0; i<colors.size(); i++) {
            colors[i] = QColor(252, 255, 0);
        }
    };
}
