#ifndef REGIONS_H
#define REGIONS_H

#include <QObject>
#include <mpi.h>
#include <lammps.h>
#include <QMap>
#include <QVariant>
#include <QVector>
class CPRegion : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString identifier READ identifier WRITE setIdentifier NOTIFY identifierChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
public:
    CPRegion(QObject *parent = nullptr);
    int count() const;
    QString identifier() const;
    bool visible() const;
    bool hovered() const;
    void update(LAMMPS_NS::LAMMPS *lammps);
    bool containsAtom(int atomIndex);
public slots:
    void setCount(int count);
    void setIdentifier(QString identifier);
    void setVisible(bool visible);
    void setHovered(bool hovered);

signals:
    void countChanged(int count);
    void identifierChanged(QString identifier);
    void visibleChanged(bool visible);
    void hoveredChanged(bool hovered);

private:
    int m_count = 0;
    QString m_identifier;
    bool m_visible = true;
    bool m_hovered = false;
    QVector<int> m_containsAtom;
};

class Regions : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
public:
    Regions(class AtomifySimulator *simulator = nullptr);
    void synchronize(class LAMMPSController *lammpsController);
    void synchronizeQML(class LAMMPSController *lammpsController);
    QVariant model() const;
    int count() const;
    bool active() const;
    void reset();
    QList<CPRegion*> regions();

public slots:
    void setModel(QVariant model);
    void setCount(int count);
    void setActive(bool active);

signals:
    void modelChanged(QVariant model);
    void countChanged(int count);
    void activeChanged(bool active);

private:
    bool addOrRemove(class LAMMPSController *lammpsController);
    void add(QString identifier);
    void remove(QString identifier);
    QList<QObject*> m_data;
    QMap<QString, QObject*> m_dataMap;
    QVariant m_model;
    int m_count = 0;
    bool m_active = false;
};

#endif // REGIONS_H
