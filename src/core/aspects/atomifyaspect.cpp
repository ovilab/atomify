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
    m_lammpsMapper = QSharedPointer<Mapper<BackendLAMMPSController>>::create(this);
    m_atomifyMapper = QSharedPointer<Mapper<BackendAtomify>>::create(this);

    registerBackendType<LAMMPSController>(m_lammpsMapper);
    registerBackendType<Atomify>(m_atomifyMapper);
}

static QByteArray createSphereBufferData(const ParticleData& particleData, QByteArray sphereBufferData)
{
    sphereBufferData.resize(particleData.size * sizeof(SphereVBOData));

    SphereVBOData* vboData = reinterpret_cast<SphereVBOData*>(sphereBufferData.data());
    // TODO(anders.hafreager) SIMDIFY
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

    return sphereBufferData;
}

struct AtomifySynchronizationJob : public Qt3DCore::QAspectJob {
    BackendAtomify* atomify = nullptr;
    BackendAbstractController* controller = nullptr;
    QByteArray m_sphereBufferData;

    void run() override
    {
        if (controller->synchronize()) {
            qDebug() << "Did synchronize!!!";
            const auto& particleData = controller->createParticleData();
            qDebug() << "Got particle data with " << particleData.size << " things";
            // particleData = applyModifiers(m_particleData, std::move(m_particleData));
            m_sphereBufferData = createSphereBufferData(particleData, std::move(m_sphereBufferData));
            qDebug() << "Creating sphere buffer data";

            uint64_t sphereCount = m_sphereBufferData.size() / sizeof(SphereVBOData);
            qDebug() << "Setting sphere buffer data";
            atomify->notifySphereBuffer(m_sphereBufferData, sphereCount);
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
