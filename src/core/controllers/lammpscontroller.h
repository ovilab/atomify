#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H

#include "states.h"

#include <QBuffer>
#include <QComponent>
#include <QGeometryRenderer>

namespace atomify {

class LAMMPSController : public Qt3DCore::QComponent {
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QBuffer* spheresBuffer READ spheresBuffer CONSTANT)
    Q_PROPERTY(States* states READ states WRITE setStates NOTIFY statesChanged)
    Q_PROPERTY(Qt3DRender::QGeometryRenderer* spheresRenderer READ spheresRenderer WRITE setSpheresRenderer NOTIFY spheresRendererChanged)
    Q_PROPERTY(int visibleAtomCount READ visibleAtomCount NOTIFY visibleAtomCountChanged)

public:
    explicit LAMMPSController(Qt3DCore::QNode* parent = nullptr);
    Qt3DRender::QBuffer* spheresBuffer() const;
    int visibleAtomCount() const;
    States* states() const;

    Qt3DRender::QGeometryRenderer* spheresRenderer() const;

signals:
    void visibleAtomCountChanged();
    void statesChanged(States* states);
    void spheresRendererChanged(Qt3DRender::QGeometryRenderer* spheresRenderer);

public slots:
    void setStates(States* states);
    void setSpheresRenderer(Qt3DRender::QGeometryRenderer* spheresRenderer);

private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
    virtual void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& change) override;

    QScopedPointer<Qt3DRender::QBuffer> m_spheresBuffer;
    Qt3DRender::QGeometryRenderer* m_spheresRenderer;
    int m_visibleAtomCount;
    States* m_states;
};

struct LAMMPSControllerData {
    Qt3DCore::QNodeId spheresBufferId;
};
}

#endif // LAMMPSCONTROLLER_H
