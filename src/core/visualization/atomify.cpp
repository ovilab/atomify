#include "atomify.h"
#include "core/controllers/lammpscontroller.h"

namespace atomify {

Atomify::Atomify()
    : m_controller(nullptr)
    , m_spheresBuffer(nullptr)
    , m_spheresGeometryRenderer(nullptr)
{
}

LAMMPSController* Atomify::controller() const
{
    return m_controller;
}

Qt3DRender::QBuffer* Atomify::spheresBuffer() const
{
    return m_spheresBuffer;
}

Qt3DRender::QGeometryRenderer* Atomify::spheresGeometryRenderer() const
{
    return m_spheresGeometryRenderer;
}

void Atomify::setController(LAMMPSController* controller)
{
    if (m_controller == controller)
        return;

    m_controller = controller;
    emit controllerChanged(m_controller);
}

void Atomify::setSpheresBuffer(Qt3DRender::QBuffer* spheresBuffer)
{
    if (m_spheresBuffer == spheresBuffer)
        return;

    m_spheresBuffer = spheresBuffer;
    emit spheresBufferChanged(m_spheresBuffer);
}

void Atomify::setSpheresGeometryRenderer(Qt3DRender::QGeometryRenderer* spheresGeometryRenderer)
{
    if (m_spheresGeometryRenderer == spheresGeometryRenderer)
        return;

    m_spheresGeometryRenderer = spheresGeometryRenderer;
    emit spheresGeometryRendererChanged(m_spheresGeometryRenderer);
}

} // namespace atomify

void atomify::Atomify::sceneChangeEvent(const Qt3DCore::QSceneChangePtr& change)
{
}

Qt3DCore::QNodeCreatedChangeBasePtr atomify::Atomify::createNodeCreationChange() const
{
    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<AtomifyData>::create(this);
    auto& data = creationChange->data;
    data.spheresBufferId = m_spheresBuffer->id();
    data.spheresGeometryRendererId = m_spheresGeometryRenderer->id();
    data.controllerId = m_controller->id();

    return creationChange;
}
