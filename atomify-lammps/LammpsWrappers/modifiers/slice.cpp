#include "slice.h"

Slice::Slice(Atoms *atoms, System *system) : Modifier(atoms, system)
{

}


void Slice::apply(AtomData &atomData)
{
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

    m_normal = normal;
    emit normalChanged(normal);
}

void Slice::setWidth(float width)
{
    if (m_width == width)
        return;

    m_width = width;
    emit widthChanged(width);
}
