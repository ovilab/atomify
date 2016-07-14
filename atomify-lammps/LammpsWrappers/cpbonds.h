#ifndef CPBONDS_H
#define CPBONDS_H

#include <QObject>
#include <QVector>

class CPBonds : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    CPBonds();
    bool enabled() const;
    QVector<QVector<float>> &bondLengths();
    float maxBondLength();
public slots:
    void setEnabled(bool enabled);

signals:
    void enabledChanged(bool enabled);

private:
    QVector<QVector<float>> m_bondLengths;
    bool m_enabled = false;
};

#endif // CPBONDS_H
