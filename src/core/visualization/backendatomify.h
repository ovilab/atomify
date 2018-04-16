#ifndef CORE_VISUALIZATION_BACKENDATOMIFY_H
#define CORE_VISUALIZATION_BACKENDATOMIFY_H

#include "core/data/lammps/lammpsdata.h"
#include "core/data/particledata.h"

#include <QBackendNode>
#include <QHash>
#include <experimental/optional>

namespace atomify {

class BackendAtomify : public Qt3DCore::QBackendNode {
public:
    BackendAtomify();
    void notifySphereBuffer(const QByteArray& buffer, uint64_t sphereCount);

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e) override;

private:
    Qt3DCore::QNodeId m_spheresBufferId;
    Qt3DCore::QNodeId m_spheresGeometryRendererId;
    Qt3DCore::QNodeId m_controllerId;

    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change) override;
};

} // namespace atomify

#endif // CORE_VISUALIZATION_BACKENDATOMIFY_H
