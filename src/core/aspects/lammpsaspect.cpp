#include "lammpsaspect.h"
#include "core/controllers/backendlammpscontroller.h"
#include "core/controllers/lammpscontroller.h"
#include <SimVis/SphereData>

#include <QAbstractAspect>
#include <QAspectJob>
#include <QPair>
#include <QPropertyUpdatedChange>

namespace atomify {

LAMMPSAspect::LAMMPSAspect(QObject* parent)
    : Qt3DCore::QAbstractAspect(parent)
{
    // Register the mapper to handle creation, lookup, and destruction of backend nodes
    m_mapper = QSharedPointer<LAMMPSControllerMapper>::create(this);
    registerBackendType<LAMMPSController>(m_mapper);
}

static void convertData(const LAMMPSData& pendingRawData, ParticleData& particleData)
{
    const auto& atomData = pendingRawData.atomData;
    resize(&particleData, atomData.size);
    particleData.timestep = pendingRawData.systemData.ntimestep;
    //#ifdef __GNUC__
    //#pragma GCC ivdep
    //#endif
#pragma omp simd
    for (int i = 0; i < atomData.size; i++) {
        particleData.positions[i][0] = static_cast<float>(atomData.x[3 * i + 0]);
        particleData.positions[i][1] = static_cast<float>(atomData.x[3 * i + 1]);
        particleData.positions[i][2] = static_cast<float>(atomData.x[3 * i + 2]);
        particleData.bitmask[i] = atomData.mask[i];
        particleData.types[i] = atomData.type[i];
        particleData.ids[i] = atomData.id[i];
        particleData.visible[i] = true;
        particleData.colors[i] = QVector3D(1.0, 0.0, 0.0);
        particleData.radii[i] = 0.3;
    }
}

static void createSphereBufferData(const ParticleData& particleData, QByteArray& sphereBufferData)
{
    sphereBufferData.resize(particleData.size * sizeof(SphereVBOData));

    SphereVBOData* vboData = reinterpret_cast<SphereVBOData*>(sphereBufferData.data());
    //#ifdef __GNUC__
    //#pragma GCC ivdep
    //#endif
#pragma omp simd
    for (size_t i = 0; i < particleData.size; i++) {
        SphereVBOData& vbo = vboData[i];

        const int id = particleData.ids[i];
        vbo.position = particleData.positions[i];
        vbo.color[0] = 1.0;
        vbo.color[1] = 0.0;
        vbo.color[2] = 0.0;
        vbo.radius = 0.3;
        vbo.particleId = id;
        vbo.flags = 0; // TODO add back
        //        vbo.flags = m_selectedParticles.contains(particleId) ? Selected : 0;
    }
}

static void setSphereBufferOnControllers(QMap<Qt3DCore::QNodeId, QPair<bool, QByteArray>>& sphereBufferData, const LAMMPSControllerMapper& mapper)
{
    for (const auto& nodeId : sphereBufferData.keys()) {
        if (!sphereBufferData[nodeId].first) {
            continue;
        }
        const auto controller = dynamic_cast<BackendLAMMPSController*>(mapper.get(nodeId));
        uint64_t sphereCount = sphereBufferData[nodeId].second.size() / sizeof(SphereVBOData);
        controller->setSphereBufferData(sphereBufferData[nodeId].second, sphereCount);
    }
}

QVector<Qt3DCore::QAspectJobPtr> LAMMPSAspect::jobsToExecute(qint64 time)
{
    class LambdaJob : public Qt3DCore::QAspectJob {
    public:
        LambdaJob(std::function<void()> callable)
            : m_callable(callable)
        {
        }

    private:
        const std::function<void()> m_callable;
        void run() override { m_callable(); }
    };

    using LambdaJobPtr = QSharedPointer<LambdaJob>;
    QVector<Qt3DCore::QAspectJobPtr> jobs;

    for (auto* controller : m_mapper->controllers()) {
        auto nodeId = controller->peerId();

        auto job = LambdaJobPtr::create([this, controller, nodeId]() {
            m_rawData[nodeId] = controller->synchronize(std::move(m_rawData[nodeId]));
            if (m_rawData[nodeId].empty)
                return;
            convertData(m_rawData[nodeId], m_particleData[nodeId]);

            //            m_rawData = controller->synchronize(std::move(m_rawData));
            //            std::cout << "Empty: " << m_rawData.empty << std::endl;
            //            return;
            //            if (m_rawData.empty)
            //                return;
            //            convertData(m_rawData, m_particleData[nodeId]);
            createSphereBufferData(m_particleData[nodeId], m_sphereBufferData[nodeId]);

            uint64_t sphereCount = m_sphereBufferData[nodeId].size() / sizeof(SphereVBOData);
            controller->setSphereBufferData(m_sphereBufferData[nodeId], sphereCount);
        });

        jobs.append(job);
    }
    return jobs;
}

} // namespace atomify

QT3D_REGISTER_NAMESPACED_ASPECT("lammps", QT_PREPEND_NAMESPACE(atomify), LAMMPSAspect)
