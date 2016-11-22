#include "scriptcommand.h"

ScriptCommand::ScriptCommand(QString command, ScriptCommand::Type type, int line, QString fileName, QString path)
    : m_command(command)
    , m_fileName(fileName)
    , m_path(path)
    , m_type(type)
    , m_line(line)
{

}

const QString &ScriptCommand::command() const
{
    return m_command;
}

ScriptCommand::Type ScriptCommand::type() const {
    return m_type;
}

int ScriptCommand::line() const
{
    return m_line;
}

QString ScriptCommand::fileName() const
{
    return m_fileName;
}

QString ScriptCommand::path() const
{
    return m_path;
}

bool ScriptCommand::canProcessSimulatorControls() const
{
    return m_canProcessSimulatorControls;
}

void ScriptCommand::setCanProcessSimulatorControls(bool canProcessSimulatorControls)
{
    m_canProcessSimulatorControls = canProcessSimulatorControls;
}
