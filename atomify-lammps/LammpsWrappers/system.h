#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include <QVector3D>
#include <mpi.h>
#include <lammps.h>

class System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QVector3D origin READ origin NOTIFY originChanged)
    Q_PROPERTY(int numberOfAtoms READ numberOfAtoms NOTIFY numberOfAtomsChanged)
    Q_PROPERTY(int numberOfAtomTypes READ numberOfAtomTypes NOTIFY numberOfAtomTypesChanged)
    Q_PROPERTY(float volume READ volume NOTIFY volumeChanged)
    Q_PROPERTY(float simulationTime READ simulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(int timesteps READ timesteps NOTIFY timestepsChanged)
    Q_PROPERTY(Atoms* atoms READ atoms WRITE setAtoms NOTIFY atomsChanged)
    Q_PROPERTY(Regions* regions READ regions WRITE setRegions NOTIFY regionsChanged)
    Q_PROPERTY(Groups* groups READ groups WRITE setGroups NOTIFY groupsChanged)
    Q_PROPERTY(Computes* computes READ computes WRITE setComputes NOTIFY computesChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
public:
    System(class AtomifySimulator *simulator = nullptr);
    void synchronize(class LAMMPSController *lammpsController);
    QVector3D origin() const;
    QVector3D size() const;
    int numberOfAtoms() const;
    float simulationTime() const;
    int timesteps() const;
    class Atoms* atoms() const;
    class Regions* regions() const;
    class Groups* groups() const;
    class Computes* computes() const;
    int numberOfAtomTypes() const;
    void reset();
    float volume() const;
    bool isValid() const;

public slots:
    void setAtoms(class Atoms* atoms);
    void setRegions(class Regions* regions);
    void setGroups(class Groups* groups);
    void setIsValid(bool isValid);
    void setComputes(class Computes* computes);

signals:
    void originChanged(QVector3D origin);
    void sizeChanged(QVector3D size);
    void numberOfAtomsChanged(int numberOfAtoms);
    void simulationTimeChanged(float simulationTime);
    void timestepsChanged(int timesteps);
    void atomsChanged(class Atoms* atoms);
    void regionsChanged(class Regions* regions);
    void groupsChanged(class Groups* groups);
    void numberOfAtomTypesChanged(int numberOfAtomTypes);
    void volumeChanged(float volume);
    void isValidChanged(bool isValid);
    void computesChanged(class Computes* computes);

private:
    class Atoms* m_atoms = nullptr;
    class Regions* m_regions = nullptr;
    class Groups* m_groups = nullptr;
    class Computes* m_computes = nullptr;
    QVector3D m_origin;
    QVector3D m_size;
    int m_numberOfAtoms = 0;
    float m_simulationTime = 0;
    int m_timesteps = 0;
    int m_numberOfAtomTypes = 0;
    float m_volume = 0;
    bool m_isValid = false;
};

#endif // SYSTEM_H
