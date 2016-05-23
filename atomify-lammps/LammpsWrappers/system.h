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
    Q_PROPERTY(float simulationTime READ simulationTime NOTIFY simulationTimeChanged)
    Q_PROPERTY(int timesteps READ timesteps NOTIFY timestepsChanged)
    QVector3D m_origin;
    QVector3D m_size;
    int m_numberOfAtoms = 0;
    float m_simulationTime = 0;
    int m_timesteps = 0;

public:
    System();
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QVector3D origin() const;
    QVector3D size() const;
    int numberOfAtoms() const;
    float simulationTime() const;
    int timesteps() const;

signals:
    void originChanged(QVector3D origin);
    void sizeChanged(QVector3D size);
    void numberOfAtomsChanged(int numberOfAtoms);
    void simulationTimeChanged(float simulationTime);
    void timestepsChanged(int timesteps);
};

#endif // SYSTEM_H
