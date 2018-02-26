#include "atomselection.h"
#include <QVariant>
AtomSelection::AtomSelection(QObject *parent) : QObject(parent),
  m_id(-1)
{

}

QVariantMap AtomSelection::properties() const
{
    return m_properties;
}

void AtomSelection::setProperties(const QVariantMap &properties)
{
    if (properties == m_properties)
        return;

    m_properties = properties;
    emit propertiesChanged(properties);
}

int AtomSelection::id() const
{
    return m_id;
}

QVector3D AtomSelection::position() const
{
    return m_position;
}

QVector3D AtomSelection::velocity() const
{
   return m_velocity;
}

QVector3D AtomSelection::force() const
{
  return m_force;
}

void AtomSelection::setPosition(QVector3D position)
{
    if (m_position == position)
        return;
    m_position = position;
    emit positionChanged(position);
}

void AtomSelection::setVelocity(QVector3D velocity)
{
    if (m_velocity == velocity)
        return;
    m_velocity = velocity;
    emit velocityChanged(velocity);
}

void AtomSelection::setForce(QVector3D force)
{
    if (m_force == force)
        return;
    m_force = force;
    emit forceChanged(force);
}

void AtomSelection::setId(int id)
{
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}
