#include "scriptcommand.h"

ScriptCommand::ScriptCommand(QString command, ScriptCommand::Type type, int line)
    : m_command(command)
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
