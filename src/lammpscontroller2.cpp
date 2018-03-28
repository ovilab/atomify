#include "lammpscontroller2.h"

#include <QDebug>
#include <Qt3DCore/QPropertyUpdatedChangePtr>

namespace atomify {

LAMMPSController2::LAMMPSController2(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(parent)
    , m_spheresBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this))
    , m_visibleAtomCount(0)
    , m_states(new States(this))
{
    qDebug() << "Yeah baby";
}

Qt3DRender::QBuffer *LAMMPSController2::spheresBuffer() const
{
    return m_spheresBuffer.data();
}

int LAMMPSController2::visibleAtomCount() const
{
    return m_visibleAtomCount;
}

States *LAMMPSController2::states() const
{
  return m_states;
}

void LAMMPSController2::setStates(States *states)
{
  if (m_states == states)
    return;

  m_states = states;
  emit statesChanged(m_states);
}

void LAMMPSController2::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{
  const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
    if (change->propertyName() == QByteArrayLiteral("visibleAtomCount")) {
        const int visibleAtoms = change->value().value<int>();
        if (m_visibleAtomCount != visibleAtoms) {
            m_visibleAtomCount = visibleAtoms;
            qDebug() << "Got atoms" << m_visibleAtomCount;
            emit visibleAtomCountChanged();
        }
    }
}

Qt3DCore::QNodeCreatedChangeBasePtr LAMMPSController2::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<LAMMPSControllerData>::create(this);
    auto &data = creationChange->data;
    data.spheresBufferId = m_spheresBuffer->id();
    return creationChange;
}

} // namespace atomify

