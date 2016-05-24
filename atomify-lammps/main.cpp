// #include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QQuickView>
#ifdef Q_OS_LINUX
#include <locale>
#endif

#include "mysimulator.h"
#include "highlighter.h"
#include "LammpsWrappers/simulatorcontrols/cpcompute.h"
#include "LammpsWrappers/simulatorcontrols/cpvariable.h"
#include "atomstyle.h"
#include "LammpsWrappers/simulatorcontrols/nvt.h"
#include "scripthandler.h"
#include "LammpsWrappers/simulatorcontrols/fixaveragetime.h"
#include "LammpsWrappers/simulatorcontrols/fixhistogram.h"
#include "LammpsWrappers/simulatorcontrols/rdf.h"
#include "vendor.h"
#include "datasource.h"
#include "LammpsWrappers/atoms.h"
#include "LammpsWrappers/system.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<AtomifySimulator>("Atomify", 1, 0, "AtomifySimulator");
    qmlRegisterType<HighlighterWrapper>("Atomify", 1, 0, "Highlighter");
    qmlRegisterType<ScriptHandler>("Atomify", 1, 0, "ScriptHandler");
    qmlRegisterType<CPCompute>("Atomify", 1, 0, "Compute");
    qmlRegisterType<CPFix>("Atomify", 1, 0, "Fix");
    qmlRegisterType<CPVariable>("Atomify", 1, 0, "Variable");
    qmlRegisterType<NVT>("Atomify", 1, 0, "NVT");
    qmlRegisterType<FixAverageTime>("Atomify",1, 0,"FixAverageTime");
    qmlRegisterType<FixHistogram>("Atomify", 1, 0, "FixHistogram");
    qmlRegisterType<RDF>("Atomify", 1, 0, "RDF");
    qmlRegisterType<DataSource>("Atomify", 1, 0, "DataSource");
    qmlRegisterType<Atoms>("Atomify", 1, 0, "Atoms");
    qmlRegisterType<System>("Atomify", 1, 0, "System");

    // QGuiApplication app(argc, argv);
    QApplication app(argc, argv);
    app.setOrganizationName("Ovilab");
    app.setOrganizationDomain("ovilab");
    app.setApplicationName("Atomify");

    QQuickView view;
    qpm::init(app, *view.engine());
    QSurfaceFormat format;
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    view.setFormat(format);
    view.setSource(QUrl("qrc:/main.qml"));
    // view.setSource(QUrl("qrc:/main_spherestmp.qml"));
    view.setWidth(1000);
    view.setHeight(1000);
    view.show();

#ifdef Q_OS_LINUX
    setlocale(LC_ALL, "C");
    setlocale(LC_NUMERIC, "C");
#endif

    for(QKeySequence k : QKeySequence::keyBindings(QKeySequence::FullScreen)) {
        qDebug() << "Use " << k.toString() << " to toggle fullscreen.";
    }

    return app.exec();
}
