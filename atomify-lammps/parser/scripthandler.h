#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QObject>
#include <QStack>
#include <QList>
#include <QUrl>
#include "scriptcommand.h"
#include "scriptparser.h"

class ScriptHandler : public QObject
{
    Q_OBJECT

public:
    explicit ScriptHandler(QObject *parent = 0);
    Q_INVOKABLE void reset();
    Q_INVOKABLE bool runCommand(QString command);
    Q_INVOKABLE void runScript(QString fileName, QString script = "");
    Q_INVOKABLE QString previousSingleCommandString();
    Q_INVOKABLE QString nextSingleCommandString();
    Q_INVOKABLE QString lastSingleCommandString();
    QList<class ScriptCommand> nextCommands(class LAMMPSController &controller);
    int simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    void didFinishPreviousCommands();
    bool hasNextCommand();
    QVector<ScriptCommand> &editorCommands() { return m_editorCommands; }
    ScriptParser &parser() { return m_parser; }

signals:
    void newScript();
    void newCommand();

public slots:

private:
    ScriptParser m_parser;
    int m_simulationSpeed = 1;
    bool m_runningScript = false;
    bool m_preRunNeeded = true;
    unsigned int m_runCommandStart = 0;
    unsigned int m_runCommandEnd = 0;
    QVector<ScriptCommand> m_previousSingleCommands;
    int m_currentPreviousSingleCommandIndex = 0;
    class RunCommand *m_activeRunCommand = nullptr;
    QVector<ScriptCommand> m_editorCommands;
    QStack<class Script*> m_scriptStack;
    QList<ScriptCommand> m_commands;

    class ScriptCommand nextCommand();
    QList<class ScriptCommand> singleCommand(class LAMMPSController &controller);
    QList<class ScriptCommand> scriptCommands(class LAMMPSController &controller);
    void setWorkingDirectory(QString fileName);
    void handleEditorCommands(QList<class ScriptCommand> &commands);
    bool commandRequiresSynchronization(const ScriptCommand &command);
    void handleRunCommand(class LAMMPSController &controller, class ScriptCommand &command, QList<class ScriptCommand> &commands);
    bool handleCommand(class LAMMPSController &controller, class ScriptCommand &command, QList<class ScriptCommand> &commands);
};

#endif // SCRIPTHANDLER_H
