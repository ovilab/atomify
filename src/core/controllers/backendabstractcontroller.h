#ifndef BACKENDABSTRACTCONTROLLER_H
#define BACKENDABSTRACTCONTROLLER_H

#include "core/data/lammps/lammpsdata.h"
#include "core/data/particledata.h"

#include <QBackendNode>

namespace atomify {

class BackendAbstractController : public Qt3DCore::QBackendNode {
public:
    BackendAbstractController();
    virtual bool synchronize() = 0;
    virtual const ParticleData& createParticleData() = 0;

protected:
    ParticleData m_particleData;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr& e) override {}

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr& change) override {}
};
}

#endif // BACKENDABSTRACTCONTROLLER_H
