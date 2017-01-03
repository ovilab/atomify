#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include <QVariant>
#include <QVector3D>
#include <mpi.h>
#include <lammps.h>
#include <qvector.h>
#include <domain.h>
#include <QMatrix3x3>
#include <qmatrix4x4.h>

class System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QVector3D origin READ origin NOTIFY originChanged)
    Q_PROPERTY(QVector3D cameraPosition READ cameraPosition WRITE setCameraPosition NOTIFY cameraPositionChanged)
    Q_PROPERTY(QVector3D center READ center NOTIFY centerChanged)
    Q_PROPERTY(QVariantList corners READ corners NOTIFY cornersChanged)
    Q_PROPERTY(QMatrix4x4 transformationMatrix READ transformationMatrix NOTIFY transformationMatrixChanged)
    Q_PROPERTY(int numberOfAtoms READ numberOfAtoms NOTIFY numberOfAtomsChanged)
    Q_PROPERTY(int numberOfAtomTypes READ numberOfAtomTypes NOTIFY numberOfAtomTypesChanged)
    Q_PROPERTY(float volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(float simulationTime READ simulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(int currentTimestep READ currentTimestep NOTIFY currentTimestepChanged)
    Q_PROPERTY(Atoms* atoms READ atoms WRITE setAtoms NOTIFY atomsChanged)
    Q_PROPERTY(Regions* regions READ regions WRITE setRegions NOTIFY regionsChanged)
    Q_PROPERTY(Groups* groups READ groups WRITE setGroups NOTIFY groupsChanged)
    Q_PROPERTY(Computes* computes READ computes WRITE setComputes NOTIFY computesChanged)
    Q_PROPERTY(Variables* variables READ variables WRITE setVariables NOTIFY variablesChanged)
    Q_PROPERTY(Fixes* fixes READ fixes WRITE setFixes NOTIFY fixesChanged)
    Q_PROPERTY(Units* units READ units WRITE setUnits NOTIFY unitsChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
public:
    System(class AtomifySimulator *simulator = nullptr);

    // Properties
    class Atoms* atoms() const;
    class Regions* regions() const;
    class Groups* groups() const;
    class Computes* computes() const;
    class Units* units() const;
    class Fixes* fixes() const;
    class Variables* variables() const;
    QVector3D origin() const;
    QVector3D size() const;
    int numberOfAtoms() const;
    float simulationTime() const;
    int currentTimestep() const;
    int numberOfAtomTypes() const;
    float volume() const;
    QVector3D cameraPosition() const;
    QVector3D center() const;

    // Actions
    void synchronize(class LAMMPSController *lammpsController);
    void synchronizeQML(class LAMMPSController *lammpsController);
    void updateThreadOnDataObjects(QThread *thread);
    void reset();
    bool isValid() const;

    QVariantList corners() const
    {
        QVariantList list;
        for(const auto& corner: m_corners) {
            list.append(corner);
        }
        return list;
    }

    QMatrix4x4 transformationMatrix() const
    {
        return m_transformationMatrix;
    }

public slots:
    void setAtoms(class Atoms* atoms);
    void setRegions(class Regions* regions);
    void setGroups(class Groups* groups);
    void setIsValid(bool isValid);
    void setComputes(class Computes* computes);
    void setVariables(class Variables* variables);
    void setCameraPosition(QVector3D cameraPosition);
    void setUnits(class Units* units);
    void setFixes(class Fixes* fixes);

signals:
    void originChanged(QVector3D origin);
    void sizeChanged(QVector3D size);
    void numberOfAtomsChanged(int numberOfAtoms);
    void simulationTimeChanged(float simulationTime);
    void currentTimestepChanged(int currentTimestep);
    void atomsChanged(class Atoms* atoms);
    void regionsChanged(class Regions* regions);
    void groupsChanged(class Groups* groups);
    void unitsChanged(class Units* units);
    void fixesChanged(class Fixes* fixes);
    void computesChanged(class Computes* computes);
    void variablesChanged(class Variables* variables);
    void numberOfAtomTypesChanged(int numberOfAtomTypes);
    void volumeChanged(float volume);
    void isValidChanged(bool isValid);
    void cameraPositionChanged(QVector3D cameraPosition);
    void centerChanged(QVector3D center);
    void cornersChanged();

    void transformationMatrixChanged(QMatrix4x4 transformationMatrix);

private:
    class Atoms* m_atoms = nullptr;
    class Regions* m_regions = nullptr;
    class Groups* m_groups = nullptr;
    class Computes* m_computes = nullptr;
    class Units* m_units = nullptr;
    class Fixes* m_fixes = nullptr;
    class Variables* m_variables = nullptr;
    QVector3D m_origin;
    QVector3D m_size;
    QVector3D m_cameraPosition;
    int m_numberOfAtoms = 0;
    float m_simulationTime = 0;
    int m_currentTimestep = 0;
    int m_numberOfAtomTypes = 0;
    float m_volume = 0;
    bool m_isValid = false;
    QVector<QVector3D> m_corners;
    void updateCorners(LAMMPS_NS::Domain *domain);
    void updateSizeAndOrigin(LAMMPS_NS::Domain *domain);
    QMatrix4x4 m_transformationMatrix;
};

#endif // SYSTEM_H
