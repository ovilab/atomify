#ifndef GROUPS_H
#define GROUPS_H
#include <QObject>
#include <mpi.h>
#include <lammps.h>
#include <QVariantList>
#include <QMap>
#include <QList>

class CPGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
public:
    CPGroup(QObject *parent);
    QString name() const;
    int count() const;

public slots:
    void setName(QString name);
    void setCount(int count);

signals:
    void nameChanged(QString name);
    void countChanged(int count);
private:
    int m_count;
    QString m_name;
};

class Groups : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
public:
    Groups(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QVariant model() const;

public slots:
    void setModel(QVariant model);

signals:
    void modelChanged(QVariant model);

private:
    QList<QObject*> m_data;
    QMap<QString, QObject*> m_dataMap;
    QVariant m_model;
    void update(class LAMMPS_NS::Group *group);
};

#endif // GROUPS_H
