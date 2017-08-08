#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQuickView>
#include <QSurfaceFormat>

#include "cylindersimulator.h"

#include <vendor.h>

int main(int argc, char *argv[])
{
    qmlRegisterType<CylinderSimulator>("CylinderSimulator", 1, 0, "CylinderSimulator");
    QApplication app(argc, argv);

    QQuickView view;
    qpm::init(app, *view.engine());
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    view.setFormat(format);
    view.setSource(QUrl("qrc:/main_cylinders.qml"));
    view.show();

    return app.exec();
}
