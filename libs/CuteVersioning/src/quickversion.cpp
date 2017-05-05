#include "quickversion.h"

#include <QVersionNumber>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "../CuteVersioning/version.h"

namespace CuteVersioning {

QuickVersion::QuickVersion()
{

}

QObject *QuickVersion::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new QuickVersion;
}

int QuickVersion::majorVersion() const
{
    return CuteVersioning::versionNumber.majorVersion();
}

int QuickVersion::minorVersion() const
{
    return CuteVersioning::versionNumber.minorVersion();
}

int QuickVersion::microVersion() const
{
    return CuteVersioning::versionNumber.microVersion();
}

QString QuickVersion::suffix() const
{
//    return CuteVersioning::versionNumber.suffix();
    return QString();
}

QString QuickVersion::latestTag() const
{
    return CuteVersioning::latestTag;
}

QString QuickVersion::identifier() const
{
    return CuteVersioning::identifier;
}

bool QuickVersion::dirty() const
{
    return CuteVersioning::dirty;
}

}
