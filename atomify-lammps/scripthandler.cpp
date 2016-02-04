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
#include <QDirIterator>
#include <QRegularExpression>
#include <QUuid>
#include <fstream>

ScriptHandler::ScriptHandler()
{
    m_tempLocation = QStandardPaths::locate(QStandardPaths::TempLocation, QString(), QStandardPaths::LocateDirectory)
            + "atomify-lammps"; // + QUuid::createUuid().toString();

    QDir dir;
    dir.mkpath(m_tempLocation);

    QDirIterator it(":/simulations", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString qrcFileName = it.next();
        QFileInfo qrcFileInfo(qrcFileName);
        if(qrcFileInfo.isDir()) {
            continue;
        }
        QString newFileName = qrcFileName;
        newFileName.replace(QRegularExpression("^\:\/"), "");
        newFileName = m_tempLocation + QDir::separator() + newFileName;
        QFileInfo newFileInfo(newFileName);
        dir.mkpath(newFileInfo.absoluteDir().path());
        if(QFile::exists(newFileName)) {
            if(!QFile::remove(newFileName)) {
                qFatal("Could not remove existing");
            }
        }
        if(!QFile::copy(qrcFileName, newFileName)) {
            qFatal("Could not copy");
        }
    }
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
    doRunScript(readFile(filename), ScriptCommand::Type::File, filename);
}

AtomStyle *ScriptHandler::atomStyle() const
{
    return m_atomStyle;
}


LammpsState *ScriptHandler::lammpsState() const
{
    return m_lammpsState;
}

void ScriptHandler::setLammpsState(LammpsState *lammpsState)
{
    m_lammpsState = lammpsState;
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
    QString tmpFileName = filename;
    tmpFileName.replace(QRegularExpression("^qrc\:\/"), "");
    tmpFileName = m_tempLocation + QDir::separator() + tmpFileName;
    QFileInfo fileInfo(tmpFileName);
    qDebug() << "Changing directory to" << fileInfo.absoluteDir().path();
    chdir(fileInfo.absoluteDir().path().toStdString().c_str());
    std::ofstream out("lol.txt");
    out << "woot";

    QString fileNameString = QQmlFile::urlToLocalFileOrQrc(filename);
    QFile f(fileNameString);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file: " << filename;
        return;
    }

    QString script(f.readAll());
    runScript(script, ScriptCommand::Type::File, filename);
}

bool ScriptHandler::parseLammpsCommand(QString command, LAMMPSController *lammpsController) {
    if(m_parser.isEditorCommand(command)) {
        command = command.trimmed();
        command.remove(0,2);

        if(m_parser.isDisableAllEnsembleFixes(command)) {
            lammpsController->disableAllEnsembleFixes();
            return true;
        }
    }

    return false;
}

void ScriptHandler::parseGUICommand(QString command)
{
    command = command.trimmed();
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

    if(m_lammpsState) {
        if(m_parser.isStaticSystem(command)) {
            m_lammpsState->staticSystem = true;
        }
    }
}

void ScriptHandler::doRunScript(QString script, ScriptCommand::Type type, QString filename) {
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

            // TODO add back parsing of included files, remember to chdir back after
//            if(m_parser.isInclude(currentCommand)) {
//                QString filename = m_parser.includePath(currentCommand);
//                loadScriptFromFile(filename);
//                currentCommand.clear(); lineNumber++; continue; // This line is complete
//            }

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

void ScriptHandler::runScript(QString script, ScriptCommand::Type type, QString filename) {
    QMutexLocker locker(&m_mutex);
    doRunScript(script, type, filename);
}

void ScriptHandler::runCommand(QString command, bool addToPreviousCommands)
{
    QMutexLocker locker(&m_mutex);
    auto commandObject = ScriptCommand(command, ScriptCommand::Type::SingleCommand);
    if(addToPreviousCommands) m_previousSingleCommands.push_back(commandObject);
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
    qWarning() << "Copy data chdir!!!!!";
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
