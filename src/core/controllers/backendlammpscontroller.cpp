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
    const auto& atomData = m_rawData.atomData;
    resize(&m_particleData, atomData.size);
    m_particleData.timestep = m_rawData.systemData.ntimestep;

    // TODO(anders.hafreager) Simdify
    for (int i = 0; i < atomData.size; i++) {
        m_particleData.positions[i][0] = static_cast<float>(atomData.x[3 * i + 0]);
        m_particleData.positions[i][1] = static_cast<float>(atomData.x[3 * i + 1]);
        m_particleData.positions[i][2] = static_cast<float>(atomData.x[3 * i + 2]);
        m_particleData.types[i] = atomData.type[i];
        m_particleData.ids[i] = atomData.id[i];
        m_particleData.visible[i] = true;
        m_particleData.colors[i] = QVector3D(1.0, 0.0, 0.0);
        m_particleData.radii[i] = 0.3;
    }

    return m_particleData;
}

//void BackendLAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e)
//{
//}

//void BackendLAMMPSController::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change)
//{
//}

} // namespace atomify
