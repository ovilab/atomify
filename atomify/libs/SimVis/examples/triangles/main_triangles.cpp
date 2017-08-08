#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "trianglessimulator.h"

#include <vendor.h>

int main(int argc, char *argv[])
{
    qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qpm::init(app, engine);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

