#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QUrl>
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
    Q_PROPERTY(int currentLine READ currentLine WRITE setCurrentLine NOTIFY currentLineChanged)
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
    int m_currentLine = -1;
    int m_currentLineInRootFile = -1;

public:
    ScriptHandler();
    const ScriptCommand &nextCommand();
    Atoms* atoms() const;
    ScriptParser &parser();
    bool parseLammpsCommand(QString command, class LAMMPSController *lammpsController);
    void parseGUICommand(QString command);
    LammpsState *lammpsState() const;
    void setLammpsState(LammpsState *lammpsState);
    Q_INVOKABLE QString previousSingleCommandString();
    Q_INVOKABLE QString nextSingleCommandString();
    Q_INVOKABLE QString lastSingleCommandString();
    Q_INVOKABLE void setWorkingDirectory(QUrl fileName);
    Q_INVOKABLE void runFile(QString filename); // TODO Set cwd
    int currentLine() const;

public slots:
    void runScript(QString script, ScriptCommand::Type type = ScriptCommand::Type::Editor, QString filename = "", QString currentDir = "");
    void runCommand(QString command, bool addToPreviousCommands = false);
    void addCommandToTop(ScriptCommand command);
    void addCommandsToTop(QList<QString> commands, ScriptCommand::Type commandType);
    void reset();
    void setAtoms(Atoms* atoms);

    QString readFile(QString filename);
    QString copyDataFileToReadablePath(QString filename);
    void setCurrentLine(int currentLine);

signals:
    void currentLineChanged(int currentLine);
    void consoleOutput(QString output);

protected:
    void doRunScript(QString script, ScriptCommand::Type type, QString filename, QString currentDir);
};

#endif // SCRIPTHANDLER_H
