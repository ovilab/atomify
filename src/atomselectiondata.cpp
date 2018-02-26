#include "atomselectiondata.h"
#include <QVariant>
AtomSelectionData::AtomSelectionData(QObject *parent) : QObject(parent),
  m_id(-1), m_type(-1)
{

}

QVariantMap AtomSelectionData::properties() const
{
    return m_properties;
}

void AtomSelectionData::setProperties(const QVariantMap &properties)
{
    if (properties == m_properties)
        return;

    m_properties = properties;
    emit propertiesChanged(properties);
}

int AtomSelectionData::id() const
{
    return m_id;
}

QVector3D AtomSelectionData::position() const
{
    return m_position;
}

QVector3D AtomSelectionData::velocity() const
{
   return m_velocity;
}

QVector3D AtomSelectionData::force() const
{
  return m_force;
}

void AtomSelectionData::setPosition(QVector3D position)
{
    if (m_position == position)
        return;
    m_position = position;
    emit positionChanged(position);
}

void AtomSelectionData::setVelocity(QVector3D velocity)
{
    if (m_velocity == velocity)
        return;
    m_velocity = velocity;
    emit velocityChanged(velocity);
}

void AtomSelectionData::setForce(QVector3D force)
{
    if (m_force == force)
        return;
    m_force = force;
    emit forceChanged(force);
}

int AtomSelectionData::type() const
{
  return m_type;
}

void AtomSelectionData::setId(int id)
{
  if (m_id == id)
    return;

    m_id = id;
    emit idChanged(m_id);
}

void AtomSelectionData::setType(int type)
{
  if (m_type == type)
    return;

  m_type = type;
  emit typeChanged(m_type);
}
