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
class LAMMPSController;
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
    void parseEditorCommand(QString command, AtomifySimulator *mySimulator);
    bool parseLammpsCommand(QString command, LAMMPSController *lammpsController);

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
};

#endif // SCRIPTHANDLER_H
