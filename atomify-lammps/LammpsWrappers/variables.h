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
    int count() const;
    QVariant model() const;

signals:
    void countChanged(int count);
    void modelChanged(QVariant model);

public slots:
    void setCount(int count);
    void setModel(QVariant model);

private:
    int m_count = 0;
    QVariant m_model;
};

#endif // VARIABLES_H
