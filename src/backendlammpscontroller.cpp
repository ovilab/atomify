#include "backendlammpscontroller.h"
#include "lammpsthread.h"
#include "lammpsaspect.h"

#include <QDebug>
namespace atomify {

BackendLAMMPSController::BackendLAMMPSController()
    : m_thread(new LAMMPSThread())
{
    qDebug() << "OOOOOH YEAH!!!";
}

void BackendLAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{

}

void BackendLAMMPSController::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{

}

LAMMPSControllerMapper::LAMMPSControllerMapper(LAMMPSAspect *aspect)
    : m_aspect(aspect)
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

}
