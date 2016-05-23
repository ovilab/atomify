#ifndef SLICE_H
#define SLICE_H
#include "modifier.h"

class Slice : public Modifier
{
    Q_OBJECT
    Q_PROPERTY(float distance READ distance WRITE setDistance NOTIFY distanceChanged)
    Q_PROPERTY(QVector3D normal READ normal WRITE setNormal NOTIFY normalChanged)
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    float m_distance = 0;
    QVector3D m_normal;
    float m_width = 0;

public:
    Slice(System *system);

    // Modifier interface
public:
    virtual void apply(AtomData &atomData) override;
    float distance() const;
    QVector3D normal() const;

    float width() const;

public slots:
    void setDistance(float distance);
    void setNormal(QVector3D normal);

    void setWidth(float width);

signals:
    void distanceChanged(float distance);
    void normalChanged(QVector3D normal);
    void widthChanged(float width);
};

#endif // SLICE_H
