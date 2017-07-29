#include "slice.h"
#include "../system.h"
#include "../atomdata.h"

Slice::Slice()
{
    setEnabled(false);
    setNormal(QVector3D(1,0,0));
}

float Slice::distanceToPlane(const QVector3D &position, const QVector3D &plane, const QVector3D &normal) {
    return QVector3D::dotProduct((position - plane), normal);
}

bool Slice::vectorIsInside(const QVector3D &position) {
    QVector3D plane = m_normal*distance();
    if(m_width==0) {
        return distanceToPlane(position, plane, m_normal) < 0;
    } else {
        return abs(distanceToPlane(position, plane, m_normal)) < 0.5*m_width;
    }
}

void Slice::apply(AtomData &atomData)
{
    if(!enabled() || m_normal.length()==0) return;
    for(int i=0; i<atomData.size(); i++) {
        if(!vectorIsInside(atomData.positions[i])) {
            atomData.visible[i] = false;
        }
    }
}

float Slice::distance() const
{
    return m_distance;
}

QVector3D Slice::normal() const
{
    return m_normal;
}

float Slice::width() const
{
    return m_width;
}

void Slice::setDistance(float distance)
{
    if (m_distance == distance)
        return;

    m_distance = distance;
    emit distanceChanged(distance);
}

void Slice::setNormal(QVector3D normal)
{
    if (m_normal == normal)
        return;

    m_normal = normal.normalized();
    emit normalChanged(normal);
}

void Slice::setWidth(float width)
{
    if (m_width == width)
        return;

    m_width = width;
    emit widthChanged(width);
}
