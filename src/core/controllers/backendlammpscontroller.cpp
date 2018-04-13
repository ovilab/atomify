#include "backendlammpscontroller.h"
#include "core/aspects/atomifyaspect.h"
#include "lammpscontroller.h"
#include "lammpsthread.h"
#include <QDebug>
#include <QPropertyUpdatedChange>

namespace atomify {

BackendLAMMPSController::BackendLAMMPSController()
    : Qt3DCore::QBackendNode(Qt3DCore::QBackendNode::ReadWrite)
    , m_thread(new LAMMPSThread())
{
    qDebug() << "OOOOOH YEAH!!! Starting LAMMPS.";
    m_thread->start();
}

LAMMPSData BackendLAMMPSController::synchronize(LAMMPSData data)
{
    if (!m_thread->dataDirty()) {
        data.empty = true;
        return data;
    }

    LAMMPSData newData = m_thread->data(std::move(data));
    newData.empty = false;
    return newData;
}

void BackendLAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e)
{
}

void BackendLAMMPSController::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<LAMMPSControllerData>>(change);
    const auto& data = typedChange->data;
    m_spheresBufferId = data.spheresBufferId;
}

Qt3DCore::QNodeId BackendLAMMPSController::spheresBufferId() const
{
    return m_spheresBufferId;
}

void BackendLAMMPSController::notifySphereBuffer(const QByteArray& buffer, uint64_t sphereCount)
{
    {
        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(m_spheresBufferId);
        change->setPropertyName("data");
        change->setValue(QVariant::fromValue(buffer));
        notifyObservers(change);
    }

    {
        qDebug() << "Sending visible atom count" << sphereCount;
        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        change->setPropertyName("visibleAtomCount");
        change->setValue(static_cast<int>(sphereCount)); // Cast for linux compilation
        change->setDeliveryFlags(Qt3DCore::QSceneChange::Nodes);
        notifyObservers(change);
    }
}

LAMMPSControllerMapper::LAMMPSControllerMapper(AtomifyAspect* aspect)
{
}

Qt3DCore::QBackendNode* LAMMPSControllerMapper::create(const Qt3DCore::QNodeCreatedChangeBasePtr& change) const
{
    auto controller = new BackendLAMMPSController;
    m_controllers[change->subjectId()] = controller;
    return controller;
}

Qt3DCore::QBackendNode* LAMMPSControllerMapper::get(Qt3DCore::QNodeId id) const
{
    return m_controllers[id];
}

void LAMMPSControllerMapper::destroy(Qt3DCore::QNodeId id) const
{
    auto controller = m_controllers.take(id);
    delete controller;
}

QList<BackendLAMMPSController*> LAMMPSControllerMapper::controllers()
{
    return m_controllers.values();
}
}
