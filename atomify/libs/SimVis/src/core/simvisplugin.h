#ifndef SIMVISPLUGIN_H
#define SIMVISPLUGIN_H

#include <QQmlExtensionPlugin>

class SimVisPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
public:
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // SIMVISPLUGIN_H
