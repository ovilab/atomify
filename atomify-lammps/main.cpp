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
#include "LammpsWrappers/simulatorcontrols/nvt.h"
#include "scripthandler.h"
#include "LammpsWrappers/simulatorcontrols/fixaveragetime.h"
#include "LammpsWrappers/simulatorcontrols/fixhistogram.h"
#include "LammpsWrappers/simulatorcontrols/rdf.h"
#include "vendor.h"
#include "datasource.h"
#include "LammpsWrappers/atoms.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/groups.h"
#include "LammpsWrappers/regions.h"
#include "LammpsWrappers/modifiers/modifiers.h"
#include "LammpsWrappers/computes.h"
#include "LammpsWrappers/units.h"
#include "dataproviders/cp1ddata.h"
#include "dataproviders/dataprovider.h"
#include "linenumbers.h"
#include "codeeditorbackend.h"
int main(int argc, char *argv[])
{
    qmlRegisterType<AtomifySimulator>("Atomify", 1, 0, "AtomifySimulator");
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
    qmlRegisterType<Units>("Atomify", 1, 0, "Units");
    qmlRegisterType<Groups>("Atomify", 1, 0, "Groups");
    qmlRegisterType<Computes>("Atomify", 1, 0, "Computes");
    qmlRegisterType<Regions>("Atomify", 1, 0, "Regions");
    qmlRegisterType<DataProvider>("Atomify", 1, 0, "DataProvider");
    qmlRegisterType<CP1DData>("Atomify", 1, 0, "CP1DData");

    qmlRegisterType<ColorModifier>("Atomify", 1, 0, "ColorModifier");
    qmlRegisterType<PeriodicImages>("Atomify", 1, 0, "PeriodicImages");
    qmlRegisterType<GroupModifier>("Atomify", 1, 0, "GroupModifier");
    qmlRegisterType<RegionModifier>("Atomify", 1, 0, "RegionModifier");
    qmlRegisterType<Slice>("Atomify", 1, 0, "Slice");

    qmlRegisterType<LineNumbers>("Atomify", 1, 0, "LineNumbers");
    qmlRegisterType<CodeEditorBackend>("Atomify", 1, 0, "CodeEditorBackend");
    qmlRegisterType<HighlighterWrapper>("Atomify", 1, 0, "Highlighter");

    // QGuiApplication app(argc, argv);
    QApplication app(argc, argv);
    app.setOrganizationName("Ovilab");
    app.setOrganizationDomain("ovilab");
    app.setApplicationName("Atomify");

    QSurfaceFormat format;
#ifndef Q_OS_ANDROID
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    // format.setSamples(32);
#endif
    QSurfaceFormat::setDefaultFormat(format);

    // View version
//    QQuickView view;
//    qpm::init(app, *view.engine());
//    view.setFormat(format); // TODO see if this is necessary on all platforms still
//    view.setSource(QUrl("qrc:/main.qml"));
//    view.show();

    // Application version
    QQmlApplicationEngine engine;
    qpm::init(app, engine);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

#ifdef Q_OS_LINUX
    setlocale(LC_ALL, "C");
    setlocale(LC_NUMERIC, "C");
#endif

    for(QKeySequence k : QKeySequence::keyBindings(QKeySequence::FullScreen)) {
        qDebug() << "Use " << k.toString() << " to toggle fullscreen.";
    }

    return app.exec();
}
