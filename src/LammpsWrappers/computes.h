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
public:
    explicit Computes(QObject *parent = 0);
    void synchronize(class LAMMPSController *lammpsController);
    void synchronizeQML(class LAMMPSController *lammpsController);
    void copyAll(class LAMMPSController *lammpsController);
    void computeAll(class LAMMPSController *lammpsController);
    void updateThreadOnDataObjects(QThread *thread);
    void reset();
    int count() const;
    QVariant model() const;
    QVector<class SimulatorControl*> simulatorControls();

signals:
    void countChanged(int count);
    void modelChanged(QVariant model);

public slots:
    void setCount(int count);
    void setModel(QVariant model);

private:
    void addCompute(QString identifier, LAMMPSController *lammpsController);
    void removeCompute(QString identifier);
    bool addOrRemove(LAMMPSController *lammpsController);
    QList<QObject*> m_data;
    QMap<QString, QObject*> m_dataMap;
    QVariant m_model;
    int m_count = 0;
};

#endif // COMPUTES_H
