#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QtQml>

#include "bondsimulator.h"

#include <vendor.h>

int main(int argc, char *argv[])
{
    qmlRegisterType<BondSimulator>("BondSimulator", 1, 0, "BondSimulator");
    QApplication app(argc, argv);

    QQuickView view;
    qpm::init(app, *view.engine());
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    view.setFormat(format);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:/main_bond.qml"));
    view.show();

    return app.exec();
}
