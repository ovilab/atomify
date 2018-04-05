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

void BackendLAMMPSController::synchronize()
{
    if (!m_thread->dataDirty()) {
        return;
    }

    const auto data = m_thread->data();
    const auto &atomData = data.atomData;
    uint64_t visibleAtomCount = atomData.size;

    QByteArray sphereBufferData;
    sphereBufferData.resize(visibleAtomCount * sizeof(SphereVBOData));
    SphereVBOData *vboData = reinterpret_cast<SphereVBOData *>(sphereBufferData.data());
    for(size_t i=0; i<visibleAtomCount; i++) {
        SphereVBOData &vbo = vboData[i];

        const int id = atomData.id[i];
        vbo.position[0] = atomData.x[3*i + 0];
        vbo.position[1] = atomData.x[3*i + 1];
        vbo.position[2] = atomData.x[3*i + 2];
        vbo.color[0] = 1.0;
        vbo.radius = 0.3;
        vbo.particleId = id;
        vbo.flags = 0; // TODO add back
//        vbo.flags = m_selectedParticles.contains(particleId) ? Selected : 0;
    }

    {
        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(m_spheresBufferId);
        change->setPropertyName("data");
        change->setValue(QVariant::fromValue(sphereBufferData));
        notifyObservers(change);
    }

    {
        qDebug() << "Sending visible atom count" << visibleAtomCount;
        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
        change->setPropertyName("visibleAtomCount");
        change->setValue(visibleAtomCount);
        change->setDeliveryFlags(Qt3DCore::QSceneChange::Nodes);
        notifyObservers(change);
    }


    qDebug() << "Did sync data: " << data.systemData.ntimestep;
}

void BackendLAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e)
{

}

void BackendLAMMPSController::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<LAMMPSControllerData>>(change);
    const auto &data = typedChange->data;
    m_spheresBufferId = data.spheresBufferId;
    qDebug() <<" Got id: " << m_spheresBufferId;
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
