#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QQuickWindow>
#ifdef Q_OS_LINUX
#include <locale>
#endif

#include "mysimulator.h"
#include "highlighter.h"
#include "CPcompute.h"
#include "lammpsoutput.h"
#include "atomstyle.h"
#include "fileio.h"
#include "nvt.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<AtomifySimulator>("AtomifySimulator", 1, 0, "AtomifySimulator");
    qmlRegisterType<HighlighterWrapper>("Highlighter", 1, 0, "Highlighter");
    qmlRegisterType<CPCompute>("Compute", 1, 0, "Compute");
    qmlRegisterType<LammpsOutput>("LammpsOutput", 1, 0, "LammpsOutput");
    qmlRegisterType<AtomStyle>("AtomStyle", 1, 0, "AtomStyle");
    qmlRegisterType<NVT>("NVT", 1, 0, "NVT");
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
#ifdef Q_OS_LINUX
    setlocale(LC_ALL, "C");
    setlocale(LC_NUMERIC, "C");
#endif

    return app.exec();
}
