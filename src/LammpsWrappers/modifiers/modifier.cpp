#include "modifier.h"
#include "../atomdata.h"
#include "../system.h"

#include <QColor>

Modifier::Modifier()
{

}

bool Modifier::enabled() const
{
    return m_enabled;
}

void Modifier::setSystem(System *system)
{
    m_system = system;
}

void Modifier::parseCommand(QString cmd) {}

QVariantList Modifier::colorList() const
{
  return m_colorList;
}

void Modifier::setEnabled(bool enabled)
{
  if (m_enabled == enabled)
    return;

    m_enabled = enabled;
    emit enabledChanged(enabled);
}

void Modifier::setColorList(QVariantList colorList)
{
  if (m_colorList == colorList)
      return;

  m_colorList = colorList;
  emit colorListChanged(m_colorList);
  m_colors.clear();
  m_colors.reserve(m_colorList.size());
  for (QVariant c : m_colorList) {
      QColor color = c.value<QColor>();
      float r = static_cast<float>(color.redF());
      float g = static_cast<float>(color.greenF());
      float b = static_cast<float>(color.blueF());
      m_colors.push_back(QVector3D(r,g,b));
  }
}
