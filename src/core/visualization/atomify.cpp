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

Qt3DRender::QGeometryRenderer* Atomify::spheresRenderer() const
{
    return m_spheresRenderer;
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

void Atomify::setSpheresRenderer(Qt3DRender::QGeometryRenderer* spheresRenderer)
{
    if (m_spheresRenderer == spheresRenderer)
        return;

    m_spheresRenderer = spheresRenderer;
    emit spheresRendererChanged(m_spheresRenderer);
}

} // namespace atomify
