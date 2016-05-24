#ifndef ATOMS_H
#define ATOMS_H

#include <SimVis/SphereData>
#include <SimVis/BondData>

#include <QColor>
#include <QObject>
#include <QVector>
#include <QVariantList>
#include <mpi.h>
#include <lammps.h>
#include "atomdata.h"

struct AtomStyle {
    QColor color;
    float radius;
    AtomStyle(float scale, QColor color) {
        this->color = color;
        this->radius = scale;
    }
};

class Atoms : public QObject
{
    Q_OBJECT
    Q_PROPERTY(SphereData* sphereData READ sphereData NOTIFY sphereDataChanged)
    Q_PROPERTY(BondData* bondData READ bondData NOTIFY bondDataChanged)
    Q_PROPERTY(Bonds* bonds READ bonds NOTIFY bondsChanged)
    Q_PROPERTY(QVariantList modifiers READ modifiers WRITE setModifiers NOTIFY modifiersChanged)
    Q_PROPERTY(float bondRadius READ bondRadius WRITE setBondRadius NOTIFY bondRadiusChanged)
public:
    Atoms(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    void updateData();
    SphereData* sphereData() const;
    QVariantList modifiers() const;
    QVector<AtomStyle*> &atomStyles();
    void setAtomType(int atomType, QString atomTypeName);
    void setAtomColorAndScale(int atomType, QColor color, float radius);
    BondData* bondData() const;
    class Bonds* bonds() const;
    AtomData &atomData();
    float bondRadius() const;

public slots:
    void setBondRadius(float bondRadius);
    void setModifiers(QVariantList modifiers);

signals:
    void sphereDataChanged(SphereData* sphereData);
    void bondDataChanged(BondData* bondData);
    void bondsChanged(class Bonds* bonds);
    void bondRadiusChanged(float bondRadius);
    void modifiersChanged(QVariantList modifiers);

private:
    AtomData m_atomData;
    QVector<BondVBOData> bondsDataRaw;
    QMap<QString, AtomStyle*> m_atomStyleTypes;
    QVector<AtomStyle*> m_atomStyles;
    SphereData* m_sphereData = nullptr;
    BondData* m_bondData = nullptr;
    class Bonds* m_bonds = nullptr;
    QVariantList m_modifiers;
    float m_bondRadius = 0.1;
    void generateBondData(AtomData &atomData);
    void generateSphereData(AtomData &atomData);
    void applyDeltaPositions(AtomData &atomData);
};

#endif // ATOMS_H
