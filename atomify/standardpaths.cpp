#include "standardpaths.h"

#include <QDebug>
#include <QQmlFile>

StandardPaths::StandardPaths(QObject *parent)
    : QObject(parent)
{

}

QUrl StandardPaths::writableLocation(StandardLocation location, const QString &filename)
{
    QString path = QStandardPaths::writableLocation((QStandardPaths::StandardLocation)location);
    if(!filename.isEmpty()) {
        path += "/" + filename;
    }
    return QUrl::fromLocalFile(path);
}

QUrl StandardPaths::locate(StandardLocation location, const QString &filename)
{
    return QUrl::fromLocalFile(QStandardPaths::locate((QStandardPaths::StandardLocation)location, filename));
}

QString StandardPaths::toLocalFile(QUrl url)
{
    return url.toLocalFile();
}

QObject* StandardPaths::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new StandardPaths;
}

/*!
 * \brief StandardPaths::originalSimulationLocation is a developer helper function to get
 * the original simulation file to easily save modified simulations to Neuronify's sources.
 * \param fileUrl
 * \return
 */
QUrl StandardPaths::originalSimulationLocation(QUrl fileUrl) {
    QString fileName = QQmlFile::urlToLocalFileOrQrc(fileUrl);
    fileName.replace(":/", "/");
    return QUrl::fromLocalFile("/home/svenni/Sync/projects/neuronify/neuronify" + fileName);
}
