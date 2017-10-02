#ifndef CPFIXINDENT_H
#define CPFIXINDENT_H

#include "cpfix.h"
#include <QQuaternion>

class CPFixIndent : public CPFix
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(int dimension READ dimension WRITE setDimension NOTIFY dimensionChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(QQuaternion rotation READ rotation WRITE setRotation NOTIFY rotationChanged)

public:
    explicit CPFixIndent(QObject *parent = 0);
    ~CPFixIndent();

private:
    QString m_type;
    QVector3D m_position;
    int m_dimension = 0;
    qreal m_radius = 1;
    QQuaternion m_rotation;

public:
    virtual void copyData(class LAMMPSController *lammpsController) override;
    QString type() const;
    QVector3D position() const;
    int dimension() const;
    qreal radius() const;
    QQuaternion rotation() const;

public slots:
    void setType(QString type);
    void setPosition(QVector3D position);
    void setDimension(int dimension);
    void setRadius(qreal radius);
    void setRotation(QQuaternion rotation);

signals:
    void typeChanged(QString type);
    void positionChanged(QVector3D position);
    void dimensionChanged(int dimension);
    void radiusChanged(qreal radius);
    void rotationChanged(QQuaternion rotation);
};

#endif // CPFIXINDENT_H
