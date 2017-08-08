#ifndef CUTE_VERSIONING_QUICKVERSION
#define CUTE_VERSIONING_QUICKVERSION

#include <QObject>
#include <QString>
#include <QQmlEngine>
#include <QVersionNumber>

namespace CuteVersioning {
class QuickVersion : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(QuickVersion)

    Q_PROPERTY(int majorVersion READ majorVersion CONSTANT)
    Q_PROPERTY(int minorVersion READ minorVersion CONSTANT)
    Q_PROPERTY(int microVersion READ microVersion CONSTANT)
    Q_PROPERTY(QString suffix READ suffix CONSTANT)
    Q_PROPERTY(QString latestTag READ latestTag CONSTANT)
    Q_PROPERTY(QString identifier READ identifier CONSTANT)
    Q_PROPERTY(bool dirty READ dirty CONSTANT)

public:
    QuickVersion();

    int majorVersion() const;
    int minorVersion() const;
    int microVersion() const;
    QString suffix() const;
    QString latestTag() const;
    QString identifier() const;
    bool dirty() const;

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
};
}
#endif // CUTE_VERSIONING_QUICKVERSION
