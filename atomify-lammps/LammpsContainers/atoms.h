#ifndef ATOMS_H
#define ATOMS_H

#include <SimVis/SphereData>
#include <SimVis/CylinderData>
#include <QColor>
#include <QObject>
#include <QVector>
#include <QVariantList>
#include <mpi.h>
#include <lammps.h>

struct AtomStyle {
    int atomType;
    QColor color;
    float scale;
    AtomStyle(int atomType, QColor color, float scale) {
        this->atomType = atomType;
        this->color = color;
        this->scale = scale;
    }
};

struct AtomData {
    QVector<QVector3D> positions;
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<int> atomTypes;
};

class Atoms : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SphereData* sphereData READ sphereData WRITE setSphereData NOTIFY sphereDataChanged)
    Q_PROPERTY(CylinderData* cylinderData READ cylinderData WRITE setCylinderData NOTIFY cylinderDataChanged)
public:
    Atoms();
    void synchronize(class LAMMPS_NS::LAMMPS *lammps);
    void updateData();
    SphereData* sphereData() const;
    CylinderData* cylinderData() const;

public slots:
    void setSphereData(SphereData* sphereData);
    void setCylinderData(CylinderData* cylinderData);

signals:
    void sphereDataChanged(SphereData* sphereData);
    void cylinderDataChanged(CylinderData* cylinderData);

private:
    AtomData m_atomData;
    SphereData* m_sphereData = nullptr;
    CylinderData* m_cylinderData = nullptr;
    QVector<class Modifier*> m_modifiers;
    void generateCylinderData(AtomData &atomData);
    void generateSphereData(AtomData &atomData);
};

#endif // ATOMS_H
