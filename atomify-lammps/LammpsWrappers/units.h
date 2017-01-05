#ifndef ATOMIFY_UNITS_H
#define ATOMIFY_UNITS_H
#include <QObject>
#include <mpi.h>
#include <lammps.h>

class Units : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString time READ time WRITE setTime NOTIFY timeChanged)
    Q_PROPERTY(QString volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(QString density READ density WRITE setDensity NOTIFY densityChanged)
    Q_PROPERTY(QString length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(int dimensions READ dimensions WRITE setDimensions NOTIFY dimensionsChanged)
public:
    enum Type { LJ, Real, Metal, SI, CGS, Electron, Micro, Nano };
    explicit Units(QObject *parent = 0);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QString name() const;
    QString time() const;
    QString volume() const;
    QString density() const;
    QString length() const;
    int dimensions() const;

signals:
    void nameChanged(QString name);
    void timeChanged(QString time);
    void volumeChanged(QString volume);
    void densityChanged(QString density);
    void lengthChanged(QString length);
    void dimensionsChanged(int dimensions);

public slots:
    void setName(QString name);
    void setTime(QString time);
    void setVolume(QString volume);
    void setDensity(QString density);
    void setLength(QString length);
    void setDimensions(int dimensions);

private:
    Type type;
    char *unit_style = nullptr; // From lammps
    QString m_name;
    QString m_time;
    QString m_volume;
    QString m_density;
    QString m_length;
    int m_dimensions = 3;
};

#endif // ATOMIFY_UNITS_H
