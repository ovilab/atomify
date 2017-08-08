#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <cmath>
#include "functionsurfacesimulator.h"

#include <QQuickView>
#include <vendor.h>

int main(int argc, char *argv[])
{
    qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");
    QApplication app(argc, argv);

    QQuickView view;
    qpm::init(app, *view.engine());
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    view.setFormat(format);
    view.setSource(QUrl("qrc:/main_functionsurface.qml"));
    view.show();

    return app.exec();
}
