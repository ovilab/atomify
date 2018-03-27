#include "lammpscontroller2.h"
#include <QDebug>

namespace atomify {

LAMMPSController2::LAMMPSController2(Qt3DCore::QNode *parent)
    : Qt3DCore::QComponent(parent)
    , m_spheresBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this))
{
    qDebug() << "Yeah baby";
}

Qt3DRender::QBuffer *LAMMPSController2::spheresBuffer() const
{
    return m_spheresBuffer.data();
}

Qt3DCore::QNodeCreatedChangeBasePtr LAMMPSController2::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<LAMMPSControllerData>::create(this);
    auto &data = creationChange->data;
    data.spheresBufferId = m_spheresBuffer->id();
    return creationChange;
}

} // namespace atomify

