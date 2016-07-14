#ifndef ATOMS_H
#define ATOMS_H
#include <QColor>
#include <QObject>
#include <QVector>
#include <QVariantList>
#include <mpi.h>
#include <lammps.h>
#include "atomdata.h"
#include <SimVis/Bonds>
#include <SimVis/Spheres>
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
    Q_PROPERTY(CPBonds* bonds READ bonds NOTIFY bondsChanged)
    Q_PROPERTY(QVariantList modifiers READ modifiers WRITE setModifiers NOTIFY modifiersChanged)
    Q_PROPERTY(float bondRadius READ bondRadius WRITE setBondRadius NOTIFY bondRadiusChanged)
public:
    Atoms(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    void updateData(class System *system, LAMMPS_NS::LAMMPS *lammps);
    QVariantList modifiers() const;
    QVector<AtomStyle*> &atomStyles();
    void setAtomType(int atomType, QString atomTypeName);
    void setAtomColorAndScale(int atomType, QColor color, float radius);
    class CPBonds* bonds() const;
    AtomData &atomData();
    float bondRadius() const;
    void reset();
    QVector<SphereData> &spheresData();
    QVector<BondData> &bondsData();

public slots:
    void setBondRadius(float bondRadius);
    void setModifiers(QVariantList modifiers);

signals:
    void bondsChanged(class CPBonds* bonds);
    void bondRadiusChanged(float bondRadius);
    void modifiersChanged(QVariantList modifiers);

private:
    AtomData m_atomData;
    QVector<BondsVBOData> bondsDataRaw;
    QMap<QString, AtomStyle*> m_atomStyleTypes;
    QVector<AtomStyle*> m_atomStyles;
    QVector<SphereData> m_spheresData;
    QVector<BondData> m_bondsData;
    class CPBonds* m_bonds = nullptr;
    QVariantList m_modifiers;
    float m_bondRadius = 0.1;
    void generateBondData(AtomData &atomData);
    void generateBondDataFromLammpsNeighborlist(AtomData &atomData, LAMMPS_NS::LAMMPS &lammps);
    void generateSphereData(AtomData &atomData);
    void applyDeltaPositions(AtomData &atomData);
};

#endif // ATOMS_H
