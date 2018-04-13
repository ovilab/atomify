#include "atomify.h"

namespace atomify {

Atomify::Atomify()
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
