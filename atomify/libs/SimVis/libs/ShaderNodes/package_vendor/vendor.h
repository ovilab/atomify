#ifndef VENDOR_H
#define VENDOR_H

#include <QQmlEngine>
#include <QCoreApplication>

#include <shadernodesplugin.h>

namespace qpm {

void init(const QCoreApplication &app, QQmlEngine &engine) {
    Q_UNUSED(app);

    // Add qml components
    engine.addImportPath(QStringLiteral("qrc:/"));

    // Add C++ components

    // class:"SimVisPlugin" uri:"SimVis"
    const char* uri = "ShaderNodes";

    ShaderNodesPlugin pluginInstance_ShaderNodesPlugin;
    pluginInstance_ShaderNodesPlugin.registerTypes(uri);
    pluginInstance_ShaderNodesPlugin.initializeEngine(&engine, uri);
}

}

#endif // VENDOR_H
