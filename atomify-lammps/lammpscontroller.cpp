#include "lammpscontroller.h"

#include "lammps/library.h"
#include "lammps/input.h"
#include "lammps/variable.h"
#include "lammps/update.h"
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <sstream>
#include <string>
#include <cstdlib>
using namespace std;

LAMMPSController::LAMMPSController()
{

}

LAMMPSController::~LAMMPSController()
{

}
LAMMPS *LAMMPSController::lammps() const
{
    return m_lammps;
}

void LAMMPSController::setLammps(LAMMPS *lammps)
{
    m_lammps = lammps;
}


int LAMMPSController::simulationSpeed() const
{
    return m_simulationSpeed;
}

void LAMMPSController::setSimulationSpeed(int simulationSpeed)
{
    m_simulationSpeed = simulationSpeed;
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
    runCommand(command.toStdString().c_str());
}

void LAMMPSController::runCommand(const char *command)
{
    if(m_lammps == 0) {
        qDebug() << "Warning, trying to run a LAMMPS command with no LAMMPS object.";
        qDebug() << "Command: " << command;
        return;
    }
    qDebug() << command;
    lammps_command((void*)m_lammps, (char*) command);
}

QString LAMMPSController::copyDataFileToReadablePath(QString filename)
{
    QFile inFile(":/scripts/"+filename);
    if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: could not open file "+inFile.fileName();
        return "";
    }

    QString content = inFile.readAll();
    inFile.close();

    QString documentsLocation = QStandardPaths::locate(QStandardPaths::TempLocation, QString(), QStandardPaths::LocateDirectory);
    QString newFilename=documentsLocation+filename;
    QFile outFile(newFilename);
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: could not open file "+outFile.fileName();
        return "";
    }

    outFile.write(content.toStdString().c_str());
    outFile.close();
    return newFilename;
}

void LAMMPSController::processCommand(QString command) {
    qDebug() << "Processing " << command;
    stringstream command_ss(command.toStdString());
    string word;
    QString processedCommand;

    int wordCount = 0;
    while(command_ss >> word) {
        if(word.find("ext://") != std::string::npos) {
            word.erase(0, 6); // Remove the ext:// prefix
            word = copyDataFileToReadablePath(QString::fromStdString(word)).toStdString();
        }

        if(wordCount == 0 && word.compare("run") == 0) {
            command_ss >> word; // Next word is the number of timesteps
            int numberOfTimesteps = atoi(word.c_str());
            if(numberOfTimesteps == 0) {
                // We probably need to read it as a variable in lammps
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

            m_runCommandActive = true;
            m_runCommandStart = m_lammps->update->ntimestep;
            m_runCommandEnd = m_runCommandStart + numberOfTimesteps;
            return;
        }

        processedCommand.append(QString::fromStdString(word)).append(" ");
        wordCount++;
    }

    runCommand(processedCommand);
}

void LAMMPSController::tick()
{
    if(m_runCommandActive > 0) {
        unsigned int currentTimestep = m_lammps->update->ntimestep;
        int maxSimulationSpeed = m_runCommandEnd - currentTimestep;
        int simulationSpeed = min(m_simulationSpeed, maxSimulationSpeed);
        if(currentTimestep == m_runCommandStart) {
            runCommand(QString("run %1 pre yes post no start %2 stop %3").arg(simulationSpeed).arg(m_runCommandStart).arg(m_runCommandEnd));
        } else {
            runCommand(QString("run %1 pre no post no start %2 stop %3").arg(simulationSpeed).arg(m_runCommandStart).arg(m_runCommandEnd));
        }

        currentTimestep = m_lammps->update->ntimestep;
        m_runCommandActive = currentTimestep < m_runCommandEnd;
    } else if(m_commands.size() > 0) {
        QString command = m_commands.front();
        m_commands.pop_front();
        processCommand(command);
    } else {
        runCommand(QString("run %1 pre no post no").arg(m_simulationSpeed));
    }
}

void LAMMPSController::loadLammpsScript(QString filename)
{
    QString lammpsScript_qstring = readFile(filename);

    if (!lammpsScript_qstring.isEmpty())
    {
        std::stringstream lammpsScript(lammpsScript_qstring.toStdString());
        std::string command;

        while(std::getline(lammpsScript,command,'\n')){
            stringstream command_ss(command);
            string word;
            if(command_ss >> word) {
                if(word.compare("include") == 0) {
                    if(command_ss >> word) {
                        word.erase(0, 6); // Remove the ext:// prefix
                        word = copyDataFileToReadablePath(QString::fromStdString(word)).toStdString();
                        loadLammpsScript(QString::fromStdString(word));
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
    m_lammps->screen = NULL;
    m_commands.clear();

    m_runCommandActive = false;
}
