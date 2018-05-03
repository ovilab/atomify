#ifndef CORE_CONTROLLERS_BACKENDLAMMPSCONTROLLER_H
#define CORE_CONTROLLERS_BACKENDLAMMPSCONTROLLER_H

#include "core/controllers/backendabstractcontroller.h"
#include "core/data/lammps/lammpsdata.h"
#include "core/data/particledata.h"
#include "lammpsthread.h"

#include <QBackendNode>
#include <QFuture>

namespace atomify {

class BackendLAMMPSController : public BackendAbstractController {
public:
    BackendLAMMPSController();
    QFuture<Value<ParticleData>> fetchParticleData() override;
    Qt3DCore::QNodeId spheresBufferId() const;
    void notifySphereBuffer(const QByteArray& buffer, uint64_t sphereCount);

protected:
    //    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e) override;

private:
    //    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change) override;
    QScopedPointer<class LAMMPSThread> m_thread;
    Value<LAMMPSData> m_rawData;
};
} // namespace atomify

#endif // CORE_CONTROLLERS_BACKENDLAMMPSCONTROLLER_H
