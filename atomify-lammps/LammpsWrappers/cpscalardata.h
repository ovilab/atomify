#ifndef CPSCALARDATA_H
#define CPSCALARDATA_H

#include <QObject>

class CPScalarData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double current READ current WRITE setCurrent NOTIFY currentChanged)

public:
    explicit CPScalarData(QObject *parent = 0);
    double current() const;
    void add(double time, double value);

signals:
    void currentChanged(double current);

public slots:
    void setCurrent(double current);

private:
    double m_current = 0;
};

#endif // CPSCALARDATA_H
