#include "bulkwater.h"
#include <iostream>
#include <QString>
#include <QDebug>
using namespace std;


BulkWater::BulkWater()
{
    setScaleAndColorEvaluator();
    initialize(":/scripts/in.bulkwater", QVector3D(0, 0, 30));
}

void BulkWater::setScaleAndColorEvaluator()
{
    m_scaleAndColorEvaluator = [](QVector<QColor> &colors, QVector<float> &scales, LAMMPS *lammps) {
        scales.resize(lammps->atom->natoms);
        QColor color1(255.0, 0.0, 0.0);
        QColor color2(255.0, 255.0, 255.0);

        for(unsigned int i=0; i<lammps->atom->natoms; i++) {
            if(lammps->atom->type[i] == 1) {
                colors[i] = color1;
                scales[i] = 2.5;
            } else if(lammps->atom->type[i] == 2) {
                colors[i] = color2;
                scales[i] = 1.0;
            }
        }
    };
}
