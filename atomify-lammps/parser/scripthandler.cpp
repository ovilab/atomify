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
    m_commands.clear();
}

bool ScriptHandler::runCommand(QString command)
{
    if(command.trimmed().startsWith("run") && m_activeRunCommand) {
        return false;
    }

    m_commands.append(command);
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

QString ScriptHandler::includePath(const ScriptCommand &command) {
    if(command.command().trimmed().startsWith("include")) {
        QStringList list = command.command().trimmed().split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if(list.size()>1) {
            QString argument = list.at(1);
            // TODO: see if this is a variable
            return argument;
        }
    }
    return QString(""); // Not an include command
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

QList<ScriptCommand> ScriptHandler::singleCommand(LAMMPSController &controller) {

}

QList<ScriptCommand> ScriptHandler::scriptCommands(LAMMPSController &controller) {
    QList<ScriptCommand> commands;
    while(true) {
        ScriptCommand command = nextCommand();

        if(!includePath(command).isEmpty()) {
            QString path = includePath(command);

            QFileInfo info(path);
            if(info.exists()) {
                Script *script = new Script(this);
                script->setFileName(path);
                script->readFile();
                m_scriptStack.push(script);
                return commands;
            } else {
                qDebug() << "Error, could not find file " << path;
                continue;
            }
        }

        commands.append(command);
        if(commandRequiresSynchronization(command)) {
            break;
        }
        if(!m_scriptStack.top()->hasNextLine()) {
            break;
        }
    }
    return commands;
}

QList<ScriptCommand> ScriptHandler::nextCommands(LAMMPSController &controller)
{
    if(m_runningScript) {
        qDebug() << "Error, can't ask for more commands while we're still working on the previous commands";
        throw "damn...";
    }

    m_runningScript = true;

    // Step 1) Check for single commands. Parse them as normal commands (i.e. include should work)
    if(m_commands.size() > 0) {
        return singleCommand(controller);
    }

    // Step 2) Continue active run/rerun command
    if(m_activeRunCommand) {
        bool preRunNeeded = true;
        QString nextRunCommand = m_activeRunCommand->nextCommand(controller.system()->timesteps(), 1, preRunNeeded);

        if(m_activeRunCommand->finished) {
            delete m_activeRunCommand;
            m_activeRunCommand = nullptr;
        }
        QList<ScriptCommand> list;
        ScriptCommand command(nextRunCommand, ScriptCommand::Type::File, 0); // TODO: line numbers
        list.append(command);

        return list;
    }

    // Step 3) Create command queue based on script stack
    if(m_scriptStack.size()>0) {
        return scriptCommands(controller);
    }

    return QList<ScriptCommand>();
}

bool ScriptHandler::hasNextCommand() {
    return m_scriptStack.size()>0 || m_commands.size()>0;
}

void ScriptHandler::didFinishPreviousCommands()
{
    m_runningScript = false;
    if(m_scriptStack.size()>0 && !m_scriptStack.top()->hasNextLine()) {
        Script *script = m_scriptStack.pop();
        delete script;
    }
}

void ScriptHandler::setWorkingDirectory(QString fileName) {
    QFileInfo fileInfo(fileName);
    if(!fileInfo.exists()) return;

    QString currentDir = fileInfo.absoluteDir().path();
    QByteArray currentDirBytes = currentDir.toUtf8();

    chdir(currentDirBytes.constData());
}

ScriptCommand ScriptHandler::nextCommand()
{
    assert(m_scriptStack.size()>0 && "scriptStack can't be empty when asking for nextCommand()");

    Script *script = m_scriptStack.top();
    setWorkingDirectory(script->fileName());
    int line = script->currentLine();

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
    return ScriptCommand(command,  (scriptIsFile ? ScriptCommand::Type::File : ScriptCommand::Type::Editor), line);
}

int ScriptHandler::simulationSpeed() const
{
    return m_simulationSpeed;
}

void ScriptHandler::setSimulationSpeed(int simulationSpeed)
{
    m_simulationSpeed = simulationSpeed;
}
