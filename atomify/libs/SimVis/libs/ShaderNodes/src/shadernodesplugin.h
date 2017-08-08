#ifndef SHADERNODESPLUGIN_H
#define SHADERNODESPLUGIN_H

#include <QQmlExtensionPlugin>

class ShaderNodesPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
public:
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // SHADERNODESPLUGIN_H
