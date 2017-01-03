#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQml>

#include "spheresimulator.h"

#include <vendor.h>

int main(int argc, char *argv[])
{
    // Fixes motion problems, see https://bugreports.qt.io/browse/QTBUG-53165
    qputenv("QSG_RENDER_LOOP", "basic");

    qmlRegisterType<SphereSimulator>("SphereSimulator", 1, 0, "SphereSimulator");
    QApplication app(argc, argv);

    QQuickView view;
    qpm::init(app, *view.engine());
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(1);
    format.setProfile(QSurfaceFormat::CoreProfile);
#ifndef Q_OS_IOS
    view.setFormat(format);
#endif
    view.setSource(QUrl("qrc:/main_spheres.qml"));
    view.show();

    return app.exec();
}
