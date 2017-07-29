#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QMutex>
#include <QObject>
#include <QDebug>
#include <QQueue>
#include <QString>
#include "scriptcommand.h"
#include "scriptparser.h"
#include "LammpsWrappers/atoms.h"

class AtomifySimulator;

class ScriptHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)

public:
    ScriptHandler();
    const ScriptCommand &nextCommand();
    Atoms* atoms() const;
    ScriptParser &parser() { return m_parser; }
    bool parseLammpsCommand(QString command, class LAMMPSController *lammpsController);
    bool parseGUICommand(QString command);
    class LammpsState *lammpsState() const;
    void setLammpsState(LammpsState *lammpsState);
    Q_INVOKABLE QString previousSingleCommandString();
    Q_INVOKABLE QString nextSingleCommandString();
    Q_INVOKABLE QString lastSingleCommandString();
    QString label() const;

public slots:
    void runScript(QString script, ScriptCommand::Type type = ScriptCommand::Type::Editor, QString filename = "", QString currentDir = "");
    void runCommand(QString command, bool addToPreviousCommands = false);
    void runFile(QString filename); // TODO Set cwd
    void addCommandToTop(ScriptCommand command);
    void addCommandsToTop(QList<QString> commands, ScriptCommand::Type commandType);
    void reset();
    void setAtoms(Atoms* atoms);

    QString readFile(QString filename);
    QString copyDataFileToReadablePath(QString filename);

signals:
    void labelChanged(QString label);

protected:
    void doRunScript(QString script, ScriptCommand::Type type, QString filename, QString currentDir);

private:
    ScriptParser m_parser;
    QMutex m_mutex;
    QQueue<ScriptCommand> m_lammpsCommandStack;
    QQueue<ScriptCommand> m_queuedCommands;
    ScriptCommand m_currentCommand;
    QVector<ScriptCommand> m_previousSingleCommands;
    int m_currentPreviousSingleCommandIndex = 0;
    class LammpsState *m_lammpsState = nullptr;
    Atoms* m_atoms = nullptr;
    QString m_tempLocation;
    QString m_label;
};

#endif // SCRIPTHANDLER_H
