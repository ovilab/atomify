#ifndef GROUPS_H
#define GROUPS_H
#include <QObject>
#include <mpi.h>
#include <lammps.h>
#include <QVariantList>

class Groups : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QString> groups READ groups WRITE setGroups NOTIFY groupsChanged)


public:
    Groups(class AtomifySimulator *simulator = nullptr);
    void synchronize(LAMMPS_NS::LAMMPS *lammps);
    QList<QString> groups() const;

public slots:
    void setGroups(QList<QString> groups);

signals:
    void groupsChanged(QList<QString> groups);
private:
    QList<QString> m_groups;
};

#endif // GROUPS_H
