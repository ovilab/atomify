#include "backendlammpscontroller.h"
#include "core/atomifyaspect.h"
#include "lammpscontroller.h"
#include "lammpsthread.h"
#include <QDebug>
#include <QPropertyUpdatedChange>
#include <QtConcurrent/QtConcurrentRun>

namespace atomify {

BackendLAMMPSController::BackendLAMMPSController()
    : BackendAbstractController()
    , m_thread(new LAMMPSThread())
{
    m_thread->start();
}

QFuture<Value<ParticleData>> BackendLAMMPSController::fetchParticleData()
{
    std::function<Value<ParticleData>(LAMMPSThread *)> dataFetcher = [](LAMMPSThread *thread) {
        qDebug() << "Fetching data";
        thread->m_systemDataRequested = true;
        thread->m_atomDataRequested = true;
        thread->m_dataReady.acquire();
        const auto &rawData = thread->m_readyData;

        Value<ParticleData> particleData;
        particleData = particleData.modified([rawData](ParticleData data) {
            const auto &atomData = rawData->atomData;
            const auto &systemData = rawData->systemData;
            data = resize(data, atomData.size);
            data.timestep = systemData.ntimestep;

            // TODO(anders.hafreager) Simdify
            for (int i = 0; i < atomData.size; i++) {
                data.positions[i][0] = static_cast<float>(atomData.x[3 * i + 0]);
                data.positions[i][1] = static_cast<float>(atomData.x[3 * i + 1]);
                data.positions[i][2] = static_cast<float>(atomData.x[3 * i + 2]);
                data.types[i] = atomData.type[i];
                data.ids[i] = atomData.id[i];
                data.visible[i] = true;
                data.colors[i] = QVector3D(1.0, 0.0, 0.0);
                data.radii[i] = 0.3;
            }
            return data;
        });
        return particleData;
    };

    return QtConcurrent::run(dataFetcher, m_thread.data());
}

// void BackendLAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e)
//{
//}

// void BackendLAMMPSController::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr&
// change)
//{
//}

} // namespace atomify
