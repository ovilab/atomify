#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H

#include <QComponent>

namespace atomify {

class LAMMPSController : public Qt3DCore::QComponent {
    Q_OBJECT
public:
    explicit LAMMPSController(Qt3DCore::QNode* parent = nullptr);

private:
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
    virtual void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& change) override;
};
}

#endif // LAMMPSCONTROLLER_H
