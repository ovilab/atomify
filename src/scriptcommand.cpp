#include "scriptcommand.h"

ScriptCommand::ScriptCommand(QString command, ScriptCommand::Type type, int line, QString filename)
    : m_command(command)
    , m_type(type)
    , m_line(line)
    , m_filename(filename)
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

const QString &ScriptCommand::filename() const
{
    return m_filename;
}

