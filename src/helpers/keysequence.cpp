#include "keysequence.h"

#include <QQmlEngine>

KeySequence::KeySequence(QObject* parent)
    : QObject(parent)
{
}

QString KeySequence::keyBindings(QKeySequence::StandardKey key)
{
    const auto& bindings = QKeySequence::keyBindings(QKeySequence::FullScreen);
    if (bindings.count() < 1) {
        return "";
    }
    if (bindings.count() == 1) {
        return bindings.first().toString();
    }
    QStringList keys;
    for (const auto& key : bindings) {
        keys << key.toString();
    }
    return keys.join(", ");
}

QObject* KeySequence::qmlInstance(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return new KeySequence;
}

QString KeySequence::nativeText(QKeySequence::StandardKey key)
{
    return QKeySequence(key).toString(QKeySequence::NativeText);
}

QString KeySequence::nativeText(const QString& key)
{
    return QKeySequence(key).toString(QKeySequence::NativeText);
}
