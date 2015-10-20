#include "scripthandler.h"
#include "lammpsfilehandler.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include <sstream>
#include <string>
#include <QDebug>
#include <QFile>
#include <QQmlFile>
#include <QMutexLocker>
ScriptHandler::ScriptHandler()
{

}

QPair<QString, CommandInfo> ScriptHandler::nextCommand()
{
    QMutexLocker locker(&m_mutex);

    if(m_queuedCommands.size()) {
        m_currentCommand = m_queuedCommands.dequeue();
    }
    else if(m_lammpsCommandStack.size()) {
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

QString ScriptHandler::currentCommand() const
{
    return m_currentCommand.first;
}

AtomStyle *ScriptHandler::atomStyle() const
{
    return m_atomStyle;
}

QQueue<QPair<QString, CommandInfo> > &ScriptHandler::queuedCommands()
{
    return m_queuedCommands;
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

void ScriptHandler::setAtomStyle(AtomStyle *atomStyle)
{
    m_atomStyle = atomStyle;
}

void ScriptHandler::runFile(QString filename)
{
    QString fileNameString = QQmlFile::urlToLocalFileOrQrc(filename);
    QFile f(fileNameString);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file: " << filename;
        return;
    }

    QString script(f.readAll());
    runScript(script, CommandInfo::Type::File, filename);
}

void ScriptHandler::parseEditorCommand(QString command, AtomifySimulator *mySimulator) {
    command.remove(0,2);
    if(m_parser.isAtomType(command)) {
        m_parser.atomType(command, [&](QString atomTypeName, int atomType) {
            if(m_atomStyle) m_atomStyle->setAtomType(atomTypeName, atomType);
        });
        return;
    }

    if(m_parser.isAtomColorAndSize(command)) {
        m_parser.AtomColorAndSize(command, [&](float scale, QString color, int atomType) {
            if(m_atomStyle) m_atomStyle->setScaleAndColorForAtom(scale, color, atomType);
        });
        return;
    }
}

bool ScriptHandler::parseLammpsCommand(QString command, LAMMPSController *lammpsController) {
    if(command.trimmed().compare("disableAllEnsembleFixes") == 0) {
        lammpsController->disableAllEnsembleFixes();
        return true;
    }

    return false;
}

void ScriptHandler::runScript(QString script, CommandInfo::Type type, QString filename) {
    QMutexLocker locker(&m_mutex);

    if(!script.isEmpty())
    {
        // If the file is not empty, load each command and add it to the queue.
        // Now, if there is an include command, load that script too.
        int lineNumber = 1;
        QString currentCommand;
        QStringList lines = script.split("\n");

        for(QString line : lines) {
            line = line.trimmed();
            if(line.endsWith("&")) {
                line.remove(line.length() - 1, 1);
                currentCommand.append(QString(" %1").arg(line));
                continue;
            } else {
                currentCommand.append(QString(" %1").arg(line));
            }

            currentCommand = currentCommand.trimmed();

            if(m_parser.isInclude(currentCommand)) {
                QString filename = m_parser.includePath(currentCommand);
                loadScriptFromFile(filename);
                currentCommand.clear(); lineNumber++; continue; // This line is complete
            }

            if(!currentCommand.isEmpty()) {
                auto commandObject = QPair<QString, CommandInfo>(currentCommand, CommandInfo(type, lineNumber, filename));
                m_lammpsCommandStack.enqueue(commandObject);
            }

            if(m_parser.isEditorCommand(currentCommand)) {
                auto commandObject = QPair<QString, CommandInfo>(currentCommand, CommandInfo(type, lineNumber, filename));
                m_queuedCommands.push_back(commandObject);
                // parseEditorCommand(currentCommand);
                currentCommand.clear(); lineNumber++; continue; // This line is complete
            }

            currentCommand.clear(); lineNumber++; continue; // This line is complete
        }
    }
}

void ScriptHandler::runCommand(QString command, bool addToPreviousCommands)
{
    if(addToPreviousCommands) m_previousSingleCommands.push_back(command);
    auto commandObject = QPair<QString, CommandInfo>(command, CommandInfo(CommandInfo::Type::SingleCommand));
    m_queuedCommands.enqueue(commandObject);
}

void ScriptHandler::addCommandToTop(QString command, CommandInfo commandInfo)
{
    auto commandObject = QPair<QString, CommandInfo>(command, commandInfo);
    m_queuedCommands.push_front(commandObject);
}

void ScriptHandler::addCommandsToTop(QList<QString> commands, CommandInfo commandInfo)
{
    QList<QPair<QString, CommandInfo> > commandObjects;
    for(QString command : commands) {
        auto commandObject = QPair<QString, CommandInfo>(command, commandInfo);
        commandObjects.push_front(commandObject);
    }

    for(auto commandObject : commandObjects) {
        m_queuedCommands.push_front(commandObject);
    }
}

void ScriptHandler::reset()
{
    m_queuedCommands.clear();
    m_lammpsCommandStack.clear();
    m_currentCommand = QPair<QString, CommandInfo>(QString(""), CommandInfo());
}

