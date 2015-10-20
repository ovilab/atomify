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

int main(int argc, char *argv[])
{
    qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");
    qmlRegisterType<HighlighterWrapper>("Highlighter", 1, 0, "Highlighter");
    qmlRegisterType<CPCompute>("Compute", 1, 0, "Compute");
    qmlRegisterType<LammpsOutput>("LammpsOutput", 1, 0, "LammpsOutput");
    qmlRegisterType<AtomStyle>("AtomStyle", 1, 0, "AtomStyle");
    // qmlRegisterType<FileIO>("Atomify", 1, 0, "FileIO");
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QSurfaceFormat format;
#if !defined(Q_OS_ANDROID)
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(3);
#endif

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    for(auto &obj : engine.rootObjects()) {
        qDebug() << "Obj: " << obj;
        qDebug() << "Window type: " << obj->isWindowType();

        QQuickWindow *window = qobject_cast<QQuickWindow*>(obj);
        if(window != NULL) {
            window->setFormat(format);
        } else qDebug() << "No :(((";
    }
#ifdef Q_OS_LINUX
    setlocale(LC_ALL, "C");
    setlocale(LC_NUMERIC, "C");
#endif

    return app.exec();
}
