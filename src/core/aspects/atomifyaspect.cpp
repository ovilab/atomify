#include "atomifyaspect.h"
#include "core/controllers/backendlammpscontroller.h"
#include "core/controllers/lammpscontroller.h"
#include "core/visualization/atomify.h"
#include <SimVis/SphereData>

#include <QAbstractAspect>
#include <QAspectJob>
#include <QPropertyUpdatedChange>

namespace atomify {

AtomifyAspect::AtomifyAspect(QObject* parent)
    : Qt3DCore::QAbstractAspect(parent)
{
    // Register the mapper to handle creation, lookup, and destruction of backend nodes
    Mapper<BackendLAMMPSController> mapper(this);
    m_atomifyMapper = QSharedPointer<Mapper<BackendAtomify>>::create(this);
    registerBackendType<LAMMPSController>(m_lammpsMapper);
    registerBackendType<Atomify>(m_atomifyMapper);
}

static QByteArray createSphereBufferData(const ParticleData& particleData, QByteArray sphereBufferData)
{
    //    sphereBufferData.resize(particleData.size * sizeof(SphereVBOData));

    //    SphereVBOData* vboData = reinterpret_cast<SphereVBOData*>(sphereBufferData.data());
    //    //#ifdef __GNUC__
    //    //#pragma GCC ivdep
    //    //#endif
    //#pragma omp simd
    //    for (size_t i = 0; i < particleData.size; i++) {
    //        SphereVBOData& vbo = vboData[i];

    //        const int id = particleData.ids[i];
    //        vbo.position = particleData.positions[i];
    //        vbo.color[0] = 1.0;
    //        vbo.color[1] = 0.0;
    //        vbo.color[2] = 0.0;
    //        vbo.radius = 0.3;
    //        vbo.particleId = id;
    //        vbo.flags = 0; // TODO add back
    //        //        vbo.flags = m_selectedParticles.contains(particleId) ? Selected : 0;
    //    }

    //    return sphereBufferData;
}

//static void setSphereBufferOnControllers(QMap<Qt3DCore::QNodeId, QPair<bool, QByteArray>>& sphereBufferData, const LAMMPSControllerMapper& mapper)
//{
//    for (const auto& nodeId : sphereBufferData.keys()) {
//        if (!sphereBufferData[nodeId].first) {
//            continue;
//        }
//        const auto controller = dynamic_cast<BackendLAMMPSController*>(mapper.get(nodeId));
//        uint64_t sphereCount = sphereBufferData[nodeId].second.size() / sizeof(SphereVBOData);
//        controller->notifySphereBuffer(sphereBufferData[nodeId].second, sphereCount);
//    }
// }

//struct LAMMPSSynchronizationJob : public Qt3DCore::QAspectJob {
//    BackendLAMMPSController* controller = nullptr;
//    LAMMPSData m_rawData;
//    ParticleData m_particleData;
//    QByteArray m_sphereBufferData;

//    void run() override
//    {
//        m_rawData = controller->synchronize(std::move(m_rawData));
//        if (m_rawData.empty)
//            return;
//        m_particleData = convertData(m_rawData, std::move(m_particleData));
//        // m_particleData = applyModifiers(m_particleData, std::move(m_particleData));
//        m_sphereBufferData = createSphereBufferData(m_particleData, std::move(m_sphereBufferData));

//        uint64_t sphereCount = m_sphereBufferData.size() / sizeof(SphereVBOData);
//        controller->notifySphereBuffer(m_sphereBufferData, sphereCount);
//    }
//};

// using LAMMPSSynchronizationJobPtr = QSharedPointer<LAMMPSSynchronizationJob>;

QVector<Qt3DCore::QAspectJobPtr> AtomifyAspect::jobsToExecute(qint64 time)
{
    QVector<Qt3DCore::QAspectJobPtr> jobs;

    //    for (auto* controller : m_mapper->controllers()) {
    //        auto nodeId = controller->peerId();

    //        if (!m_jobs.contains(nodeId)) {
    //            m_jobs[nodeId] = LAMMPSSynchronizationJobPtr::create();
    //            m_jobs[nodeId]->controller = controller;
    //        }

    //        jobs.append(m_jobs[nodeId]);
    //    }
    return jobs;
}

} // namespace atomify

QT3D_REGISTER_NAMESPACED_ASPECT("atomify", QT_PREPEND_NAMESPACE(atomify), AtomifyAspect)
