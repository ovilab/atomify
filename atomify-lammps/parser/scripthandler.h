#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QObject>
#include <QStack>
#include <QList>
#include <QUrl>

class ScriptHandler : public QObject
{
    Q_OBJECT

public:
    explicit ScriptHandler(QObject *parent = 0);
    Q_INVOKABLE void reset();
    Q_INVOKABLE bool runCommand(QString command);
    Q_INVOKABLE void runScript(QString fileName, QString script = "");
    QList<class ScriptCommand> nextCommands(class LAMMPSController &controller);
    int simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    void didFinishPreviousCommands();
    bool hasNextCommand();
signals:
    void newScript();
    void newCommand();

public slots:

private:
    class ScriptCommand nextCommand();
    QList<class ScriptCommand> singleCommand(class LAMMPSController &controller);
    QList<class ScriptCommand> scriptCommands(class LAMMPSController &controller);
    class RunCommand *m_activeRunCommand = nullptr;
    QStack<class Script*> m_scriptStack;
    QList<QString> m_commands;
    QString includePath(const ScriptCommand &command);
    bool commandRequiresSynchronization(const ScriptCommand &command);
    int m_simulationSpeed = 1;
    bool m_runningScript = false;
    bool m_preRunNeeded = true;
    unsigned int m_runCommandStart = 0;
    unsigned int m_runCommandEnd = 0;
    void setWorkingDirectory(QString fileName);
};

#endif // SCRIPTHANDLER_H
