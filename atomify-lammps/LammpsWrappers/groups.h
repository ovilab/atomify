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
    CPGroup(QObject *parent = nullptr);
    QString name() const;
    int count() const;

public slots:
    void setName(QString name);
    void setCount(int count);

signals:
    void nameChanged(QString name);
    void countChanged(int count);
private:
    int m_count = 0;
    QString m_name;
};

class Groups : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    Groups(class AtomifySimulator *simulator = nullptr);
    void synchronize(class LAMMPSController *lammpsController);
    QVariant model() const;
    int count() const;
    bool active() const;

public slots:
    void setModel(QVariant model);
    void setCount(int count);
    void setActive(bool active);

signals:
    void modelChanged(QVariant model);
    void countChanged(int count);
    void activeChanged(bool active);

private:
    QList<QObject*> m_data;
    QMap<QString, QObject*> m_dataMap;
    QVariant m_model;
    void update(class LAMMPS_NS::Group *group);
    int m_count = 0;
    bool m_active = false;
};

#endif // GROUPS_H
