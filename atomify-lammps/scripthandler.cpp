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
#include "LammpsWrappers/bonds.h"

ScriptHandler::ScriptHandler() :
    m_mutex(QMutex::Recursive)
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

    if(m_currentCommand.type() == ScriptCommand::Type::Editor) setCurrentLine(m_currentCommand.line());

    return m_currentCommand;
}

Atoms *ScriptHandler::atoms() const
{
    return m_atoms;
}

ScriptParser &ScriptHandler::parser() { return m_parser; }


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

void ScriptHandler::setAtoms(Atoms *atoms)
{
    m_atoms = atoms;
}

void ScriptHandler::setWorkingDirectory(QUrl fileName) {
    QFileInfo fileInfo(fileName.toLocalFile());
    if(!fileInfo.exists()) return;
    QString currentDir = fileInfo.absoluteDir().path();
    QByteArray currentDirBytes = currentDir.toUtf8();
    chdir(currentDirBytes.constData());
}

void ScriptHandler::runFile(QString filename)
{
    QString tmpFileName = filename;
    bool isQRC = tmpFileName.contains(QRegularExpression("^qrc\:\/"));
    if(isQRC) {
        tmpFileName.replace(QRegularExpression("^qrc\:\/"), "");
        tmpFileName = m_tempLocation + QDir::separator() + tmpFileName;
    }
    QFileInfo fileInfo(tmpFileName);

    QString currentDir = fileInfo.absoluteDir().path();
    QByteArray currentDirBytes = currentDir.toUtf8();
    chdir(currentDirBytes.constData());

    QFile f(tmpFileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file: " << tmpFileName;
        return;
    }

    QString script(f.readAll());
    runScript(script, ScriptCommand::Type::File, tmpFileName, currentDir);
}

int ScriptHandler::currentLine() const
{
    return m_currentLine;
}

bool ScriptHandler::parseLammpsCommand(QString command, LAMMPSController *lammpsController) {
    if(m_parser.isEditorCommand(command)) {
        command = command.trimmed();
        command.remove(0,2);

        if(m_parser.isDisableAllEnsembleFixes(command)) {
            lammpsController->disableAllEnsembleFixes();
            return true;
        }

        if(m_parser.isSimulationSpeed(command)) {
            // TODO: set simulation speed here
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
            // TODO: Switch arguments so they match the m_atoms->setAtomType call
            if(m_atoms) m_atoms->setAtomType(atomType, atomTypeName);
        });
        return;
    }

    if(m_parser.isBond(command)) {
        m_parser.bond(command, [&](int atomType1, int atomType2, float bondLength) {
            if(m_atoms) {
                if(m_atoms->bonds()->bondLengths().size() > std::max(atomType1, atomType2)) {
                    m_atoms->bonds()->bondLengths()[atomType1][atomType2] = bondLength;
                    m_atoms->bonds()->bondLengths()[atomType2][atomType1] = bondLength;
                    m_atoms->bonds()->setEnabled(true);
                }
                qDebug() << "HERE Max bond thing: " << m_atoms->bonds()->maxBondLength();
            }
        });
    }

    if(m_parser.isAtomColorAndSize(command)) {
        m_parser.atomColorAndSize(command, [&](float radius, QString color, int atomType) {
            if(m_atoms) {
                m_atoms->setAtomColorAndScale(atomType, color, radius);
            }
        });
        return;
    }

    if(m_lammpsState) {
        if(m_parser.isStaticSystem(command)) {
            m_lammpsState->staticSystem = true;
            return;
        }
    }

    if(m_parser.isDisableBonds(command)) {
        m_atoms->bonds()->setEnabled(false);
    }

}

void ScriptHandler::doRunScript(QString script, ScriptCommand::Type type, QString filename, QString currentDir) {
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
                runFile(filename);
                if(!currentDir.isEmpty()) {
                    // TODO: fix so editor sends in default path
                    QByteArray currentDirBytes = currentDir.toUtf8();
                    chdir(currentDirBytes.constData());
                }
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

void ScriptHandler::runScript(QString script, ScriptCommand::Type type, QString filename, QString currentDir) {
    QMutexLocker locker(&m_mutex);
    doRunScript(script, type, filename, currentDir);
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

    QByteArray tempLocationBytes = m_tempLocation.toUtf8();
    chdir(tempLocationBytes.constData());

    QString newFilename=m_tempLocation+filename;
    if(fileFound) {
        QFile::copy(qrcFilename, newFilename);
    }

    return newFilename;
}

void ScriptHandler::setCurrentLine(int currentLine)
{
    if (m_currentLine == currentLine)
        return;

    m_currentLine = currentLine;
    emit currentLineChanged(currentLine);
}
