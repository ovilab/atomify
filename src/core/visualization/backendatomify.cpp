#include "backendatomify.h"
#include "atomify.h"

#include <QPropertyUpdatedChange>

#include <exception>

namespace atomify {

BackendAtomify::BackendAtomify()
{
}

void BackendAtomify::sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e)
{
}

void BackendAtomify::initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change)
{
    const auto typedChange = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<AtomifyData>>(change);
    const auto& data = typedChange->data;
    m_spheresBufferId = data.spheresBufferId;
    m_spheresGeometryRendererId = data.spheresGeometryRendererId;
    m_controllerId = data.controllerId;
}

void BackendAtomify::notifySphereBuffer(const QByteArray& buffer, uint64_t sphereCount)
{
    {
        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(m_spheresBufferId);
        change->setPropertyName("data");
        change->setValue(QVariant::fromValue(buffer));
        notifyObservers(change);
    }

    {
        auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(m_spheresGeometryRendererId);
        change->setPropertyName("instanceCount");
        change->setValue(static_cast<int>(sphereCount));
        notifyObservers(change);
    }
}

} // namespace atomify
