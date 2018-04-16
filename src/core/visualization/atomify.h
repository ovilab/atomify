#ifndef CORE_VISUALIZATION_ATOMIFY_H
#define CORE_VISUALIZATION_ATOMIFY_H

#include <Qt3DCore/QComponent>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QGeometryRenderer>

namespace atomify {

class LAMMPSController;

class Atomify : public Qt3DCore::QComponent {
    Q_OBJECT
    Q_PROPERTY(atomify::LAMMPSController* controller READ controller WRITE setController NOTIFY controllerChanged) // TODO(anders.hafreager) Create abstract class for MDControllers
    Q_PROPERTY(Qt3DRender::QBuffer* spheresBuffer READ spheresBuffer WRITE setSpheresBuffer NOTIFY spheresBufferChanged)
    Q_PROPERTY(Qt3DRender::QGeometryRenderer* spheresGeometryRenderer READ spheresGeometryRenderer WRITE setSpheresGeometryRenderer NOTIFY spheresGeometryRendererChanged)

public:
    Atomify();
    LAMMPSController* controller() const;
    Qt3DRender::QBuffer* spheresBuffer() const;
    Qt3DRender::QGeometryRenderer* spheresGeometryRenderer() const;

public slots:
    void setController(LAMMPSController* controller);
    void setSpheresBuffer(Qt3DRender::QBuffer* spheresBuffer);
    void setSpheresGeometryRenderer(Qt3DRender::QGeometryRenderer* spheresGeometryRenderer);

signals:
    void controllerChanged(LAMMPSController* controller);
    void spheresBufferChanged(Qt3DRender::QBuffer* spheresBuffer);
    void spheresGeometryRendererChanged(Qt3DRender::QGeometryRenderer* spheresGeometryRenderer);

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& change) override;

private:
    LAMMPSController* m_controller;
    Qt3DRender::QBuffer* m_spheresBuffer;
    Qt3DRender::QGeometryRenderer* m_spheresGeometryRenderer;
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

struct AtomifyData {
    Qt3DCore::QNodeId spheresBufferId;
    Qt3DCore::QNodeId spheresGeometryRendererId;
    Qt3DCore::QNodeId controllerId;
};

} // namespace atomify

#endif // CORE_VISUALIZATION_ATOMIFY_H
