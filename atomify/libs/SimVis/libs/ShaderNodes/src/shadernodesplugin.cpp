#include "shadernodesplugin.h"

#include "shaderbuildermaterialbinding.h"
#include "shaderbuilder.h"
#include "shaderbuilderbinding.h"
#include "shadernode.h"
#include "shaderoutput.h"
#include "shaderutils.h"
#include "shaderparameter.h"

#include <qqml.h>
#include <QQmlEngine>
#include <Qt3DQuick/private/quick3dnode_p.h>

static const char* packageUri = "ShaderNodes";

void ShaderNodesPlugin::registerTypes(const char *uri)
{
    Q_INIT_RESOURCE(shadernodes_imports);
    Q_ASSERT(uri == QLatin1String(packageUri));
    qmlRegisterExtendedType<ShaderNode, Qt3DCore::Quick::Quick3DNode>(packageUri, 1, 0, "ShaderNode");
    qmlRegisterType<ShaderBuilder>(packageUri, 1, 0, "ShaderBuilder");
    qmlRegisterType<ShaderOutput>(packageUri, 1, 0, "ShaderOutput");
    qmlRegisterType<ShaderBuilderBinding>(packageUri, 1, 0, "ShaderBuilderBinding");
//    qmlRegisterType<ShaderBuilderMaterialBinding>(packageUri, 1, 0, "ShaderBuilderMaterialBinding");
    qmlRegisterType<ShaderParameter>(packageUri, 1, 0, "ShaderParameter");

    qmlRegisterSingletonType<ShaderUtils>(packageUri, 1, 0, "ShaderUtils", &ShaderUtils::qmlInstance);
}

void ShaderNodesPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}
