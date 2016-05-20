#ifndef SYSTEM_H
#define SYSTEM_H

#include <QObject>
#include <QVector3D>
#include <mpi.h>
#include <lammps.h>

class System : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVector3D size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QVector3D origin READ origin WRITE setOrigin NOTIFY originChanged)
    QVector3D m_origin;
    QVector3D m_size;

public:
    System();
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QVector3D origin() const;
    QVector3D size() const;

public slots:
    void setOrigin(QVector3D origin);
    void setSize(QVector3D size);

signals:
    void originChanged(QVector3D origin);
    void sizeChanged(QVector3D size);
};

#endif // SYSTEM_H
