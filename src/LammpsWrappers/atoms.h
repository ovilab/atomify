#ifndef ATOMS_H
#define ATOMS_H

#include <SimVis/BondData>
#include <SimVis/SphereData>

#include "atomdata.h"
#include <QColor>
#include <QObject>
#include <QVariantList>
#include <QVector>
#include <lammps.h>
#include <mpi.h>

struct AtomStyle {
    QColor color;
    float radius;
    QString shortName;
    QString fullName;
    AtomStyle(QString shortName, QString fullName, float radius, QColor color)
    {
        this->color = color;
        this->radius = radius;
        this->shortName = shortName;
        this->fullName = fullName;
    }
};

class Atoms : public QObject {
    Q_OBJECT
    Q_PROPERTY(SphereData* sphereData READ sphereData NOTIFY sphereDataChanged)
    Q_PROPERTY(BondData* bondData READ bondData NOTIFY bondDataChanged)
    Q_PROPERTY(Bonds* bonds READ bonds NOTIFY bondsChanged)
    Q_PROPERTY(QVariantList modifiers READ modifiers WRITE setModifiers NOTIFY modifiersChanged)
    Q_PROPERTY(bool sort READ sort WRITE setSort NOTIFY sortChanged)
    Q_PROPERTY(float bondScale READ bondScale WRITE setBondScale NOTIFY bondScaleChanged)
    Q_PROPERTY(float sphereScale READ sphereScale WRITE setSphereScale NOTIFY sphereScaleChanged)
    Q_PROPERTY(QString renderingMode READ renderingMode WRITE setRenderingMode NOTIFY renderingModeChanged)
    Q_PROPERTY(int numberOfBonds READ numberOfBonds WRITE setNumberOfBonds NOTIFY numberOfBondsChanged)
    Q_PROPERTY(float globalScale READ globalScale WRITE setGlobalScale NOTIFY globalScaleChanged)
    Q_PROPERTY(QVector<int> selectedParticles READ selectedParticles WRITE setSelectedParticles NOTIFY selectedParticlesChanged)

public:
    enum AtomFlag {
        NoFlags = 0,
        Selected = 1 << 0
    };
    Q_DECLARE_FLAGS(AtomFlags, AtomFlag)

    Atoms(class AtomifySimulator* simulator = nullptr);
    void synchronize(class LAMMPSController* lammpsController);
    void processModifiers(class System* system);
    SphereData* sphereData() const;
    QVariantList modifiers() const;
    QVector<AtomStyle*>& atomStyles();
    void setAtomType(int atomType, QString atomTypeName);
    void setAtomColorAndScale(int atomType, QColor color, float size);
    void setAtomColor(int atomType, QColor color);
    BondData* bondData() const;
    class Bonds* bonds() const;
    AtomData& atomData();
    void reset();
    bool sort() const;
    void synchronizeRenderer();
    void createRenderererData(LAMMPSController* lammpsController);
    float bondScale() const;
    float sphereScale() const;
    QString renderingMode() const;
    bool dirtyData() const;
    void setDirtyData(bool dirtyData);
    int numberOfBonds() const;
    void setAtomSize(int atomType, float radius);
    long memoryUsage();
    float globalScale() const;
    QVector<int> selectedParticles() const;

public slots:
    void setModifiers(QVariantList modifiers);
    void setSort(bool sort);
    void setBondScale(float bondScale);
    void setSphereScale(float sphereScale);
    void setRenderingMode(QString renderingMode);
    void setNumberOfBonds(int numberOfBonds);
    void setGlobalScale(float globalScale);
    void setSelectedParticles(QVector<int> particleIds);

signals:
    void sphereDataChanged(SphereData* sphereData);
    void bondDataChanged(BondData* bondData);
    void bondsChanged(class Bonds* bonds);
    void modifiersChanged(QVariantList modifiers);
    void sortChanged(bool sort);
    void bondScaleChanged(float bondScale);
    void sphereScaleChanged(float sphereScale);
    void renderingModeChanged(QString renderingMode);
    void numberOfBondsChanged(int numberOfBonds);
    void globalScaleChanged(float globalScale);
    void selectedParticlesChanged(QVector<int> selectedParticles);

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
    bool m_sort = false;
    bool m_dirtyData = false;
    float m_bondScale = 1.0;
    float m_sphereScale = 2.0;
    QString m_renderingMode = "Ball and stick";
    int m_numberOfBonds = 0;
    float m_globalScale = 1.0;
    void readAtomTypesFromFile();
    void generateBondData(AtomData& atomData, LAMMPSController* controller);
    void generateBondDataFromLammpsNeighborlist(AtomData& atomData, LAMMPSController* controller);
    bool generateBondDataFromNeighborList(AtomData& atomData, class LAMMPSController* controller);
    bool generateBondDataFromBondList(AtomData& atomData, LAMMPSController* controller);
    void generateSphereData(AtomData& atomData);
    bool doWeHavefullNeighborList(class LAMMPS_NS::Neighbor* neighbor);
    QVector<int> m_selectedParticles;
};

#endif // ATOMS_H
