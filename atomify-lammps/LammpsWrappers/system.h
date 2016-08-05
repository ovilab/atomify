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
    Q_PROPERTY(long realTime READ realTime WRITE setRealTime NOTIFY realTimeChanged)
    Q_PROPERTY(float simulationTime READ simulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(int timesteps READ timesteps WRITE setTimesteps NOTIFY timestepsChanged)
    Q_PROPERTY(Atoms* atoms READ atoms WRITE setAtoms NOTIFY atomsChanged)
    Q_PROPERTY(Units* units READ units WRITE setUnits NOTIFY unitsChanged)
    Q_PROPERTY(bool isValid READ isValid WRITE setIsValid NOTIFY isValidChanged)
public:
    System(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QVector3D origin() const;
    QVector3D size() const;
    int numberOfAtoms() const;
    float simulationTime() const;
    int timesteps() const;
    class Atoms* atoms() const;
    int numberOfAtomTypes() const;
    void reset();
    float volume() const;
    bool isValid() const;
    class Units* units() const;
    long realTime() const;

public slots:
    void setAtoms(class Atoms* atoms);
    void setIsValid(bool isValid);
    void setUnits(class Units* units);
    void setRealTime(long realTime);
    void setTimesteps(int timesteps);

signals:
    void originChanged(QVector3D origin);
    void sizeChanged(QVector3D size);
    void numberOfAtomsChanged(int numberOfAtoms);
    void simulationTimeChanged(float simulationTime);
    void timestepsChanged(int timesteps);
    void atomsChanged(class Atoms* atoms);
    void numberOfAtomTypesChanged(int numberOfAtomTypes);
    void volumeChanged(float volume);
    void isValidChanged(bool isValid);
    void unitsChanged(class Units* units);
    void realTimeChanged(long realTime);

private:
    class Atoms* m_atoms = nullptr;
    class Units* m_units = nullptr;
    QVector3D m_origin;
    QVector3D m_size;
    int m_numberOfAtoms = 0;
    float m_simulationTime = 0;
    int m_timesteps = 0;
    int m_numberOfAtomTypes = 0;
    long m_realTime = 0.0;
    float m_volume = 0;
    bool m_isValid = false;
};

#endif // SYSTEM_H
