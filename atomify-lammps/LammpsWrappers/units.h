#ifndef ATOMIFY_UNITS_H
#define ATOMIFY_UNITS_H
#include <QObject>
#include <mpi.h>
#include <lammps.h>

class Units : public QObject
{
    Q_OBJECT
public:
    explicit Units(QObject *parent = 0);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
signals:

public slots:
private:
    char *unit_style = nullptr; // From lammps
};

#endif // ATOMIFY_UNITS_H
