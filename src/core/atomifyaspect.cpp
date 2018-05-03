#include "atomifyaspect.h"
#include "core/controllers/backendlammpscontroller.h"
#include "core/controllers/lammpscontroller.h"
#include "core/visualization/atomify.h"
#include <SimVis/SphereData>

#include <QAbstractAspect>
#include <QAspectJob>
#include <QPropertyUpdatedChange>
#include <QtConcurrent/QtConcurrentRun>

namespace atomify {

AtomifyAspect::AtomifyAspect(QObject* parent)
    : Qt3DCore::QAbstractAspect(parent)
{
    // Register the mapper to handle creation, lookup, and destruction of backend nodes
    m_lammpsMapper = QSharedPointer<Mapper<BackendLAMMPSController>>::create(this);
    m_atomifyMapper = QSharedPointer<Mapper<BackendAtomify>>::create(this);

    registerBackendType<LAMMPSController>(m_lammpsMapper);
    registerBackendType<Atomify>(m_atomifyMapper);
}

Value<QByteArray> createSphereBufferData(Value<ParticleData> particleData)
{
    qDebug() << "Create sphere buffer data";

    Value<QByteArray> sphereBufferData;
    sphereBufferData->resize(particleData->size * sizeof(SphereVBOData));

    SphereVBOData* vboData = reinterpret_cast<SphereVBOData*>(sphereBufferData->data());
    // TODO(anders.hafreager) SIMDIFY
    for (size_t i = 0; i < particleData->size; i++) {
        SphereVBOData& vbo = vboData[i];

        const int id = particleData->ids[i];
        vbo.position = particleData->positions[i];
        vbo.color[0] = 1.0;
        vbo.color[1] = 0.0;
        vbo.color[2] = 0.0;
        vbo.radius = 0.3;
        vbo.particleId = id;
        vbo.flags = 0; // TODO add back
        //        vbo.flags = m_selectedParticles.contains(particleId) ? Selected : 0;
    }

    return sphereBufferData;
}

struct AtomifySynchronizationJob : public Qt3DCore::QAspectJob {
    BackendAtomify* atomify = nullptr;
    BackendAbstractController* controller = nullptr;
//    QByteArray m_sphereBufferData;
    QFuture<Value<ParticleData>> m_particleData;
    QFuture<Value<QByteArray>> m_sphereBufferData;

    void run() override
    {
        // if job not launched, launch it
        // check if job finished
        // if job finished, start createSphereBufferDataJob
        // check if createSphereBufferDataJob finished

        if (m_particleData.isCanceled()) {
            m_particleData = controller->fetchParticleData();
        }

        // TODO consider waiting a few ms, or maybe not...
        if (m_particleData.isResultReadyAt(0)) {
            qDebug() << "Running sphere buffer data";
            m_sphereBufferData = QtConcurrent::run(&createSphereBufferData, m_particleData.result());
            m_particleData = {};
        }

        if (m_sphereBufferData.isResultReadyAt(0)) {
            qDebug() << "Sphere buffer data was finished";
            const auto &sphereBufferData = m_sphereBufferData.result();
            uint64_t sphereCount = sphereBufferData->size() / sizeof(SphereVBOData);
            atomify->notifySphereBuffer(*sphereBufferData, sphereCount);
        }
    }
};

using AtomifySynchronizationJobPtr = QSharedPointer<AtomifySynchronizationJob>;

QVector<Qt3DCore::QAspectJobPtr> AtomifyAspect::jobsToExecute(qint64 time)
{
    if (m_job == nullptr) {
        m_job = AtomifySynchronizationJobPtr::create();
        m_job->atomify = m_atomifyMapper->controller();
        m_job->controller = m_lammpsMapper->controller();
    }
    return { m_job };
}

} // namespace atomify

QT3D_REGISTER_NAMESPACED_ASPECT("atomify", QT_PREPEND_NAMESPACE(atomify), AtomifyAspect)
