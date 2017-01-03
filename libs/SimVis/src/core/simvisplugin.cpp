#include "simvisplugin.h"
#include "simulator.h"
#include "../render/geometry/pointgeometry.h"
#include "../render/geometry/spherespointgeometry.h"
#include "../render/geometry/spheredata.h"
#include "../render/geometry/bonddata.h"
#include "../render/geometry/cylinderdata.h"
#include "../render/geometry/uniformgridgeometry.h"
#include "../utils/marchingcubestabletexture.h"

#include <qqml.h>
#include <QQmlEngine>


void SimVisPlugin::registerTypes(const char *uri)
{
    Q_INIT_RESOURCE(simvis_shaders);
    Q_INIT_RESOURCE(simvis_imports);
    // @uri SimVis
    Q_ASSERT(uri == QLatin1String("SimVis"));
    qmlRegisterUncreatableType<Simulator>("SimVis", 1, 0, "Simulator",
                                          "Cannot create abstract type Simulator. This must be subclassed.");
    qmlRegisterType<PointGeometry>("SimVis", 1, 0, "PointGeometry");
    qmlRegisterType<SpheresPointGeometry>("SimVis", 1, 0, "SpheresPointGeometry");
    qmlRegisterType<UniformGridGeometry>("SimVis", 1, 0, "UniformGridGeometry");
    qmlRegisterType<SphereData>("SimVis", 1, 0, "SphereData");
    qmlRegisterType<CylinderData>("SimVis", 1, 0, "CylinderData");
    qmlRegisterType<BondData>("SimVis", 1, 0, "BondData");
    qmlRegisterType<MarchingCubesTableTexture>("SimVis", 1, 0, "MarchingCubesTableTexture");
}

void SimVisPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}


