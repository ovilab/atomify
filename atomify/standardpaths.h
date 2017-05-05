#ifndef STANDARDPATHS_H
#define STANDARDPATHS_H

#include <QObject>
#include <QStandardPaths>
#include <QQmlEngine>

class StandardPaths : public QObject
{
    Q_OBJECT
    Q_ENUMS(StandardLocation)
public:
    // Do not re-order, must match QStandardPaths
    enum StandardLocation {
        DesktopLocation,
        DocumentsLocation,
        FontsLocation,
        ApplicationsLocation,
        MusicLocation,
        MoviesLocation,
        PicturesLocation,
        TempLocation,
        HomeLocation,
        DataLocation,
        CacheLocation,
        GenericDataLocation,
        RuntimeLocation,
        ConfigLocation,
        DownloadLocation,
        GenericCacheLocation,
        GenericConfigLocation,
        AppDataLocation,
        AppConfigLocation,
        AppLocalDataLocation = DataLocation
    };

    explicit StandardPaths(QObject *parent = nullptr);
    Q_INVOKABLE static QUrl writableLocation(StandardLocation location, const QString &filename);
    Q_INVOKABLE static QUrl locate(StandardLocation location, const QString &filename);
    Q_INVOKABLE static QString toLocalFile(QUrl url);
    Q_INVOKABLE static QUrl originalSimulationLocation(QUrl fileUrl);

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
};

#endif // STANDARDPATHS_H
