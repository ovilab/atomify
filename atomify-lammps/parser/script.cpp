#include "script.h"

Script::Script(QObject *parent) : QObject(parent),
  m_currentLine(-1)
{

}

QString Script::fileName() const
{
    return m_fileName;
}

int Script::currentLine() const
{
    return m_currentLine;
}

bool Script::hasNextLine() const
{
    return m_currentLine+1 < m_lines.size();
}

QString Script::getNextLine()
{
    if(!hasNextLine()) return QString(""); //TODO: should never happen. User should ask for hasNextLine first, so throw error in the future.
    QString nextLine = m_lines.at(m_currentLine);
    setCurrentLine(currentLine()+1);
    return nextLine;
}

QString Script::script() const
{
    return m_script;
}

void Script::setFileName(QString fileName)
{
    if (m_fileName == fileName)
        return;

    m_fileName = fileName;
    emit fileNameChanged(fileName);
}

void Script::setCurrentLine(int currentLine)
{
    if (m_currentLine == currentLine)
        return;

    m_currentLine = currentLine;
    emit currentLineChanged(currentLine);
}

void Script::setScript(QString script)
{
    if (m_script == script)
        return;

    m_script = script;
    readLines();
    emit scriptChanged(script);
}

void Script::readLines()
{
    m_lines = m_script.split(QRegExp("\n|\r\n|\r"));
    setCurrentLine(0);
    // TODO: process lines, search for labels
}
