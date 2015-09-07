#include "lennardjonesdiffusion.h"

LennardJonesDiffusion::LennardJonesDiffusion()
{
    setScaleAndColorEvaluator();
    initialize(":/scripts/in.lennardjonesdiffusion", QVector3D(0, 0, 25));
    // initialize("/repos/lammps/src/usc/silica/in2.usc", QVector3D(0, 0, 25));
    // initialize("/repos/lammps/src/usc/water/water.in", QVector3D(0, 0, 25));
    // initialize("/repos/lammps/src/usc/silicawater/SiOWater.in", QVector3D(0, 0, 25));
    // initialize("/repos/lammps/src/usc/silica/in3.usc", QVector3D(0, 0, 25));
    // initialize("/repos/lammps/src/usc/silicawater/SiOWater.in", QVector3D(0, 0, 0));
    // initialize("/projects/PhD/2015/lammpsUSCPotential/silicawater/silicaWater.in2", QVector3D(0, 0, 0));
}


void LennardJonesDiffusion::setScaleAndColorEvaluator()
{
    m_scaleAndColorEvaluator = [](QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes) {
        int numberOfAtoms = colors.size();
        scales.resize(numberOfAtoms);
        QColor color1(255.0, 0.0, 0.0);
        QColor color2(255.0, 255.0, 255.0);
        QColor color3(218, 221, 0);

        for(unsigned int i=0; i<numberOfAtoms; i++) {
            if(atomTypes[i] == 2) {
                colors[i] = color1;
                scales[i] = 2.4;
            } else if(atomTypes[i] == 3) {
                colors[i] = color2;
                scales[i] = 1.0;
            } else if(atomTypes[i] == 1) {
                colors[i] = color3;
                scales[i] = 1.0;
            }
        }
    };
}
