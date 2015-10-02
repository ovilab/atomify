#include "scripthandler.h"
#include "lammpsfilehandler.h"
#include <sstream>
#include <string>
#include <QDebug>
#include <QMutexLocker>

ScriptHandler::ScriptHandler()
{

}

//QString line = m_commands.front().trimmed();
//QString command = line;
//m_commands.pop_front();

//// Check if the last character is & - then combine the command with the next line
//while(line.endsWith(QChar('&'))) {
//    command.remove(line.length() - 1,1);
//    line = m_commands.front().trimmed();
//    command.append(QString(" %1").arg(line));
//    m_commands.pop_front();
//}

//return command;

QPair<QString, CommandInfo> ScriptHandler::nextCommand()
{
    QMutexLocker locker(&m_mutex);
    if(m_lammpsCommandStack.size()) {
        m_currentCommand = m_lammpsCommandStack.dequeue();
    } else {
        m_currentCommand = QPair<QString, CommandInfo>(QString(""), CommandInfo());
    }

    return m_currentCommand;
}

void ScriptHandler::loadScriptFromFile(QString filename)
{
    runScript(IO::readFile(filename), CommandInfo::Type::File, filename);
}

QString ScriptHandler::previousSingleCommand()
{
    if(--m_currentPreviousSingleCommand < 0) {
        m_currentPreviousSingleCommand = 0;
    }

    return m_previousSingleCommands.at(m_currentPreviousSingleCommand);
}

QString ScriptHandler::nextSingleCommand()
{
    if(++m_currentPreviousSingleCommand < m_previousSingleCommands.count()) {
        return m_previousSingleCommands.at(m_currentPreviousSingleCommand);
    } else{
        m_currentPreviousSingleCommand = m_previousSingleCommands.count();
        return QString("");
    }
}

QString ScriptHandler::lastSingleCommand()
{
    m_currentPreviousSingleCommand = m_previousSingleCommands.count()-1;
    if(m_previousSingleCommands.count()>0) {
        return m_previousSingleCommands.last();
    }
    else return QString("");
}

void ScriptHandler::runScript(QString script, CommandInfo::Type type, QString filename)
{
    QMutexLocker locker(&m_mutex);

    if(!script.isEmpty())
    {
        // If the file is not empty, load each command and add it to the queue.
        // Now, if there is an include command, load that script too.
        std::stringstream lammpsScript(script.toStdString());
        std::string command;
        int lineNumber = 0;
        while(std::getline(lammpsScript,command,'\n')){
            lineNumber++;
            std::stringstream command_ss(command);
            std::string word;
            if(command_ss >> word) {
                if(word.compare("include") == 0) {
                    if(command_ss >> word) {
                        if(word.find("ext://") != std::string::npos) {
                            word.erase(0, 6); // Remove the ext:// prefix
                            word = IO::copyDataFileToReadablePath(QString::fromStdString(word)).toStdString();
                        }
                        loadScriptFromFile(QString::fromStdString(word));
                    } else {
                        qDebug() << "Invalid include statement.";
                        continue;
                    }
                } else {
                    auto commandObject = QPair<QString, CommandInfo>(QString::fromStdString(command), CommandInfo(type, lineNumber, filename));
                    m_lammpsCommandStack.enqueue(commandObject);
                }
            }
        }
    }
}

void ScriptHandler::runCommand(QString command)
{
    auto commandObject = QPair<QString, CommandInfo>(command, CommandInfo(CommandInfo::Type::SingleCommand));
    m_lammpsCommandStack.enqueue(commandObject);
    m_previousSingleCommands.push_back(command);
}

void ScriptHandler::reset()
{
    m_lammpsCommandStack.clear();
    m_currentCommand = QPair<QString, CommandInfo>(QString(""), CommandInfo());
}

