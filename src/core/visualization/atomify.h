#ifndef ATOMIFY_H
#define ATOMIFY_H

#include <Qt3DCore/QComponent>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometryRenderer>

namespace atomify {

class LAMMPSController;

class Atomify : public Qt3DCore::QComponent {
    Q_OBJECT
    Q_PROPERTY(LAMMPSController* controller READ controller WRITE setController NOTIFY controllerChanged) // TODO(anders.hafreager) Create abstract class for MDControllers
    Q_PROPERTY(Qt3DRender::QBuffer* spheresBuffer READ spheresBuffer WRITE setSpheresBuffer NOTIFY spheresBufferChanged)
    Q_PROPERTY(Qt3DRender::QGeometryRenderer* spheresRenderer READ spheresRenderer WRITE setSpheresRenderer NOTIFY spheresRendererChanged)
public:
    Atomify();
    LAMMPSController* controller() const;
    Qt3DRender::QBuffer* spheresBuffer() const;
    Qt3DRender::QGeometryRenderer* spheresRenderer() const;

public slots:
    void setController(LAMMPSController* controller);
    void setSpheresBuffer(Qt3DRender::QBuffer* spheresBuffer);
    void setSpheresRenderer(Qt3DRender::QGeometryRenderer* spheresRenderer);

signals:
    void controllerChanged(LAMMPSController* controller);
    void spheresBufferChanged(Qt3DRender::QBuffer* spheresBuffer);
    void spheresRendererChanged(Qt3DRender::QGeometryRenderer* spheresRenderer);

private:
    LAMMPSController* m_controller;
    Qt3DRender::QBuffer* m_spheresBuffer;
    Qt3DRender::QGeometryRenderer* m_spheresRenderer;
};

} // namespace atomify

#endif // ATOMIFY_H
