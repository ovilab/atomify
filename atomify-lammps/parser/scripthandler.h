#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QObject>
#include <QStack>

class ScriptHandler : public QObject
{
    Q_OBJECT
public:
    explicit ScriptHandler(QObject *parent = 0);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void runCommand(QString command);
    Q_INVOKABLE void runScript(QString script, QString fileName);
    QList<class ScriptCommand> nextCommands();
    int simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    bool hasNextCommand();

signals:
    void newScript();
    void newCommand();

public slots:

private:
    class ScriptCommand nextCommand();
    QStack<class Script*> m_scriptStack;
    QVector<QString> m_commands;
    int m_simulationSpeed = 1;
    bool m_preRunNeeded = true;
    unsigned int m_runCommandStart = 0;
    unsigned int m_runCommandEnd = 0;
};

#endif // SCRIPTHANDLER_H
