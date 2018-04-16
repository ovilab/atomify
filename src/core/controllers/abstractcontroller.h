#ifndef CORE_CONTROLLERS_ABSTRACTCONTROLLER_H
#define CORE_CONTROLLERS_ABSTRACTCONTROLLER_H

#include <Qt3DCore/QComponent>

namespace atomify {

class AbstractController : public Qt3DCore::QComponent {
    Q_OBJECT
public:
    explicit AbstractController(Qt3DCore::QNode* parent = nullptr)
        : Qt3DCore::QComponent(parent)
    {
    }

signals:

public slots:
};

} // namespace atomify

#endif // CORE_CONTROLLERS_ABSTRACTCONTROLLER_H
