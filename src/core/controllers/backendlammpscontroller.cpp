#include "backendlammpscontroller.h"
#include "core/aspects/atomifyaspect.h"
#include "lammpscontroller.h"
#include "lammpsthread.h"
#include <QDebug>
#include <QPropertyUpdatedChange>

namespace atomify {

BackendLAMMPSController::BackendLAMMPSController()
    : BackendAbstractController()
    , m_thread(new LAMMPSThread())
{
    qDebug() << "OOOOOH YEAH!!! Starting LAMMPS.";
    m_thread->start();
}

bool BackendLAMMPSController::synchronize()
{
    if (!m_thread->dataDirty()) {
        return false;
    }

    m_rawData = m_thread->data(std::move(m_rawData));
    return true;
}

const ParticleData& BackendLAMMPSController::createParticleData()
{
    //    const auto& atomData = pendingRawData.atomData;
    //    resize(&particleData, atomData.size);
    //    particleData.timestep = pendingRawData.systemData.ntimestep;
    //    //#ifdef __GNUC__
    //    //#pragma GCC ivdep
    //    //#endif
    //#pragma omp simd
    //    for (int i = 0; i < atomData.size; i++) {
    //        particleData.positions[i][0] = static_cast<float>(atomData.x[3 * i + 0]);
    //        particleData.positions[i][1] = static_cast<float>(atomData.x[3 * i + 1]);
    //        particleData.positions[i][2] = static_cast<float>(atomData.x[3 * i + 2]);
    //        particleData.bitmask[i] = atomData.mask[i];
    //        particleData.types[i] = atomData.type[i];
    //        particleData.ids[i] = atomData.id[i];
    //        particleData.visible[i] = true;
    //        particleData.colors[i] = QVector3D(1.0, 0.0, 0.0);
    //        particleData.radii[i] = 0.3;
    //    }

    //    return particleData;
}

void BackendLAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e)
{
}

void BackendLAMMPSController::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change)
{
    //    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<LAMMPSControllerData>>(change);
    //    const auto& data = typedChange->data;
    //    m_spheresBufferId = data.spheresBufferId;
}

void BackendLAMMPSController::notifySphereBuffer(const QByteArray& buffer, uint64_t sphereCount)
{
    //    {
    //        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(m_spheresBufferId);
    //        change->setPropertyName("data");
    //        change->setValue(QVariant::fromValue(buffer));
    //        notifyObservers(change);
    //    }

    //    {
    //        qDebug() << "Sending visible atom count" << sphereCount;
    //        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(peerId());
    //        change->setPropertyName("visibleAtomCount");
    //        change->setValue(static_cast<int>(sphereCount)); // Cast for linux compilation
    //        change->setDeliveryFlags(Qt3DCore::QSceneChange::Nodes);
    //        notifyObservers(change);
    //    }
}

} // namespace atomify
