#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <vendor.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qpm::init(app, engine);
    engine.load(QUrl(QLatin1String("qrc:/main_sharednodes.qml")));

    return app.exec();
}
