#include <QApplication>
#include <QOpenGLContext>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQuickWindow>
#include <QSurfaceFormat>
#include <QmlPreviewer>
#include <QtQml>
#include <mpi.h>

#include "vendor.h"
#include <exceptions.h>
#include <input.h>
#include <lammps.h>
#include <library.h>
#ifdef Q_OS_LINUX
#include <locale>
#endif

void registerQML();

int regularLAMMPS(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    try {
        void* ptr = nullptr;
        lammps_open(argc, argv, MPI_COMM_WORLD, &ptr);
        LAMMPS_NS::LAMMPS* lammps = static_cast<LAMMPS_NS::LAMMPS*>(ptr);
        // LAMMPS *lammps = new LAMMPS(argc,argv,MPI_COMM_WORLD);
        lammps->input->file();
        delete lammps;
    } catch (LAMMPS_NS::LAMMPSAbortException& ae) {
        MPI_Abort(ae.universe, 1);
    } catch (LAMMPS_NS::LAMMPSException& e) {
        MPI_Finalize();
        exit(1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}

void copyFiles(QDirIterator& iterator, QDir& dataDir)
{
    QDir rootQrcFolder(":/");
    while (iterator.hasNext()) {
        const QString& qrcFileName = iterator.next();
        QFileInfo qrcFileInfo(qrcFileName);
        QString qrcDirPath = qrcFileInfo.dir().absolutePath();
        QString relativeDirPath = rootQrcFolder.relativeFilePath(qrcDirPath);
        QString relativeFilePath = rootQrcFolder.relativeFilePath(qrcFileName);

        QString targetDirPath = dataDir.absoluteFilePath(relativeDirPath);
        QDir targetDir(targetDirPath);
        if (!targetDir.exists()) {
            bool pathCreated = targetDir.mkpath(".");
            if (!pathCreated) {
                qWarning() << "Could not create" << targetDir.absolutePath();
                continue;
            }
        }

        QString targetPath = dataDir.absoluteFilePath(relativeFilePath);

        if (QFile(targetPath).exists()) {
            continue;
        }

        QFile file(qrcFileName);
        bool fileCopied = file.copy(targetPath);
        if (!fileCopied) {
            qWarning() << "Could not copy" << qrcFileName << "to" << targetPath;
            continue;
        }
        QFile targetFile(targetPath);
        targetFile.setPermissions(targetFile.permissions() | QFile::WriteOwner | QFile::WriteUser);
    }
}

void copyFilesToLocalFolder()
{
    QString dataDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dataDir(dataDirPath);
    QDirIterator exampleIterator(":/examples", QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);
    copyFiles(exampleIterator, dataDir);
    QDirIterator extrasIterator(":/extras", QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);

    copyFiles(extrasIterator, dataDir);
}

void showDataDir()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    qDebug() << "Atomify contents are found in " << dir;
    exit(0);
}

int main(int argc, char* argv[])
{
    QString initialScriptFilePath;

    if (argc > 1) {
        if (strcmp(argv[1], "--showdatadir") == 0) {
            // We need to create Qt app for this, but now we know that the user does not
            // want to run a regular script.
        } else if (strcmp(argv[1], "--clearcache") == 0) {
            // We need to create Qt app for this, but now we know that the user does not
            // want to run a regular script.
        } else if (strcmp(argv[1], "--version") == 0) {
            printf(ATOMIFYVERSION);
            printf("\n");
            exit(0);
        } else {
            bool dashInFound = false;
            bool dashGFound = false;
            bool nextIsScript = false;
            for (int i = 1; i < argc; i++) {
                if (nextIsScript) {
                    nextIsScript = false;

                    initialScriptFilePath = QString::fromUtf8(argv[i]);
                    QString fullPathRelativeToCurrentPath = QDir::current().absoluteFilePath(initialScriptFilePath);
                    QFile file(fullPathRelativeToCurrentPath);
                    if (file.exists()) {
                        initialScriptFilePath = QString("file://") + fullPathRelativeToCurrentPath;
                    } else {
                        file.setFileName(QString("file://") + initialScriptFilePath);
                        if (!file.exists()) {
                            qDebug() << "Warning, file " << initialScriptFilePath << " could not be found.";
                            initialScriptFilePath.clear();
                        }
                    }
                }

                if (strcmp(argv[i], "-in") == 0) {
                    dashInFound = true;
                    nextIsScript = true;
                }

                if (strcmp(argv[i], "-g") == 0) {
                    dashGFound = true;
                }
            }

            if (dashInFound && !dashGFound) {
                qDebug() << "Running regular LAMMPS";
                return regularLAMMPS(argc, argv);
            }
        }
    }

    registerQML();
    QApplication app(argc, argv);
    app.setOrganizationName("Ovilab");
    app.setOrganizationDomain("ovilab");

#ifdef BREW
    app.setApplicationName("AtomifyHomebrew");
#else
    app.setApplicationName("Atomify");
#endif
    QSurfaceFormat format;
#ifndef Q_OS_ANDROID
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
#endif
    QSurfaceFormat::setDefaultFormat(format);

    if (argc > 1) {
        if (strcmp(argv[1], "--showdatadir") == 0) {
            showDataDir();
        }

        if (strcmp(argv[1], "--clearcache") == 0) {
            QSettings settings;
            settings.clear();
            exit(0);
        }
    }

    copyFilesToLocalFolder();

    QQmlApplicationEngine engine;
    QmlPreviewer previewer(app);
    qpm::init(app, engine);
    if (false && argc > 2) {
        previewer.show();
    } else {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
        if (engine.rootObjects().size() == 0) {
            qDebug() << "ERROR: Could not load QML";
            return 1;
        }
        QWindow* window = qobject_cast<QWindow*>(engine.rootObjects()[0]);
        if (!window) {
            qDebug() << "ERROR: Could not load window";
            return 1;
        }
        window->setProperty("initialScriptFilePath", initialScriptFilePath);
#ifdef Q_OS_MAC
        window->setIcon(QIcon("../Resources/icon.icns"));
        app.setWindowIcon(QIcon("../Resources/icon.icns"));
#endif
#ifdef Q_OS_LINUX
        setlocale(LC_ALL, "C");
        setlocale(LC_NUMERIC, "C");
#endif

        for (QKeySequence k : QKeySequence::keyBindings(QKeySequence::FullScreen)) {
            qDebug() << "Use " << k.toString() << " to toggle fullscreen.";
        }
    }

    return app.exec();
}
