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

struct BondsStyle {
    QVector<QVector<float>> bondLengths;

    BondsStyle() {
        int numTypes = 20;
        bondLengths.resize(numTypes);
        for(QVector<float> &vec : bondLengths) {
            vec.resize(numTypes);
            for(float &length : vec) {
                length = 0.0;
            }
        }

        // TODO: Remove and fix this
        bondLengths[1][2] = 2.6;
        bondLengths[2][1] = 2.6;
    }
};

struct Neighborlist {
    QVector<int> numNeighbors;
    QVector<QVector<int>> neighbors;
};

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
public:
    Atoms(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    void updateData();
    SphereData* sphereData() const;
    QVector<class Modifier*> &modifiers();
    QVector<AtomStyle*> &atomStyles();
    void setAtomType(int atomType, QString atomTypeName);
    BondData* bondData() const;

public slots:

signals:
    void sphereDataChanged(SphereData* sphereData);
    void bondDataChanged(BondData* bondData);

private:
    AtomData m_atomData;
    QMap<QString, AtomStyle*> m_atomStyleTypes;
    QVector<AtomStyle*> m_atomStyles;
    void copyNeighborlist(LAMMPS_NS::LAMMPS *lammps);
    SphereData* m_sphereData = nullptr;
    BondData* m_bondData = nullptr;
    BondsStyle bondsStyle;
    Neighborlist neighborlist;
    QVector<class Modifier*> m_modifiers;
    void generateBondData(AtomData &atomData);
    void generateSphereData(AtomData &atomData);
};

#endif // ATOMS_H
