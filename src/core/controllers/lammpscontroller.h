#ifndef CORE_CONTROLLERS_LAMMPSCONTROLLER_H
#define CORE_CONTROLLERS_LAMMPSCONTROLLER_H

#include "abstractcontroller.h"

namespace atomify {

class LAMMPSController : public AbstractController {
    Q_OBJECT
public:
    explicit LAMMPSController(Qt3DCore::QNode* parent = nullptr);

private:
    // Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
    virtual void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& change) override;
};

} // namespace atomify

#endif // CORE_CONTROLLERS_LAMMPSCONTROLLER_H
