#include "quickworker.h"
#include "../renderables/billboards/billboards.h"
#include "../renderables/points/points.h"
#include "../renderables/spheres/spheres.h"

QuickWorker::QuickWorker()
{

}

QuickWorker::~QuickWorker()
{

}

void QuickWorker::synchronizeRenderer(Renderable *renderableObject)
{
    Billboards* billboards = qobject_cast<Billboards*>(renderableObject);
    if(billboards) {
        billboards->setPositions(m_positions);
        return;
    }

    Points* points = qobject_cast<Points*>(renderableObject);
    if(points) {
        points->setPositions(m_positions);
        return;
    }

    Spheres* spheres = qobject_cast<Spheres*>(renderableObject);
    if(spheres) {
        spheres->setPositions(m_positions);
        return;
    }
}

