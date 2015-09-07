#include "simulation.h"
#include "lammps/variable.h"
#include "lammps/input.h"
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
using std::function;

Simulation::Simulation()
{
    setScaleAndColorEvaluator();
}

void Simulation::setScaleAndColorEvaluator()
{
    m_scaleAndColorEvaluator = [](QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes) {
        QColor color1(252, 255, 0);
        QColor color2(0.0, 255.0, 6.0);
        QColor color3(255.0, 0.0, 234.0);
        QColor color4(130.0, 130.0, 130.0);
        QColor color5(255.0, 255.0, 0.0);

        int numberOfAtoms = colors.size();

        for(unsigned int i=0; i<numberOfAtoms; i++) {
            if(atomTypes[i] == 1) colors[i] = color1;
            else if(atomTypes[i] == 2) colors[i] = color2;
            else if(atomTypes[i] == 3) colors[i] = color3;
            else if(atomTypes[i] == 4) colors[i] = color4;
            else if(atomTypes[i] == 5) colors[i] = color5;
        }
    };
}

void Simulation::initialize(QString inputScriptFile, QVector3D initialCameraPosition, QVector3D deltaPosition)
{
    m_inputScriptFile = inputScriptFile;
    m_positionOffset = deltaPosition;
    m_initialCameraPosition = initialCameraPosition;
    m_isInitialized = true;
}

function<void (QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes)> Simulation::scaleAndColorEvaluator()
{
    return m_scaleAndColorEvaluator;
}

QString Simulation::inputScriptFile()
{
    return m_inputScriptFile;
}

QVector3D Simulation::positionOffset()
{
    return m_positionOffset;
}

QVector3D Simulation::initialCameraPosition() const
{
    return m_initialCameraPosition;
}
