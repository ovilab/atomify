#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QMutex>
#include <QObject>
#include <QDebug>
#include <QQueue>
#include <QString>
#include "scriptcommand.h"
#include "scriptparser.h"
#include "atomstyle.h"

class AtomifySimulator;

class ScriptHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AtomStyle* atomStyle READ atomStyle WRITE setAtomStyle)
private:
    ScriptParser m_parser;
    QMutex m_mutex;
    QQueue<ScriptCommand> m_lammpsCommandStack;
    QQueue<ScriptCommand> m_queuedCommands;
    ScriptCommand m_currentCommand;
    QVector<ScriptCommand> m_previousSingleCommands;
    int m_currentPreviousSingleCommandIndex = 0;
    class LammpsState *m_lammpsState = nullptr;
    AtomStyle* m_atomStyle = nullptr;
    QString m_tempLocation;

    Q_INVOKABLE QString previousSingleCommandString();
    Q_INVOKABLE QString nextSingleCommandString();
    Q_INVOKABLE QString lastSingleCommandString();

public:
    ScriptHandler();
    const ScriptCommand &nextCommand();
    void loadScriptFromFile(QString filename);
    AtomStyle* atomStyle() const;
    ScriptParser &parser() { return m_parser; }
    bool parseLammpsCommand(QString command, class LAMMPSController *lammpsController);
    void parseGUICommand(QString command);

    LammpsState *lammpsState() const;
    void setLammpsState(LammpsState *lammpsState);

public slots:
    void runScript(QString script, ScriptCommand::Type type = ScriptCommand::Type::Editor, QString filename = "");
    void runCommand(QString command, bool addToPreviousCommands = false);
    void runFile(QString filename); // TODO Set cwd
    void addCommandToTop(ScriptCommand command);
    void addCommandsToTop(QList<QString> commands, ScriptCommand::Type commandType);
    void reset();
    void setAtomStyle(AtomStyle* atomStyle);

    QString readFile(QString filename);
    QString copyDataFileToReadablePath(QString filename);
protected:
    void doRunScript(QString script, ScriptCommand::Type type, QString filename);
};

#endif // SCRIPTHANDLER_H
