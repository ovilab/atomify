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
public:
    Atoms(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    void updateData();
    SphereData* sphereData() const;
    QVector<class Modifier*> &modifiers();
    QVector<AtomStyle*> &atomStyles();
    void setAtomType(int atomType, QString atomTypeName);
    void setAtomColorAndScale(int atomType, QColor color, float radius);
    BondData* bondData() const;
    class Bonds* bonds() const;

signals:
    void sphereDataChanged(SphereData* sphereData);
    void bondDataChanged(BondData* bondData);
    void bondsChanged(class Bonds* bonds);

private:
    AtomData m_atomData;
    QMap<QString, AtomStyle*> m_atomStyleTypes;
    QVector<AtomStyle*> m_atomStyles;
    SphereData* m_sphereData = nullptr;
    BondData* m_bondData = nullptr;
    class Bonds* m_bonds;
    QVector<class Modifier*> m_modifiers;
    void generateBondData(AtomData &atomData);
    void generateSphereData(AtomData &atomData);
};

#endif // ATOMS_H
