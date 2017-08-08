#ifndef VENDOR_H
#define VENDOR_H

#include <QQmlEngine>
#include <QCoreApplication>

#include "core/simvisplugin.h"
#include "../libs/ShaderNodes/src/shadernodesplugin.h"

namespace qpm {

void init(const QCoreApplication &app, QQmlEngine &engine) {
    Q_UNUSED(app);

    // Add qml components
    engine.addImportPath(QStringLiteral("qrc:/"));

    // Add C++ components

    // class:"SimVisPlugin" uri:"SimVis"
    SimVisPlugin pluginInstance_SimVisPlugin;
    pluginInstance_SimVisPlugin.registerTypes("SimVis");
    pluginInstance_SimVisPlugin.initializeEngine(&engine, "SimVis");

    ShaderNodesPlugin pluginInstance_ShaderNodesPlugin;
    pluginInstance_ShaderNodesPlugin.registerTypes("ShaderNodes");
    pluginInstance_ShaderNodesPlugin.initializeEngine(&engine, "ShaderNodes");

}

}


#endif // VENDOR_H
