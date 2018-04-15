#ifndef BACKENDATOMIFY_H
#define BACKENDATOMIFY_H

#include "core/data/lammps/lammpsdata.h"
#include "core/data/particledata.h"

#include <QBackendNode>
#include <QHash>
#include <experimental/optional>

namespace atomify {

class BackendAtomify : public Qt3DCore::QBackendNode {
public:
    BackendAtomify();

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e) override;

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change) override;
};

} // namespace atomify

#endif // BACKENDATOMIFY_H
