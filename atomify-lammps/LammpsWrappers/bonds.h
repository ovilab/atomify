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

public slots:
    void setEnabled(bool enabled);

signals:
    void enabledChanged(bool enabled);

private:
    QVector<QVector<float>> m_bondLengths;
    bool m_enabled = false;
};

#endif // BONDS_H
