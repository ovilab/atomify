#include "lennardjonescrystal.h"

LennardJonesCrystal::LennardJonesCrystal()
{
    setScaleAndColorEvaluator();
    initialize(":/scripts/in.lennardjonescrystal", QVector3D(0, 0, 25));
}

void LennardJonesCrystal::setScaleAndColorEvaluator()
{
    m_scaleAndColorEvaluator = [](QVector<QColor> &colors, QVector<float> &scales, LAMMPS *lammps) {
        for(unsigned int i=0; i<lammps->atom->natoms; i++) {
            colors[i] = QColor(110, 212, 219);
        }
    };
}
