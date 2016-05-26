#ifndef REGIONS_H
#define REGIONS_H

#include <QObject>
#include <mpi.h>
#include <lammps.h>
#include <QMap>
#include <QVariant>
class CPRegion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
public:
    CPRegion(QObject *parent = nullptr);
    QString name() const;
    int count() const;

public slots:
    void setName(QString name);
    void setCount(int count);

signals:
    void nameChanged(QString name);
    void countChanged(int count);
private:
    QString m_name;
    int m_count = 0;
};

class Regions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
public:
    Regions(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QVariant model() const;
    int count() const;

public slots:
    void setModel(QVariant model);
    void setCount(int count);

signals:
    void modelChanged(QVariant model);
    void countChanged(int count);

private:
    QList<QObject*> m_data;
    QMap<QString, QObject*> m_dataMap;
    QVariant m_model;
    void update(LAMMPS_NS::LAMMPS *lammps);
    int m_count = 0;
};

#endif // REGIONS_H
