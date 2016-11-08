#include "script.h"
#include "scripthandler.h"
#include "scriptcommand.h"
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

void ScriptHandler::runCommand(QString command)
{
    m_commands.append(command);
}

void ScriptHandler::runScript(QString script, QString fileName)
{
    Script *scriptObj = new Script(this);
    scriptObj->setFileName(fileName);
    scriptObj->setScript(script);

    m_scriptStack.push(scriptObj);
    emit newScript();
}

QList<ScriptCommand> ScriptHandler::nextCommands()
{
    QList<ScriptCommand> commands;
    commands.append(nextCommand());
    return commands;
}

bool ScriptHandler::hasNextCommand() {
    return m_scriptStack.size() > 0;
}

ScriptCommand ScriptHandler::nextCommand()
{
    Script *script = m_scriptStack.top();
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
    }

    bool scriptIsFile = !script->fileName().isEmpty();

    return ScriptCommand(command, (scriptIsFile ? ScriptCommand::Type::File : ScriptCommand::Type::Editor), line);
}

int ScriptHandler::simulationSpeed() const
{
    return m_simulationSpeed;
}

void ScriptHandler::setSimulationSpeed(int simulationSpeed)
{
    m_simulationSpeed = simulationSpeed;
}
