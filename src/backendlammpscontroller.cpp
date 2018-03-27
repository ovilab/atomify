#include "backendlammpscontroller.h"
#include "lammpsthread.h"
#include "lammpsaspect.h"
#include "lammpscontroller2.h"

#include <QDebug>
#include <QPropertyUpdatedChange>
#include <SimVis/SphereData>

namespace atomify {

BackendLAMMPSController::BackendLAMMPSController()
    : m_thread(new LAMMPSThread())
{
    qDebug() << "OOOOOH YEAH!!! Starting LAMMPS.";
    m_thread->start();
}

void BackendLAMMPSController::synchronize()
{
    if (m_thread->dataDirty()) {
        auto data = m_thread->data();
        QByteArray spheresBufferData;
//        spheresBufferData.resize(data.positions.size() * sizeof(SphereVBOData));
        spheresBufferData.resize(10 * sizeof(SphereVBOData));
        SphereVBOData *posData = reinterpret_cast<SphereVBOData*>(spheresBufferData.data());
        // TODO can we just set the address here? Instead of copying.
        for(auto &pos : data.positions) {
            SphereVBOData d;
            d.color[0] = 1.0;
            d.radius = 1.0;
            d.position = pos;

            *posData++ = d;
        }

        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(m_spheresBufferId);
        change->setPropertyName("data");
        change->setValue(QVariant::fromValue(spheresBufferData));
        notifyObservers(change);
        qDebug() << "Did sync data: " << data.timestep;
    }
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
