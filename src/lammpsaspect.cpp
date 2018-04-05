#include "lammpsaspect.h"
#include "backendlammpscontroller.h"
#include "lammpscontroller.h"
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

        pendingRawData[controller->peerId()].first = !data.empty;
        pendingRawData[controller->peerId()].second = std::move(data);
    }
}

static void convertData(const QList<class BackendLAMMPSController*> &controllers, const QMap<Qt3DCore::QNodeId, QPair<bool, LAMMPSData>> &pendingRawData, QMap<Qt3DCore::QNodeId, QPair<bool, ParticleData>> &pendingParticleData) {
    for (const auto &controller : controllers) {
        if (pendingRawData.contains(controller->peerId()) && pendingRawData[controller->peerId()].first) {
            if (!pendingParticleData.contains(controller->peerId())) {
                pendingParticleData[controller->peerId()] = { true, ParticleData() };
            }

            pendingParticleData[controller->peerId()].first = true;
            auto &particleData = pendingParticleData[controller->peerId()].second;

            auto data = pendingRawData[controller->peerId()];
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

static void createSphereBufferData(const QList<class BackendLAMMPSController*> &controllers, const QMap<Qt3DCore::QNodeId, QPair<bool, ParticleData>> &pendingParticleData, QMap<Qt3DCore::QNodeId, QPair<bool, QByteArray>> &sphereBufferData) {
    for (const auto &controller : controllers) {
        if (pendingParticleData.contains(controller->peerId()) && pendingParticleData[controller->peerId()].first) {
            const ParticleData &particleData = pendingParticleData[controller->peerId()].second;
            if (!sphereBufferData.contains(controller->peerId())) {
                sphereBufferData[controller->peerId()] = {true, QByteArray()};
            }

            sphereBufferData[controller->peerId()].first = true;
            QByteArray &data = sphereBufferData[controller->peerId()].second;
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
    QVector<Qt3DCore::QAspectJobPtr> jobs;

    auto job1 = LambdaJobPtr::create([&]() {
        copyDataFromLAMMPS(m_mapper->controllers(), m_pendingRawData);
    });

    auto job2 = LambdaJobPtr::create([&]() {
        convertData(m_mapper->controllers(), m_pendingRawData, m_pendingParticleData);
    });

    auto job3 = LambdaJobPtr::create([&]() {
        createSphereBufferData(m_mapper->controllers(), m_pendingParticleData, m_sphereBufferData);
    });

    auto job4 = LambdaJobPtr::create([&]() {
        for (const auto &nodeId : m_sphereBufferData.keys()) {
            if (!m_sphereBufferData[nodeId].first) {
                continue;
            }
            const auto controller = dynamic_cast<BackendLAMMPSController*>(m_mapper->get(nodeId));
            uint64_t sphereCount = m_sphereBufferData[nodeId].second.size() / sizeof(SphereVBOData);
            controller->setSphereBufferData(m_sphereBufferData[nodeId].second, sphereCount);
        }
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

    jobs.append(job1);
    jobs.append(job2);
    jobs.append(job3);
    jobs.append(job4);
    jobs.append(job5);

    return jobs;
}

}

QT3D_REGISTER_NAMESPACED_ASPECT("lammps", QT_PREPEND_NAMESPACE(atomify), LAMMPSAspect)
