#include "backendlammpscontroller.h"
#include "lammpsthread.h"
#include "lammpsaspect.h"
#include "lammpscontroller.h"
#include <QDebug>
#include <QPropertyUpdatedChange>
#include <SimVis/SphereData>

namespace atomify {

BackendLAMMPSController::BackendLAMMPSController()
    : Qt3DCore::QBackendNode(Qt3DCore::QBackendNode::ReadWrite)
    , m_thread(new LAMMPSThread())
{
    qDebug() << "OOOOOH YEAH!!! Starting LAMMPS.";
    m_thread->start();
}

LAMMPSData BackendLAMMPSController::synchronize()
{
    if (!m_thread->dataDirty()) {
        return LAMMPSData();
    }

    const auto data = m_thread->data();
    return data;
}

void BackendLAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{

}

void BackendLAMMPSController::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<LAMMPSControllerData>>(change);
    const auto &data = typedChange->data;
    m_spheresBufferId = data.spheresBufferId;
}

Qt3DCore::QNodeId BackendLAMMPSController::spheresBufferId() const
{
    return m_spheresBufferId;
}

LAMMPSControllerMapper::LAMMPSControllerMapper(LAMMPSAspect *aspect)
{

}

Qt3DCore::QBackendNode *LAMMPSControllerMapper::create(const Qt3DCore::QNodeCreatedChangeBasePtr &change) const
{
    auto controller = new BackendLAMMPSController;
    m_controllers[change->subjectId()] = controller;
    return controller;
}

Qt3DCore::QBackendNode *LAMMPSControllerMapper::get(Qt3DCore::QNodeId id) const
{
    return m_controllers[id];
}

void LAMMPSControllerMapper::destroy(Qt3DCore::QNodeId id) const
{
    auto controller = m_controllers.take(id);
    delete controller;
}

QList<BackendLAMMPSController *> LAMMPSControllerMapper::controllers()
{
    return m_controllers.values();
}

}
