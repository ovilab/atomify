#include "lammpscontroller.h"

#include "lammps/integrate.h"
#include "lammps/library.h"
#include "lammps/input.h"
#include "lammps/variable.h"
#include "lammps/update.h"
#include "lammps/modify.h"
#include "CPcompute.h"
#include <stdio.h>

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <iostream>
using namespace std;

LAMMPSController::LAMMPSController()
{

}

LAMMPSController::~LAMMPSController()
{
    if(m_lammps) {
        lammps_close((void*)m_lammps);
        m_lammps = 0;
    }
    m_commands.clear();
}

LAMMPS_NS::LAMMPS *LAMMPSController::lammps() const
{
    return m_lammps;
}

void LAMMPSController::setLammps(LAMMPS *lammps)
{
    m_lammps = lammps;
}


int LAMMPSController::simulationSpeed() const
{
    return m_state.simulationSpeed;
}

void LAMMPSController::setSimulationSpeed(int simulationSpeed)
{
    if(m_state.simulationSpeed != simulationSpeed) {
        m_state.simulationSpeed = simulationSpeed;
    }
}


LammpsOutput *LAMMPSController::outputParser() const
{
    return m_outputParser;
}

void LAMMPSController::setOutputParser(LammpsOutput *outputParser)
{
    m_outputParser = outputParser;
}


QMap<QString, CPCompute *> LAMMPSController::computes() const
{
    return m_computes;
}

void LAMMPSController::setComputes(const QMap<QString, CPCompute *> &computes)
{
    m_computes = computes;
}

QString LAMMPSController::readFile(QString filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file: " << file.fileName();
        return "";
    }

    return file.readAll();
}

void LAMMPSController::runCommand(QString command) {
    // qDebug() << "Running command: " << command;
    runCommand(command.toStdString().c_str());
}

void LAMMPSController::runCommand(const char *command)
{
    if(m_lammps == 0) {
        qDebug() << "Warning, trying to run a LAMMPS command with no LAMMPS object.";
        qDebug() << "Command: " << command;
        return;
    }
    lammps_command((void*)m_lammps, (char*) command);
}

QString LAMMPSController::copyDataFileToReadablePath(QString filename)
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

    QString tempLocation = QStandardPaths::locate(QStandardPaths::TempLocation, QString(), QStandardPaths::LocateDirectory);
    QString newFilename=tempLocation+filename;
    if(fileFound) {
        QFile::copy(qrcFilename, newFilename);
    }

    return newFilename;
}

void LAMMPSController::processCommand(QString command) {
    // Parse one single LAMMPS command. Three notes:
    //   Files are read with full filenames in LAMMPS. Files added as Qt Resrouces have path :/file, which will not work.
    //   We have introduced the notation ext://file which will copy the file to a temp location and is readable with its full filename.
    //
    //   Also, run commands are modified so that run N will be split into smaller run commands to allow smooth visualizations.
    //   This means that if the number of timesteps are given as a LAMMPS variable, we need to extract that.
    //
    //   We also added a fastrun command that act as a normal run command with no visualization.
    stringstream command_ss(command.toStdString());
    string word;
    QString processedCommand;

    int wordCount = 0;
    while(command_ss >> word) {
        if(word.find("ext://") != std::string::npos) {
            word.erase(0, 6); // Remove the ext:// prefix
            word = copyDataFileToReadablePath(QString::fromStdString(word)).toStdString();
        }

        // If this is the first word in a command and it is "run"
        if(wordCount == 0 && word.compare("run") == 0) {
            command_ss >> word; // Next word is the number of timesteps
            int numberOfTimesteps = atoi(word.c_str());
            if(numberOfTimesteps == 0) {
                // We probably need to read it as a variable in lammps.
                string wordCopy = word;
                wordCopy.erase(0,2);
                wordCopy.erase(word.length()-3,1);
                const char *word_c_str = wordCopy.c_str();

                int found = m_lammps->input->variable->find((char*)word_c_str);
                if(found > -1) {
                    char *numberOfTimestepsString = m_lammps->input->variable->retrieve((char*)word_c_str);
                    numberOfTimesteps = atoi(numberOfTimestepsString);
                }
            }

            if(command_ss >> word) {
                qDebug() << "Warning, LAMMPS visualizer doesn't support run commands with arguments. These will be ignored.";
            }

            m_state.runCommandActive = true;
            m_state.runCommandStart = m_lammps->update->ntimestep;
            m_state.runCommandEnd = m_state.runCommandStart + numberOfTimesteps;
            return;
        }

        if(wordCount == 0 && word.compare("fastrun") == 0) {
            processedCommand = "run ";
            wordCount++;
            continue;
        }

        processedCommand.append(QString::fromStdString(word)).append(" ");
        wordCount++;
    }

    runCommand(processedCommand);
}

void LAMMPSController::checkOutput() {
    if(m_outputParser == NULL) return;

    if(m_outputParser->childrenDirty() || m_state.outputNotUpdated) {
        QStringList thermoCommands;
        QString thermoCommand = "thermo_style custom ";

        for(QObject *child: m_outputParser->children()) {
            CPCompute *compute = qobject_cast<CPCompute*>(child);
            if(compute != NULL) {
                int computeid = m_lammps->modify->find_compute(compute->identifier().toStdString().c_str());
                if(computeid >= 0) {
                    QString command = QString("c_").append(compute->identifier()).append(" ");
                    thermoCommand.append(command);
                    thermoCommands.append(command);
                }
            }
        }
        if(thermoCommands.size() > 0) {
            qDebug() << "We have a new thermo command: " << thermoCommand;
        } else qDebug() << "We don't have a new thermo command...";

        m_state.needPreRun = true;
        m_state.outputNotUpdated = false;
    }
}

void LAMMPSController::processComputes()
{
    for(QString key : m_computes.keys()) {
        int computeid = m_lammps->modify->find_compute(key.toStdString().c_str());
        if(computeid < 0) {
            // We need to add it. First check all dependencies
            CPCompute *compute = m_computes[key];
            qDebug() << "Adding " << compute->identifier();
            bool allDependenciesFound = true; // Assume all are found and find potential counterexample
            foreach(QString dependency, compute->dependencies()) {
                computeid = m_lammps->modify->find_compute(dependency.toStdString().c_str());
                if(computeid < 0) {
                    qDebug() << "Didn't find dependency " << dependency;
                    allDependenciesFound = false;
                    break;
                }
            }

            if(allDependenciesFound) {
                qDebug() << "All dependencies found for " << compute->identifier();
                // Now that all dependencies are met we can add this one too
                runCommand(compute->command());
            }
        }
    }
}

void LAMMPSController::loadScriptFromFile(QString filename)
{
    QString lammpsScript_qstring = readFile(filename);

    runScript(lammpsScript_qstring);
}

void LAMMPSController::runScript(QString script)
{
    if(!script.isEmpty())
    {
        // If the file is not empty, load each command and add it to the queue.
        // Now, if there is an include command, load that script too.
        std::stringstream lammpsScript(script.toStdString());
        std::string command;

        while(std::getline(lammpsScript,command,'\n')){
            stringstream command_ss(command);
            string word;
            if(command_ss >> word) {
                if(word.compare("include") == 0) {
                    if(command_ss >> word) {
                        if(word.find("ext://") != std::string::npos) {
                            word.erase(0, 6); // Remove the ext:// prefix
                            word = copyDataFileToReadablePath(QString::fromStdString(word)).toStdString();
                        }
                        loadScriptFromFile(QString::fromStdString(word));
                    } else {
                        qDebug() << "Invalid include statement.";
                        continue;
                    }
                } else {
                    m_commands.push_back(QString::fromStdString(command));
                }
            }
        }
    }
}

void LAMMPSController::reset()
{
    if(m_lammps) {
        lammps_close((void*)m_lammps);
        m_lammps = 0;
    }

    lammps_open_no_mpi(0, 0, (void**)&m_lammps);
    m_state.simulationSpeed = 1;
    // m_lammps->screen = NULL;
    m_commands.clear();
    m_state.needPreRun = true;
    m_state.outputNotUpdated = true;
    m_state.runCommandActive = false;
}

void LAMMPSController::tick()
{
    if(m_outputParser) {
        m_lammps->screen = m_outputParser->file();
    }

    processComputes();

    // If we have an active run command, perform the run command with the current chosen speed.
    if(m_state.runCommandActive > 0) {
        unsigned int currentTimestep = m_lammps->update->ntimestep;
        int maxSimulationSpeed = m_state.runCommandEnd - currentTimestep;

        // Choose the smaller of the number of timesteps left and the current simulation speed.
        int simulationSpeed = min(m_state.simulationSpeed, maxSimulationSpeed);
        if(currentTimestep == m_state.runCommandStart) {
            // If this is the first timestep in this run command, execute the pre yes command to prepare the full run command.
            runCommand(QString("run %1 pre yes post no start %2 stop %3").arg(simulationSpeed).arg(m_state.runCommandStart).arg(m_state.runCommandEnd));
        } else {
            runCommand(QString("run %1 pre no post no start %2 stop %3").arg(simulationSpeed).arg(m_state.runCommandStart).arg(m_state.runCommandEnd));
        }
        checkOutput();
        m_state.needPreRun = true;
        currentTimestep = m_lammps->update->ntimestep;
        m_state.runCommandActive = currentTimestep < m_state.runCommandEnd;
    } else if(m_commands.size() > 0) {
        // If the command stack has any commands left, process them.
        QString command = m_commands.front();
        m_commands.pop_front();
        processCommand(command);
    } else {
        // If no commands are queued, just perform a normal run command with the current simulation speed.
        if(m_state.needPreRun) {
            m_state.needPreRun = false;
            runCommand(QString("run %1 pre yes post no").arg(m_state.simulationSpeed));
        } else {
            runCommand(QString("run %1 pre no post no").arg(m_state.simulationSpeed));
        }
        checkOutput();
    }

}
