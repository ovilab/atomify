#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#ifdef Q_OS_LINUX
#include <locale>
#endif

#include "mysimulator.h"
#include "highlighter.h"
#include "CPcompute.h"
#include "lammpsoutput.h"
#include "atomskin.h"

int main(int argc, char *argv[])
{
    qmlRegisterType<MySimulator>("MySimulator", 1, 0, "MySimulator");
    qmlRegisterType<HighlighterWrapper>("Highlighter", 1, 0, "Highlighter");
    qmlRegisterType<CPCompute>("Compute", 1, 0, "Compute");
    qmlRegisterType<LammpsOutput>("LammpsOutput", 1, 0, "LammpsOutput");
    qmlRegisterType<AtomSkin>("AtomSkin", 1, 0, "AtomSkin");
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

#ifdef Q_OS_LINUX
    setlocale(LC_ALL, "C");
    setlocale(LC_NUMERIC, "C");
#endif
    return app.exec();
}
