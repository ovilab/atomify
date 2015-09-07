#ifndef SIMULATION_H
#define SIMULATION_H
#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QMap>

#include <functional>
#include <sstream>
#include <string>

#include "mpi_stubs/mpi.h"
#include "lammps/lammps.h"
#include "lammps/library.h"
#include "lammps/atom.h"
using namespace LAMMPS_NS;

class Simulation
{
protected:
    void initialize(QString inputScriptFile, QVector3D initialCameraPosition, QVector3D positionOffset = QVector3D(0,0,0));
    virtual void setScaleAndColorEvaluator();

    std::function<void (QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes)> m_scaleAndColorEvaluator;
    bool m_isInitialized = false;
    QVector3D m_positionOffset;
    QVector3D m_initialCameraPosition;
    QString m_inputScriptFile;
public:
    Simulation();
    ~Simulation() {}
    std::function<void (QVector<QColor> &colors, QVector<float> &scales, QVector<int> &atomTypes)> scaleAndColorEvaluator();
    QString inputScriptFile();
    QVector3D positionOffset();
    QVector3D initialCameraPosition() const;
};

#endif // SIMULATION_H
