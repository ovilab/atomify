#include "lammpsaspect.h"
#include "core/controllers/backendlammpscontroller.h"
#include "core/controllers/lammpscontroller.h"
#include <SimVis/SphereData>

#include <QPair>
#include <QAbstractAspect>
#include <QAspectJob>
#include <QPropertyUpdatedChange>

namespace atomify {

LAMMPSAspect::LAMMPSAspect(QObject *parent)
    : Qt3DCore::QAbstractAspect(parent)
{
    // Register the mapper to handle creation, lookup, and destruction of backend nodes
    m_mapper = QSharedPointer<LAMMPSControllerMapper>::create(this);
    registerBackendType<LAMMPSController>(m_mapper);
}

static void copyDataFromLAMMPS(const QList<class BackendLAMMPSController*> &controllers, QMap<Qt3DCore::QNodeId, QPair<bool, LAMMPSData>> &pendingRawData) {
    for (const auto &controller : controllers) {
        auto data = controller->synchronize();
        if (data.empty)
            return;

        if (!pendingRawData.contains(controller->peerId())) {
            pendingRawData[controller->peerId()] = { true, LAMMPSData() };
        }

        pendingRawData[controller->peerId()].first = true;
        pendingRawData[controller->peerId()].second = std::move(data);
    }
}

static void convertData(const QMap<Qt3DCore::QNodeId, QPair<bool, LAMMPSData>> &pendingRawData, QMap<Qt3DCore::QNodeId, QPair<bool, ParticleData>> &pendingParticleData) {
    for (auto nodeId : pendingRawData.keys()) {
        if (pendingRawData.contains(nodeId) && pendingRawData[nodeId].first) {
            if (!pendingParticleData.contains(nodeId)) {
                pendingParticleData[nodeId] = { true, ParticleData() };
            }

            pendingParticleData[nodeId].first = true;
            auto &particleData = pendingParticleData[nodeId].second;

            auto data = pendingRawData[nodeId];
            const auto &atomData = data.second.atomData;
            resize(&particleData, atomData.size);
            particleData.timestep = data.second.systemData.ntimestep;
            //#ifdef __GNUC__
            //#pragma GCC ivdep
            //#endif
#pragma omp simd
            for (int i = 0; i < atomData.size; i++) {
                particleData.positions[i][0] = static_cast<float>(atomData.x[3*i+0]);
                particleData.positions[i][1] = static_cast<float>(atomData.x[3*i+1]);
                particleData.positions[i][2] = static_cast<float>(atomData.x[3*i+2]);
                particleData.bitmask[i] = atomData.mask[i];
                particleData.types[i] = atomData.type[i];
                particleData.ids[i] = atomData.id[i];
                particleData.visible[i] = true;
                particleData.colors[i] = QVector3D(1.0, 0.0, 0.0);
                particleData.radii[i] = 0.3;
            }
        }
    }
}

static void createSphereBufferData(const QMap<Qt3DCore::QNodeId, QPair<bool, ParticleData>> &pendingParticleData, QMap<Qt3DCore::QNodeId, QPair<bool, QByteArray>> &sphereBufferData) {
    for (auto nodeId : pendingParticleData.keys()) {
        if (pendingParticleData.contains(nodeId) && pendingParticleData[nodeId].first) {
            const ParticleData &particleData = pendingParticleData[nodeId].second;
            if (!sphereBufferData.contains(nodeId)) {
                sphereBufferData[nodeId] = {true, QByteArray()};
            }

            sphereBufferData[nodeId].first = true;
            QByteArray &data = sphereBufferData[nodeId].second;
            data.resize(particleData.size * sizeof(SphereVBOData));

            SphereVBOData *vboData = reinterpret_cast<SphereVBOData *>(data.data());
            //#ifdef __GNUC__
            //#pragma GCC ivdep
            //#endif
#pragma omp simd
            for(size_t i=0; i<particleData.size; i++) {
                SphereVBOData &vbo = vboData[i];

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
    }
}

static void setSphereBufferOnControllers(QMap<Qt3DCore::QNodeId, QPair<bool, QByteArray>> &sphereBufferData, const LAMMPSControllerMapper &mapper) {
    for (const auto &nodeId : sphereBufferData.keys()) {
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
        LambdaJob(std::function<void()> callable) : m_callable(callable) { }

    private:
        const std::function<void()> m_callable;
        void run() override { m_callable(); }
    };

    using LambdaJobPtr = QSharedPointer<LambdaJob>;

    auto job1 = LambdaJobPtr::create([&]() {
        copyDataFromLAMMPS(m_mapper->controllers(), m_pendingRawData);
    });

    auto job2 = LambdaJobPtr::create([&]() {
        convertData(m_pendingRawData, m_pendingParticleData);
    });

    auto job3 = LambdaJobPtr::create([&]() {
        createSphereBufferData(m_pendingParticleData, m_sphereBufferData);
    });

    auto job4 = LambdaJobPtr::create([&]() {
        setSphereBufferOnControllers(m_sphereBufferData, *m_mapper);
    });

    auto job5 = LambdaJobPtr::create([&]() {
        for (auto &e : m_pendingParticleData) {
            e.first = false;
        }
        for (auto &e : m_pendingRawData) {
            e.first = false;
        }
        for (auto &e : m_sphereBufferData) {
            e.first = false;
        }
    });

    job2->addDependency(job1);
    job3->addDependency(job2);
    job4->addDependency(job3);
    job5->addDependency(job4);

    return {job1, job2, job3, job4, job5};
}

}

QT3D_REGISTER_NAMESPACED_ASPECT("lammps", QT_PREPEND_NAMESPACE(atomify), LAMMPSAspect)
