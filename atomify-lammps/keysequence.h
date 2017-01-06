#ifndef KEYSEQUENCE_H
#define KEYSEQUENCE_H

#include <QObject>
#include <QKeySequence>
#include <QQmlEngine>

class KeySequence : public QObject
{
    Q_OBJECT
public:
    explicit KeySequence(QObject *parent = 0);

    Q_INVOKABLE QString keyBindings(QKeySequence::StandardKey key);

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

};

#endif // KEYSEQUENCE_H
