#ifndef BACKENDLAMMPSCONTROLLER_H
#define BACKENDLAMMPSCONTROLLER_H

#include <QBackendNode>
#include <QHash>

namespace atomify {

class BackendLAMMPSController : public Qt3DCore::QBackendNode
{
public:
    BackendLAMMPSController();

    // QBackendNode interface
protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) override;
    QScopedPointer<class LAMMPSThread> m_thread;
};

class LAMMPSControllerMapper : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit LAMMPSControllerMapper(class LAMMPSAspect *aspect);
    Qt3DCore::QBackendNode *create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    mutable QHash<Qt3DCore::QNodeId, class BackendLAMMPSController*> m_controllers;
    class LAMMPSAspect *m_aspect;
};
}

#endif // BACKENDLAMMPSCONTROLLER_H
