#include "script.h"
#include "scripthandler.h"
#include "scriptcommand.h"
#include "runcommand.h"
#include "lammpscontroller.h"
#include <unistd.h>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <cassert>
#include "../LammpsWrappers/system.h"

ScriptHandler::ScriptHandler(QObject *parent) : QObject(parent)
{

}

void ScriptHandler::reset() {
    for(Script *script : m_scriptStack) {
        delete script;
    }
    m_scriptStack.clear();
    m_singleCommands.clear();
    setError(nullptr); // Deleted in LAMMPSController
}

bool ScriptHandler::runCommand(QString command)
{
    if(command.trimmed().startsWith("run") && m_activeRunCommand) {
        return false;
    }
    ScriptCommand commandObject(command, ScriptCommand::Type::SingleCommand);
    m_singleCommands.append(commandObject);
    m_previousSingleCommands.push_back(commandObject);
    return true;
}

void ScriptHandler::runScript(QString fileName, QString script)
{
    Script *scriptObj = new Script(this);
    scriptObj->setFileName(fileName);
    if(script.isEmpty()) {
        scriptObj->readFile();
    } else {
        scriptObj->setScript(script);
    }

    m_scriptStack.push(scriptObj);
    emit newScript();
}

bool ScriptHandler::commandRequiresSynchronization(const ScriptCommand &command) {
    // All of these commands might change visuals so we should synchronize
    if(command.command().trimmed().startsWith("create_box")) return true;
    if(command.command().trimmed().startsWith("create_atoms")) return true;
    if(command.command().trimmed().startsWith("read_data")) return true;
    if(command.command().trimmed().startsWith("read_restart")) return true;
    if(command.command().trimmed().startsWith("run")) return true;
    if(command.command().trimmed().startsWith("minimize")) return true;
    if(command.command().trimmed().startsWith("region")) return true;
    if(command.command().trimmed().startsWith("group")) return true;
    if(command.command().trimmed().startsWith("boundary")) return true;
    if(command.command().trimmed().startsWith("change_box")) return true;
    if(command.command().trimmed().startsWith("box")) return true;
    if(command.command().trimmed().startsWith("create_bonds")) return true;
    if(command.command().trimmed().startsWith("delete_atoms")) return true;
    if(command.command().trimmed().startsWith("displace_atoms")) return true;
    if(command.command().trimmed().startsWith("read_dump")) return true;
    if(command.command().trimmed().startsWith("replicate")) return true;
    if(command.command().trimmed().startsWith("set")) return true;
    if(command.command().trimmed().startsWith("velocity")) return true;
    if(command.command().trimmed().startsWith("compute")) return true;
    if(command.command().trimmed().startsWith("fix")) return true;
    if(command.command().trimmed().startsWith("variable")) return true;
    if(command.command().trimmed().startsWith("uncompute")) return true;
    if(command.command().trimmed().startsWith("unfix")) return true;
    if(command.command().trimmed().startsWith("fix_modify")) return true;
    if(command.command().trimmed().startsWith("compute_modify")) return true;

    return false;
}

void ScriptHandler::handleRunCommand(LAMMPSController &controller, ScriptCommand &command, QList<ScriptCommand> &commands) {
    QStringList words = command.command().trimmed().split(QRegExp("\\s+"), QString::SkipEmptyParts);
    if(words.size()>1) {
        // First try to parse second argument as a number
        QString runArgument = words.at(1);
        bool ok;
        ulong timesteps = runArgument.toULong(&ok);
        if(ok) {
            // We managed to parse this to an uint

            // If user asked for run 0, just do it
            if(timesteps == 0) {
                // just run the 'run 0' command
                commands.append(command);
                return;
            }

            // Create RunCommand object to split the run command into smaller parts
            ulong start = controller.system()->currentTimestep();
            ulong stop = start + timesteps;
            m_activeRunCommand = new RunCommand(start, stop);
        } else {
            // This is a variable
            QString strippedArgument = runArgument;
            if(strippedArgument.startsWith("$")) {
                // Variable is probably on the form $X, $(var) or ${var}.
                if(strippedArgument.length() == 2) {
                    strippedArgument.remove(0,1); // Remove the $
                } else if( (strippedArgument.startsWith("$(") && strippedArgument.endsWith(")")) ||
                           (strippedArgument.startsWith("${") && strippedArgument.endsWith("}"))) {
                    strippedArgument.remove(0,2); // Remove the $( or ${
                    strippedArgument.remove(strippedArgument.length()-1,1); // Remove the ) or }
                }
            } else if(strippedArgument.startsWith("v_")) {
                strippedArgument.remove(0,2); // Remove the v_
            }

            if(controller.variableExists(strippedArgument)) {
                ulong timesteps = controller.variableValue(strippedArgument);
                ulong start = controller.system()->currentTimestep();
                ulong stop = start + timesteps;
                m_activeRunCommand = new RunCommand(start, stop);
            } else {
                qDebug() << "Error, could not parse run command";
                exit(1);
            }
        }
    }

    // Now fetch the newest one, with preRun = true
    QString nextRunCommand = m_activeRunCommand->nextCommand(controller.system()->currentTimestep(), m_simulationSpeed, true);
    ScriptCommand commandObject(nextRunCommand, ScriptCommand::Type::File, command.line(), command.fileName(), command.path());
    commands.append(commandObject);
}

QList<ScriptCommand> ScriptHandler::scriptCommands(LAMMPSController &controller) {
    QList<ScriptCommand> commands;
    while(true) {
        ScriptCommand command = nextCommand();
        bool canAppendMoreCommands = handleCommand(controller, command, commands);
        if(!canAppendMoreCommands) break;
    }

    return commands;
}

bool ScriptHandler::handleCommand(LAMMPSController &controller, ScriptCommand &command, QList<ScriptCommand> &commands) {
    // Returns true if we can continue after this command
    if(!m_parser.includePath(command.command()).isEmpty()) {
        QString path = m_parser.includePath(command.command());

        QFileInfo info(path);
        if(info.exists()) {
            Script *script = new Script(this);
            script->setFileName(path);
            script->readFile();
            m_scriptStack.push(script);
            return false;
        } else {
            qDebug() << "Error, could not find file " << path;
            return false;
        }
    }

    if(command.command().startsWith("run")) {
        handleRunCommand(controller, command, commands);
        return false;
    }

    commands.append(command);
    if(commandRequiresSynchronization(command)) {
        return false;
    }
    if(!m_scriptStack.top()->hasNextLine()) {
        return false;
    }

    return true;
}

void ScriptHandler::handleEditorCommands(QList<ScriptCommand> &commands) {
    QList<ScriptCommand> remainingCommands;

    for(ScriptCommand &command : commands) {
        if(m_parser.isEditorCommand(command.command())) {
            QString commandString = command.command();
            commandString.remove(0,2);

            bool isHandled = false;
            isHandled |= m_parser.atomColor(commandString);
            isHandled |= m_parser.atomColorAndSize(commandString);
            isHandled |= m_parser.atomType(commandString);
            isHandled |= m_parser.bond(commandString);
            if(isHandled) {
                m_editorCommands.append(command);
                continue;
            }
        }
        // If not handled editor command, it is normal command
        remainingCommands.append(command);
    }

    commands = remainingCommands;
}

QList<ScriptCommand> ScriptHandler::nextCommands(LAMMPSController &controller, bool continueIfNoCommands)
{
    if(m_runningScript) {
        qDebug() << "Error, can't ask for more commands while we're still working on the previous commands";
        throw "damn...";
    }

    m_runningScript = true;

    // Step 1) Check for single commands. Parse them as normal commands (i.e. include should work)
    if(m_singleCommands.size() > 0) {
        // First check all single commands for editor commands and append to editor command list
        handleEditorCommands(m_singleCommands);

        QList<ScriptCommand> commands;
        auto it = m_singleCommands.begin();
        while(it != m_singleCommands.end()) {
            // Add single commands until we need to synchronize
            ScriptCommand &command = *it;
            bool canAppendMoreCommands = handleCommand(controller, command, commands);
            m_singleCommands.erase(it++);
            if(!canAppendMoreCommands) break;
        }
        return commands;
    }

    // Step 2) Continue active run/rerun command
    if(m_activeRunCommand) {
        bool preRunNeeded = true; // TODO: figure out whether or not this is true
        QString nextRunCommand = m_activeRunCommand->nextCommand(controller.system()->currentTimestep(), m_simulationSpeed, preRunNeeded);

        QList<ScriptCommand> commands;
        ScriptCommand command(nextRunCommand, ScriptCommand::Type::File, 0); // TODO: line numbers
        commands.append(command);

        return commands;
    }

    // Step 3) Create command queue based on script stack
    if(m_scriptStack.size()>0) {
        QList<ScriptCommand> commands = scriptCommands(controller);
        handleEditorCommands(commands);
        return commands;
    }

    if(continueIfNoCommands) {
        QList<ScriptCommand> commands;
        ScriptCommand command("run 1000", ScriptCommand::Type::SingleCommand);
        handleCommand(controller, command, commands);
        return commands;
    }

    return QList<ScriptCommand>();
}

bool ScriptHandler::hasNextCommand() {
    return m_scriptStack.size()>0 || m_singleCommands.size()>0;
}

QVector<ScriptCommand> &ScriptHandler::editorCommands() { return m_editorCommands; }

ScriptParser &ScriptHandler::parser() { return m_parser; }

LammpsError *ScriptHandler::error() const
{
    return m_error;
}

void ScriptHandler::setError(LammpsError *error)
{
    if (m_error == error)
        return;

    m_error = error;
    emit errorChanged(error);
}

void ScriptHandler::didFinishPreviousCommands()
{
    m_runningScript = false;
    if(m_scriptStack.size()>0 && !m_scriptStack.top()->hasNextLine()) {
        Script *script = m_scriptStack.pop();
        delete script;
    }

    if(m_activeRunCommand && m_activeRunCommand->finished) {
        delete m_activeRunCommand;
        m_activeRunCommand = nullptr;
    }
}

void ScriptHandler::setWorkingDirectory(QString fileName) {
    if(fileName.startsWith("file://")) {
        fileName.remove("file://");
    }

    QFileInfo fileInfo(fileName);
    if(!fileInfo.exists()) {
        return;
    }

    currentDirectory = fileInfo.absoluteDir().path();
    QByteArray currentDirBytes = currentDirectory.toUtf8();

    chdir(currentDirBytes.constData());
}

ScriptCommand ScriptHandler::nextCommand()
{
    assert(m_scriptStack.size()>0 && "scriptStack can't be empty when asking for nextCommand()");

    Script *script = m_scriptStack.top();
    setWorkingDirectory(script->fileName());
    int line = script->currentLine()+1;

    QString command;
    bool shouldReadNextLine = true;
    while(shouldReadNextLine) {
        shouldReadNextLine = false;

        QString nextLine = script->getNextLine().trimmed();
        if(nextLine.endsWith("&")) {
            nextLine.remove(nextLine.length() - 1, 1); // Remove the & char
            command.append(" ");
            command.append(nextLine);
            if(script->hasNextLine()) {
                // This is a comment and it continues on the next line
                shouldReadNextLine = true;
                continue;
            }
        }
        command = nextLine;
    }

    bool scriptIsFile = !script->fileName().isEmpty();
    if(scriptIsFile) {
        QString fileName = script->fileName();
        QFileInfo fileInfo(fileName);

        return ScriptCommand(command, ScriptCommand::Type::File, line, fileInfo.fileName(), fileInfo.path());
    } else {
        return ScriptCommand(command, ScriptCommand::Type::Editor, line);
    }
}

int ScriptHandler::simulationSpeed() const
{
    return m_simulationSpeed;
}

void ScriptHandler::setSimulationSpeed(int simulationSpeed)
{
    m_simulationSpeed = simulationSpeed;
}

QString ScriptHandler::previousSingleCommandString()
{
    if(--m_currentPreviousSingleCommandIndex < 0) {
        m_currentPreviousSingleCommandIndex = 0;
    }

    return m_previousSingleCommands.at(m_currentPreviousSingleCommandIndex).command();
}

QString ScriptHandler::nextSingleCommandString()
{
    if(++m_currentPreviousSingleCommandIndex < m_previousSingleCommands.count()) {
        return m_previousSingleCommands.at(m_currentPreviousSingleCommandIndex).command();
    } else{
        m_currentPreviousSingleCommandIndex = m_previousSingleCommands.count();
        return QString("");
    }
}

QString ScriptHandler::lastSingleCommandString()
{
    m_currentPreviousSingleCommandIndex = m_previousSingleCommands.count()-1;
    if(m_previousSingleCommands.count() > 0) {
        return m_previousSingleCommands.last().command();
    }
    else return QString("");
}
