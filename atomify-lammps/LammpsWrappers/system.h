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
    QVector3D m_origin;
    QVector3D m_size;
    int m_numberOfAtoms = 0;

public:
    System();
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QVector3D origin() const;
    QVector3D size() const;
    int numberOfAtoms() const;

signals:
    void originChanged(QVector3D origin);
    void sizeChanged(QVector3D size);
    void numberOfAtomsChanged(int numberOfAtoms);
};

#endif // SYSTEM_H
