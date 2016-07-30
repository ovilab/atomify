#ifndef ATOMIFY_UNITS_H
#define ATOMIFY_UNITS_H
#include <QObject>
#include <mpi.h>
#include <lammps.h>

class Units : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
    enum Type { LJ, Real, Metal, SI, CGS, Electron, Micro, Nano };
    explicit Units(QObject *parent = 0);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QString name() const;

signals:

    void nameChanged(QString name);

public slots:

void setName(QString name);

private:
    Type type;
    char *unit_style = nullptr; // From lammps
    QString m_name;
};

#endif // ATOMIFY_UNITS_H
