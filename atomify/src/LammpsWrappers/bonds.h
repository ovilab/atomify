#ifndef BONDS_H
#define BONDS_H

#include <QObject>
#include <QVector>

class Bonds : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    Bonds();
    bool enabled() const;
    QVector<QVector<float>> &bondLengths();
    float maxBondLength();
    bool active() const;
    void reset();
public slots:
    void setEnabled(bool enabled);

signals:
    void enabledChanged(bool enabled);

private:
    QVector<QVector<float>> m_bondLengths;
    bool m_enabled = true;
};

#endif // BONDS_H
