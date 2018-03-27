#include "backendlammpscontroller.h"
#include "lammpsthread.h"
#include "lammpsaspect.h"
#include "lammpscontroller2.h"

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
    // TODO remove dummy variables
    float m_sphereScale = 1.0;
    // END TODO

    if (!m_thread->dataDirty()) {
        return;
    }

    const auto data = m_thread->data();
    const AtomData &atomData = data.atomData;
    const int visibleAtomCount = atomData.size();

    QByteArray sphereBufferData;
    sphereBufferData.resize(visibleAtomCount * sizeof(SphereVBOData));
    SphereVBOData *vboData = reinterpret_cast<SphereVBOData *>(sphereBufferData.data());
    for(int i=0; i<visibleAtomCount; i++) {
        const int particleId = atomData.lammpsParticleId[i];
        SphereVBOData &vbo = vboData[i];
        vbo.position = atomData.positions[i] + atomData.deltaPositions[i];
        vbo.color = atomData.colors[i];
        vbo.radius = atomData.radii[i]*m_sphereScale;
        vbo.particleId = particleId;
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


    qDebug() << "Did sync data: " << data.timestep;
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
