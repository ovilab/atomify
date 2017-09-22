#ifndef SLICE_H
#define SLICE_H
#include "modifier.h"
#include <QVector3D>

class Slice : public Modifier
{
    Q_OBJECT
    Q_PROPERTY(float distance READ distance WRITE setDistance NOTIFY distanceChanged)
    Q_PROPERTY(QVector3D normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(bool reversed READ reversed WRITE setReversed NOTIFY reversedChanged)
private:
    float m_distance = 1;
    QVector3D m_normal;
    QVector3D m_normalizedNormal;
    float m_width = 0;
    bool m_reversed = false;
    bool vectorIsInside(const QVector3D &position);
    float distanceToPlane(const QVector3D &position, const QVector3D &plane, const QVector3D &normal);

public:
    Slice();
    virtual void apply(class AtomData &atomData) override;
    float distance() const;
    QVector3D normal() const;
    float width() const;
    bool reversed() const;

public slots:
    void setDistance(float distance);
    void setNormal(QVector3D normal);
    void setWidth(float width);
    void setReversed(bool reversed);

signals:
    void distanceChanged(float distance);
    void normalChanged(QVector3D normal);
    void widthChanged(float width);
    void reversedChanged(bool reversed);
};

#endif // SLICE_H
