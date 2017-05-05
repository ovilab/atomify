#ifndef FIXES_H
#define FIXES_H

#include <QObject>
#include <QVariant>

class Fixes : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

public:
    explicit Fixes(QObject *parent = 0);

    QVariant model() const;
    void reset();
    void synchronize(class LAMMPSController *lammpsController);
    int count() const;

    void synchronizeQML(LAMMPSController *lammpsController);
signals:
    void modelChanged(QVariant model);
    void countChanged(int count);

public slots:
    void setModel(QVariant model);
    void setCount(int count);

private:
    QVariant m_model;
    QList<QObject *> m_data;
    QMap<QString, QObject *> m_dataMap;

    void add(QString identifier, class LAMMPSController *lammpsController);
    void remove(QString identifier);
    int m_count = 0;
    bool addOrRemove(LAMMPSController *lammpsController);
};

#endif // FIXES_H
