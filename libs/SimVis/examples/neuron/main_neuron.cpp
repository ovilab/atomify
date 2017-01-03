#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QQuickView>
#include <QDebug>

#include "neuronreader.h"
#include "instancebuffer.h"

#include <vendor.h>

int main(int argc, char *argv[])
{
    qDebug() << "Starting neuron example";

    qmlRegisterType<NeuronReader>("NeuroML", 1, 0, "NeuronReader");
    QApplication app(argc, argv);

    qDebug() << "Opening view";

    QQuickView view;
    qpm::init(app, *view.engine());

    InstanceBuffer buffer;
    view.engine()->rootContext()->setContextProperty("_instanceBuffer", &buffer);
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    view.setResizeMode(QQuickView::ResizeMode::SizeRootObjectToView);
    view.setFormat(format);
    qDebug() << "Setting source";
    view.setSource(QUrl("qrc:/main_neuron.qml"));
    qDebug() << "Showing view";
    view.show();
    qDebug() << "Exec app";


    return app.exec();
}
