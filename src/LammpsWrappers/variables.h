#ifndef VARIABLES_H
#define VARIABLES_H

#include <QObject>
#include <QVariant>

class Variables : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)

public:
    explicit Variables(QObject *parent = 0);
    void synchronize(class LAMMPSController *lammpsController);
    void synchronizeQML(LAMMPSController *lammpsController);
    void updateThreadOnDataObjects(QThread *thread);
    int count() const;
    QVector<class SimulatorControl *> simulatorControls();
    void reset();
    QVariant model() const;
signals:
    void countChanged(int count);
    void modelChanged(QVariant model);

public slots:
    void setCount(int count);
    void setModel(QVariant model);

private:
    QList<QObject*> m_data;
    QMap<QString, QObject*> m_dataMap;
    QVariant m_model;
    int m_count = 0;
    void add(QString identifier, class LAMMPSController *lammpsController);
    void remove(QString identifier);
    bool isSupported(class LAMMPSController *lammpsController, int ivar);
    void copyData(LAMMPSController *lammpsController);
    bool addOrRemove(LAMMPSController *lammpsController);
};

#endif // VARIABLES_H
