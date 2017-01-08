// #include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QQuickView>
#include <QmlPreviewer>
#ifdef Q_OS_LINUX
#include <locale>
#endif
#include "mousemover.h"
#include "mysimulator.h"
#include "highlighter.h"
#include "LammpsWrappers/simulatorcontrols/cpcompute.h"
#include "LammpsWrappers/simulatorcontrols/cpfix.h"
#include "LammpsWrappers/simulatorcontrols/cpvariable.h"
#include "LammpsWrappers/simulatorcontrols/nvt.h"
#include "LammpsWrappers/simulatorcontrols/rdf.h"
#include "performance.h"
#include "vendor.h"
#include "datasource.h"
#include "LammpsWrappers/atoms.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/groups.h"
#include "LammpsWrappers/fixes.h"
#include "LammpsWrappers/regions.h"
#include "LammpsWrappers/modifiers/modifiers.h"
#include "LammpsWrappers/computes.h"
#include "LammpsWrappers/variables.h"
#include "LammpsWrappers/units.h"
#include "dataproviders/data1d.h"
#include "dataproviders/data2d.h"
#include "dataproviders/dataprovider.h"
#include "linenumbers.h"
#include "windowgl2.h"
#include "codeeditorbackend.h"
#include "states.h"
#include "parsefileuploader.h"
#include "standardpaths.h"
#include "keysequence.h"

void copyExamplesToLocalFolder()
{
    QString dataDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dataDir(dataDirPath);
    QDir rootQrcFolder(":/");
    QDirIterator exampleIterator(":/examples", QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);
    while(exampleIterator.hasNext()) {
        const QString &qrcFileName = exampleIterator.next();
        QFileInfo qrcFileInfo(qrcFileName);
        QString qrcDirPath = qrcFileInfo.dir().absolutePath();
        QString relativeDirPath = rootQrcFolder.relativeFilePath(qrcDirPath);
        QString relativeFilePath = rootQrcFolder.relativeFilePath(qrcFileName);

        QString targetDirPath = dataDir.absoluteFilePath(relativeDirPath);
        QDir targetDir(targetDirPath);
        if(!targetDir.exists()) {
            bool pathCreated = targetDir.mkpath(".");
            if(!pathCreated) {
                qWarning() << "Could not create" << targetDir.absolutePath();
                continue;
            }
        }

        QString targetPath = dataDir.absoluteFilePath(relativeFilePath);

        if(QFile(targetPath).exists()) {
            continue;
        }

        QFile file(qrcFileName);
        bool fileCopied = file.copy(targetPath);
        if(!fileCopied) {
            qWarning() << "Could not copy" << qrcFileName << "to" << targetPath;
            continue;
        }
        QFile targetFile(targetPath);
        targetFile.setPermissions(targetFile.permissions() | QFile::WriteOwner | QFile::WriteUser);
    }
}

int main(int argc, char *argv[])
{
    if(argc>1) {
        if(strcmp(argv[1], "--cleardata")==0) {
            QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/Ovilab/Atomify";
            qDebug() << "Dir: " << dir;

            QDir dataDir(dir);
            dataDir.removeRecursively();
            return 0;
        }
    }
    qmlRegisterType<AtomifySimulator>("Atomify", 1, 0, "AtomifySimulator");
    qmlRegisterType<CPCompute>("Atomify", 1, 0, "Compute");
    qmlRegisterType<CPFix>("Atomify", 1, 0, "Fix");
    qmlRegisterType<CPVariable>("Atomify", 1, 0, "Variable");
    qmlRegisterType<NVT>("Atomify", 1, 0, "NVT");
    qmlRegisterType<RDF>("Atomify", 1, 0, "RDF");
    qmlRegisterType<DataSource>("Atomify", 1, 0, "DataSource");
    qmlRegisterType<Atoms>("Atomify", 1, 0, "Atoms");
    qmlRegisterType<System>("Atomify", 1, 0, "System");
    qmlRegisterType<Units>("Atomify", 1, 0, "Units");
    qmlRegisterType<Groups>("Atomify", 1, 0, "Groups");
    qmlRegisterType<Computes>("Atomify", 1, 0, "Computes");
    qmlRegisterType<Variables>("Atomify", 1, 0, "Variables");
    qmlRegisterType<Fixes>("Atomify", 1, 0, "Fixes");
    qmlRegisterType<Regions>("Atomify", 1, 0, "Regions");
    qmlRegisterType<DataProvider>("Atomify", 1, 0, "DataProvider");
    qmlRegisterType<Data1D>("Atomify", 1, 0, "Data1D");
    qmlRegisterType<Data2D>("Atomify", 1, 0, "Data2D");

    qmlRegisterType<ColorModifier>("Atomify", 1, 0, "ColorModifier");
    qmlRegisterType<PeriodicImages>("Atomify", 1, 0, "PeriodicImages");
    qmlRegisterType<GroupModifier>("Atomify", 1, 0, "GroupModifier");
    qmlRegisterType<RegionModifier>("Atomify", 1, 0, "RegionModifier");
    qmlRegisterType<PropertyModifier>("Atomify", 1, 0, "PropertyModifier");
    qmlRegisterType<Slice>("Atomify", 1, 0, "Slice");

    qmlRegisterType<LineNumbers>("Atomify", 1, 0, "LineNumbers");
    qmlRegisterType<CodeEditorBackend>("Atomify", 1, 0, "CodeEditorBackend");
    qmlRegisterType<HighlighterWrapper>("Atomify", 1, 0, "Highlighter");
    qmlRegisterType<WindowGL2>("Atomify", 1, 0, "WindowGL2");
    qmlRegisterType<MouseMover>("Atomify", 1, 0, "MouseMover");
    qmlRegisterType<States>("Atomify", 1, 0, "States");
    qmlRegisterType<Performance>("Atomify", 1, 0, "Performance");

    qmlRegisterType<ParseFileUploader>("Atomify", 1, 0, "ParseFileUploader");

    qmlRegisterSingletonType<StandardPaths>("StandardPaths", 1, 0, "StandardPaths", &StandardPaths::qmlInstance);

    qmlRegisterSingletonType<KeySequence>("KeySequence", 1, 0, "KeySequence", &KeySequence::qmlInstance);

    // QGuiApplication app(argc, argv);
    QApplication app(argc, argv);
    app.setOrganizationName("Ovilab");
    app.setOrganizationDomain("ovilab");
    app.setApplicationName("Atomify");

    QSurfaceFormat format;
#ifndef Q_OS_ANDROID
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    // format.setSamples(32);
#endif
    QSurfaceFormat::setDefaultFormat(format);

    copyExamplesToLocalFolder();

    // Application version
    QQmlApplicationEngine engine;
    QmlPreviewer previewer(app);
    if(argc > 2) {
        previewer.show();
    } else {
        qpm::init(app, engine);
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

#ifdef Q_OS_LINUX
        setlocale(LC_ALL, "C");
        setlocale(LC_NUMERIC, "C");
#endif

        for(QKeySequence k : QKeySequence::keyBindings(QKeySequence::FullScreen)) {
            qDebug() << "Use " << k.toString() << " to toggle fullscreen.";
        }
    }

    return app.exec();
}
