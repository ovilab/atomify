#include "lammpscontroller.h"

#include <QDebug>
#include <Qt3DCore/QPropertyUpdatedChangePtr>

namespace atomify {

LAMMPSController::LAMMPSController(Qt3DCore::QNode* parent)
    : Qt3DCore::QComponent(parent)
{
    qDebug() << "Creating LAMMPSController with id " << this->id();
}

void LAMMPSController::sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e)
{
    //    const auto change = qSharedPointerCast<Qt3DCore::QPropertyUpdatedChange>(e);
    //    if (change->propertyName() == QByteArrayLiteral("visibleAtomCount")) {
    //        const int visibleAtoms = change->value().value<int>();
    //        if (m_visibleAtomCount != visibleAtoms) {
    //            m_visibleAtomCount = visibleAtoms;
    //            emit visibleAtomCountChanged();
    //        }
    //    }
}

//Qt3DCore::QNodeCreatedChangeBasePtr LAMMPSController::createNodeCreationChange() const
//{
//    auto creationChange = Qt3DCore::QNodeCreatedChangePtr<LAMMPSControllerData>::create(this);
//    auto& data = creationChange->data;
//    data.spheresBufferId = m_spheresBuffer->id();
//    return creationChange;
//}

} // namespace atomify
