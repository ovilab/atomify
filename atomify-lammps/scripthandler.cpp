#include "scripthandler.h"
#include "lammpscontroller.h"
#include "mysimulator.h"
#include <unistd.h>
#include <sstream>
#include <string>
#include <QDebug>
#include <QFile>
#include <QQmlFile>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QFileInfo>

ScriptHandler::ScriptHandler()
{
    m_tempLocation = QStandardPaths::locate(QStandardPaths::TempLocation, QString(), QStandardPaths::LocateDirectory);
}

const ScriptCommand& ScriptHandler::nextCommand()
{
    QMutexLocker locker(&m_mutex);

    if(m_queuedCommands.size()) {
        m_currentCommand = m_queuedCommands.dequeue();
    }
    else if(m_lammpsCommandStack.size()) {
        m_currentCommand = m_lammpsCommandStack.dequeue();
    } else {
        m_currentCommand = ScriptCommand();
    }

    return m_currentCommand;
}

void ScriptHandler::loadScriptFromFile(QString filename)
{
    runScript(readFile(filename), ScriptCommand::Type::File, filename);
}

AtomStyle *ScriptHandler::atomStyle() const
{
    return m_atomStyle;
}

QString ScriptHandler::previousSingleCommandString()
{
    if(--m_currentPreviousSingleCommandIndex < 0) {
        m_currentPreviousSingleCommandIndex = 0;
    }

    return m_previousSingleCommands.at(m_currentPreviousSingleCommandIndex).command();
}

QString ScriptHandler::nextSingleCommandString()
{
    if(++m_currentPreviousSingleCommandIndex < m_previousSingleCommands.count()) {
        return m_previousSingleCommands.at(m_currentPreviousSingleCommandIndex).command();
    } else{
        m_currentPreviousSingleCommandIndex = m_previousSingleCommands.count();
        return QString("");
    }
}

QString ScriptHandler::lastSingleCommandString()
{
    m_currentPreviousSingleCommandIndex = m_previousSingleCommands.count()-1;
    if(m_previousSingleCommands.count() > 0) {
        return m_previousSingleCommands.last().command();
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
    runScript(script, ScriptCommand::Type::File, filename);
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

void ScriptHandler::runScript(QString script, ScriptCommand::Type type, QString filename) {
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
                auto commandObject = ScriptCommand(currentCommand, type, lineNumber, filename);
                m_lammpsCommandStack.enqueue(commandObject);
            }

            if(m_parser.isEditorCommand(currentCommand)) {
                auto commandObject = ScriptCommand(currentCommand, type, lineNumber, filename);
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
    QMutexLocker locker(&m_mutex);
    if(addToPreviousCommands) m_previousSingleCommands.push_back(command);
    auto commandObject = ScriptCommand(command, ScriptCommand::Type::SingleCommand);
    m_queuedCommands.enqueue(commandObject);
}

void ScriptHandler::addCommandToTop(ScriptCommand command)
{
    QMutexLocker locker(&m_mutex);
    m_queuedCommands.push_front(command);
}

void ScriptHandler::addCommandsToTop(QList<QString> commands, ScriptCommand::Type commandType)
{
    QMutexLocker locker(&m_mutex);
    QList<ScriptCommand> commandObjects;
    for(QString command : commands) {
        auto commandObject = ScriptCommand(command, commandType);
        commandObjects.push_front(commandObject);
    }

    for(auto commandObject : commandObjects) {
        m_queuedCommands.push_front(commandObject);
    }
}

void ScriptHandler::reset()
{
    QMutexLocker locker(&m_mutex);
    m_queuedCommands.clear();
    m_lammpsCommandStack.clear();
    m_currentCommand = ScriptCommand();
}

QString ScriptHandler::readFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file: " << file.fileName();
        return "";
    }

    return file.readAll();
}

QString ScriptHandler::copyDataFileToReadablePath(QString filename)
{
    QString qrcFilename = ":/scripts/"+filename;

    bool fileFound = false;
    QFileInfo fileInfo(qrcFilename);
    fileFound = fileInfo.exists();

    if(!fileFound) {
        // The file could be a zipped file, let's check that
        qrcFilename.append(".gz");
        filename.append(".gz");
        fileInfo = QFileInfo(qrcFilename);
        fileFound = fileInfo.exists();
    }

    chdir(m_tempLocation.toStdString().c_str());

    QString newFilename=m_tempLocation+filename;
    if(fileFound) {
        QFile::copy(qrcFilename, newFilename);
    }

    return newFilename;
}
