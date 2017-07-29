#include "lammpserror.h"

LammpsError::LammpsError(QObject *parent) : QObject(parent)
{

}

QString LammpsError::scriptFile() const
{
    return m_scriptFile;
}

QString LammpsError::message() const
{
    return m_message;
}

int LammpsError::line() const
{
    return m_line;
}

QString LammpsError::command() const
{
    return m_command;
}

void LammpsError::setScriptFile(QString scriptFile)
{
    if (m_scriptFile == scriptFile)
        return;

    m_scriptFile = scriptFile;
    emit scriptFileChanged(scriptFile);
}

void LammpsError::setMessage(QString message)
{
    if (m_message == message)
        return;

    m_message = message;
    emit messageChanged(message);
}

void LammpsError::setLine(int line)
{
    if (m_line == line)
        return;

    m_line = line;
    emit lineChanged(line);
}

void LammpsError::setCommand(QString command)
{
    if (m_command == command)
        return;

    m_command = command;
    emit commandChanged(command);
}
