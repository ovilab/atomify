#ifndef COMPUTES_H
#define COMPUTES_H

#include <QObject>
#include <mpi.h>
#include <lammps.h>
#include <modify.h>
#include <QMap>
#include <QVariant>

class Computes : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
public:
    explicit Computes(QObject *parent = 0);
    void synchronize(class LAMMPSController *lammpsController);
    int count() const;
    QVariant model() const;
    bool active() const;

signals:
    void countChanged(int count);
    void modelChanged(QVariant model);
    void activeChanged(bool active);

public slots:
    void setCount(int count);
    void setModel(QVariant model);
    void setActive(bool active);

private:
    void addCompute(QString identifier, LAMMPSController *lammpsController);
    void removeCompute(QString identifier, LAMMPSController *lammpsController);
    QList<QObject*> m_data;
    QMap<QString, QObject*> m_dataMap;
    QVariant m_model;
    int m_count = 0;
    void reset(LAMMPSController *lammpsController);
    bool m_active = false;
};

#endif // COMPUTES_H
