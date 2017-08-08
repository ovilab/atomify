#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QObject>
#include <QStack>
#include <QList>
#include <QUrl>
#include "scriptcommand.h"
#include "scriptparser.h"
#include "LammpsWrappers/lammpserror.h"

class ScriptHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LammpsError* error READ error WRITE setError NOTIFY errorChanged)

public:
    explicit ScriptHandler(QObject *parent = 0);
    Q_INVOKABLE void reset();
    Q_INVOKABLE bool runCommand(QString command);
    Q_INVOKABLE void runScript(QString fileName, QString script = "");
    Q_INVOKABLE QString previousSingleCommandString();
    Q_INVOKABLE QString nextSingleCommandString();
    Q_INVOKABLE QString lastSingleCommandString();
    QList<class ScriptCommand> nextCommands(class LAMMPSController &controller, bool continueIfNoCommands);
    int simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    void didFinishPreviousCommands();
    bool hasNextCommand();
    QVector<ScriptCommand> &editorCommands();
    ScriptParser &parser();
    LammpsError* error() const;

signals:
    void newScript();
    void newCommand();
    void errorChanged(LammpsError* error);
    void consoleOutput(QString message);

public slots:
    void setError(LammpsError* error);

private:
    ScriptParser m_parser;
    QString currentDirectory;
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
    QList<ScriptCommand> m_singleCommands;

    class ScriptCommand nextCommand();
    QList<class ScriptCommand> singleCommand(class LAMMPSController &controller);
    QList<class ScriptCommand> scriptCommands(class LAMMPSController &controller);
    void setWorkingDirectory(QString fileName);
    void handleEditorCommands(QList<class ScriptCommand> &commands);
    void handleRunCommand(class LAMMPSController &controller, class ScriptCommand &command, QList<class ScriptCommand> &commands);
    bool handleCommand(class LAMMPSController &controller, class ScriptCommand &command, QList<class ScriptCommand> &commands);
    bool commandRequiresSynchronization(const ScriptCommand &command);
    LammpsError* m_error = nullptr;
};

#endif // SCRIPTHANDLER_H
