#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#ifdef Q_OS_LINUX
#include <locale>
#endif

#include "mysimulator.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

#ifdef Q_OS_LINUX
    setlocale(LC_ALL, "C");
    setlocale(LC_NUMERIC, "C");
#endif
    return app.exec();
}
