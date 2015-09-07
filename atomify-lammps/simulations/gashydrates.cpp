#include "gashydrates.h"

GasHydrates::GasHydrates()
{
    setScaleAndColorEvaluator();
    initialize(":/scripts/in.gashydrates", QVector3D(0, 0, 30));
}

void GasHydrates::setScaleAndColorEvaluator()
{
    m_scaleAndColorEvaluator = [](QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes) {
        int numberOfAtoms = colors.size();

        scales.resize(numberOfAtoms);
        QColor color1(255.0, 0.0, 0.0);
        QColor color2(255.0, 255.0, 255.0);
        QColor color3(110, 212, 219);
        for(unsigned int i=0; i<numberOfAtoms; i++) {
            if(atomTypes[i] == 1) {
                colors[i] = color1;
                scales[i] = 2.5;
            } else if(atomTypes[i] == 2) {
                colors[i] = color2;
                scales[i] = 1.0;
            } else if(atomTypes[i] == 3) {
                colors[i] = color3;
                scales[i] = 5.0;
            }
        }
    };
}
