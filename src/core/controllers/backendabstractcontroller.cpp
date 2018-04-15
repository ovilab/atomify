#include "backendabstractcontroller.h"

namespace atomify {

BackendAbstractController::BackendAbstractController()
    : Qt3DCore::QBackendNode(Qt3DCore::QBackendNode::ReadWrite)
{
}

} // namespace atomify
