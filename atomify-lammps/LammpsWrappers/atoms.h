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
    Q_PROPERTY(bool sort READ sort WRITE setSort NOTIFY sortChanged)
    Q_PROPERTY(float bondScale READ bondScale WRITE setBondScale NOTIFY bondScaleChanged)
    Q_PROPERTY(float sphereScale READ sphereScale WRITE setSphereScale NOTIFY sphereScaleChanged)
public:
    Atoms(class AtomifySimulator *simulator = nullptr);
    void synchronize(class LAMMPSController *lammpsController);
    void processModifiers(class System *system);
    SphereData* sphereData() const;
    QVariantList modifiers() const;
    QVector<AtomStyle*> &atomStyles();
    void setAtomType(int atomType, QString atomTypeName);
    void setAtomColorAndScale(int atomType, QColor color, float size);
    void setAtomColor(int atomType, QColor color);
    BondData* bondData() const;
    class Bonds* bonds() const;
    AtomData &atomData();
    void reset();
    bool sort() const;
    void synchronizeRenderer();
    void createRenderererData();
    float bondScale() const;
    float sphereScale() const;

public slots:
    void setModifiers(QVariantList modifiers);
    void setSort(bool sort);
    void setBondScale(float bondScale);
    void setSphereScale(float sphereScale);

signals:
    void sphereDataChanged(SphereData* sphereData);
    void bondDataChanged(BondData* bondData);
    void bondsChanged(class Bonds* bonds);
    void modifiersChanged(QVariantList modifiers);
    void sortChanged(bool sort);
    void bondScaleChanged(float bondScale);
    void sphereScaleChanged(float sphereScale);

private:
    AtomData m_atomData;
    AtomData m_atomDataProcessed;
    QVector<BondVBOData> bondsDataRaw;
    QByteArray m_bondsDataRaw;
    QByteArray m_sphereDataRaw;
    QMap<QString, AtomStyle*> m_atomStyleTypes;
    QVector<AtomStyle*> m_atomStyles;
    SphereData* m_sphereData = nullptr;
    BondData* m_bondData = nullptr;
    class Bonds* m_bonds = nullptr;
    QVariantList m_modifiers;
    void generateBondData(AtomData &atomData);
    void generateBondDataFromLammpsNeighborlist(AtomData &atomData, LAMMPS_NS::LAMMPS &lammps);
    void generateSphereData(AtomData &atomData);
    bool m_sort = false;
    float m_bondScale = 1.0;
    float m_sphereScale = 1.0;
};

#endif // ATOMS_H
