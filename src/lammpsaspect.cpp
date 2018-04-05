#include "lammpsaspect.h"
#include "backendlammpscontroller.h"
#include "lammpscontroller.h"
#include <SimVis/SphereData>

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

static void copyDataFromLAMMPS(const QList<class BackendLAMMPSController*> &controllers, QMap<Qt3DCore::QNodeId, LAMMPSData> &pendingRawData) {
    for (const auto &controller : controllers) {
        auto data = controller->synchronize();
        if (!data.empty) {
            pendingRawData[controller->peerId()] = std::move(data);
        }
    }
}

static void convertData(const QList<class BackendLAMMPSController*> &controllers, const QMap<Qt3DCore::QNodeId, LAMMPSData> &pendingRawData, QMap<Qt3DCore::QNodeId, ParticleData> &pendingParticleData) {
    for (const auto &controller : controllers) {
        if (pendingRawData.contains(controller->peerId())) {
            pendingParticleData[controller->peerId()] = ParticleData();
            auto &particleData = pendingParticleData[controller->peerId()];

            auto data = pendingRawData[controller->peerId()];
            const auto &atomData = data.atomData;
            particleData.resize(atomData.size);
            particleData.timestep = data.systemData.ntimestep;

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

static QByteArray createSphereBufferData()

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
        // TODO(anders.hafreager) Instead of clearing, we should reuse these data structures
        // to avoid allocating crazy amounts of memory all the time
        m_pendingParticleData.clear();
        m_pendingRawData.clear();
    });

    job2->addDependency(job1);
    job3->addDependency(job2);

    jobs.append(job1);
    jobs.append(job2);
    jobs.append(job3);

    return jobs;
}

}

QT3D_REGISTER_NAMESPACED_ASPECT("lammps", QT_PREPEND_NAMESPACE(atomify), LAMMPSAspect)
