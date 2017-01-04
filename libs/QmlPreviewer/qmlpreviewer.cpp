#include "qmlpreviewer.h"

#include <QDebug>
#include <QApplication>
#include <QDirIterator>
#include <QRegularExpression>
#include <QProcess>
#include <QResource>
#include <QQmlEngine>
#include <QCryptographicHash>

QmlPreviewer::QmlPreviewer(QApplication &app)
{
    Q_UNUSED(app)
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &QmlPreviewer::reload);
}

void QmlPreviewer::reload()
{
    m_view.engine()->clearComponentCache();

    qDebug() << "Reloading" << m_filePath;

    for(auto qrcPath : m_qrcPaths) {
        QVariantMap map = qrcPath.toMap();
        qDebug() << "Unregistering" << map["rcc"].toString();
        QResource::unregisterResource(map["rcc"].toString(), m_prefix);
    }


    for(auto qrcPath : m_qrcPaths) {
        QVariantMap map = qrcPath.toMap();
        QProcess process;
        process.start("rcc", QStringList()
                      << "-binary" << map["path"].toUrl().toLocalFile()
                      << "-o" << map["rcc"].toString());
        qDebug() << process.readAllStandardError();
        process.waitForFinished();
        qDebug() << process.readAllStandardError();

        qDebug() << "Registering" << map["rcc"].toString();

        QResource::registerResource(map["rcc"].toString(), m_prefix);

        QDirIterator it(QString(":" + m_prefix), QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString next = it.next();
            QString relativeFilePath = next;
            QUrl qrcDirectory = map["path"].toUrl().adjusted(QUrl::RemoveFilename);
            relativeFilePath = relativeFilePath.replace(":/qtqmlpreview/", "");
            qDebug() << "Path" << qrcDirectory;
            qDebug() << "Relative" << relativeFilePath;
            QUrl result = qrcDirectory.resolved(QUrl(relativeFilePath));
            qDebug() << "Adding path" << result.toLocalFile();
            m_watcher.addPath(result.toLocalFile());
        }

        qDebug() << "Watching" << m_watcher.files().count() << "files";
    }

    QMetaObject::invokeMethod(m_rootItem, "reload");
}

void QmlPreviewer::handleDialogStart(QVariant qrcPaths, QUrl filePath)
{
    qDebug() << "Handle dialog start";

    for(auto qrcPath : m_qrcPaths) {
        QVariantMap map = qrcPath.toMap();
        qDebug() << "Unregistering" << map["rcc"].toString();
        QResource::unregisterResource(map["rcc"].toString(), m_prefix);
    }

    QVariantList paths = qrcPaths.toList();
    QUrl projectPath;
    m_qrcPaths.clear();
    for(QVariant path : paths) {
        QUrl pathUrl = path.toUrl();
        QString hash = QCryptographicHash::hash(pathUrl.toString().toLatin1(), QCryptographicHash::Md5).toBase64().replace("=", "").replace("/", "").replace("\\", "");
        QVariantMap map{
            {"path", pathUrl},
            {"rcc", hash + QString(".rcc")},
            {"hash", hash}
        };
        m_qrcPaths.append(map);
        projectPath = path.toUrl().adjusted(QUrl::RemoveFilename);
    }
    m_filePath = filePath.toLocalFile();
    reload();
}

void QmlPreviewer::show()
{
    m_view.setSource(QUrl("qrc:///QmlPreviewer/QmlPreviewerDialog.qml"));
    m_view.setResizeMode(QQuickView::SizeRootObjectToView);
    m_view.show();

    m_rootItem = m_view.rootObject();
    connect(m_rootItem, SIGNAL(start(QVariant, QUrl)), this, SLOT(handleDialogStart(QVariant, QUrl)));
}
