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
    QColor color;
    float scale;
    AtomStyle(float scale, QColor color) {
        this->color = color;
        this->scale = scale;
    }
};

struct AtomData {
    QVector<QVector3D> positions;
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<int> types;
    bool isValid();
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
    QVector<class Modifier*> &modifiers();
    QVector<AtomStyle*> &atomStyles();
    void setAtomType(int atomType, QString atomTypeName);

public slots:
    void setSphereData(SphereData* sphereData);
    void setCylinderData(CylinderData* cylinderData);

signals:
    void sphereDataChanged(SphereData* sphereData);
    void cylinderDataChanged(CylinderData* cylinderData);

private:
    AtomData m_atomData;
    QMap<QString, AtomStyle*> m_atomStyleTypes;
    QVector<AtomStyle*> m_atomStyles;
    SphereData* m_sphereData = nullptr;
    CylinderData* m_cylinderData = nullptr;
    QVector<class Modifier*> m_modifiers;
    void generateCylinderData(AtomData &atomData);
    void generateSphereData(AtomData &atomData);
};

#endif // ATOMS_H
