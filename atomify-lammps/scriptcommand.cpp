#include "scriptcommand.h"

ScriptCommand::ScriptCommand(QString command, ScriptCommand::Type type, int line, int lineInRootFile, QString filename, QString path)
    : m_command(command)
    , m_type(type)
    , m_line(line)
    , m_lineInRootFile(lineInRootFile)
    , m_filename(filename)
    , m_path(path)
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

int ScriptCommand::lineInRootFile() const
{
    return m_lineInRootFile;
}

const QString &ScriptCommand::filename() const
{
    return m_filename;
}

const QString &ScriptCommand::path() const
{
    return m_path;
}

