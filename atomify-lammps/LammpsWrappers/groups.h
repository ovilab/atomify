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
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int bitmask READ bitmask WRITE setBitmask NOTIFY bitmaskChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
public:
    CPGroup(QObject *parent = nullptr);
    void update(LAMMPS_NS::LAMMPS *lammps);
    QString identifier() const;
    int count() const;
    int bitmask() const;
    bool hovered() const;
    bool visible() const;

public slots:
    void setCount(int count);
    void setBitmask(int bitmask);
    void setHovered(bool hovered);
    void setVisible(bool visible);
    void setIdentifier(QString identifier);

signals:
    void countChanged(int count);
    void bitmaskChanged(int bitmask);
    void hoveredChanged(bool hovered);
    void visibleChanged(bool visible);
    void identifierChanged(QString identifier);

private:
    bool m_hovered = false;
    bool m_visible = true;
    int m_count = 0;
    int m_bitmask = 0;
    QString m_identifier;
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
    QList<CPGroup*> groups();
    void reset();

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
    int m_count = 0;
    bool m_active = false;
    void remove(QString identifier);
    void add(QString identifier);
};

#endif // GROUPS_H
