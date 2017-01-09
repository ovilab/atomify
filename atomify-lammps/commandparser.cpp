#include "commandparser.h"

#include <QFile>
#include <QTextStream>
#include "mysimulator.h"
#include "LammpsWrappers/system.h"
#include "LammpsWrappers/atoms.h"
#include "LammpsWrappers/bonds.h"

CommandParser::CommandParser()
{

}

void CommandParser::parseFile(QString fileName, bool moveCamera)
{
    moveCameraPosition = false;
    moveCameraViewCenter = false;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if(line.trimmed().startsWith("#/")) {
            line.remove(0,2);
            parseCommand(line);
        }
    }

    file.close();

    if(!moveCamera) return;

    if(moveCameraPosition && moveCameraViewCenter) {
        emit m_simulator->newCameraPositionAndViewCenterRequest(m_simulator->cameraPositionRequest(), m_simulator->cameraViewCenterRequest());
    } else if(moveCameraPosition) {
        emit m_simulator->newCameraPositionRequest(m_simulator->cameraPositionRequest());
    } else if(moveCameraViewCenter) {
        emit m_simulator->newViewCenterRequest(m_simulator->cameraViewCenterRequest());
    }
}

AtomifySimulator *CommandParser::simulator() const
{
    return m_simulator;
}

void CommandParser::setSimulator(AtomifySimulator *simulator)
{
    m_simulator = simulator;
}

void CommandParser::parseCommand(QString command)
{
    atomColor(command);
    atomType(command);
    bond(command);
    atomColorAndSize(command);
    atomSize(command);

    // TODO: make this much better :P
    moveCameraPosition |= cameraPosition(command);
    moveCameraViewCenter |= viewCenter(command);
}

void CommandParser::atomColor(QString command)
{
    QRegularExpression regex ("^(?:atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(?:color)(?:\\s*|\\t*)(#\\w*?|\\w*)$");
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return;
    QString colorString = match.captured(2);
    QColor color(colorString);
    m_simulator->system()->atoms()->setAtomColor(atomType, color);
}

void CommandParser::atomType(QString command)
{
    QRegularExpression regex("^(?:atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(\\w*)$");
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return;
    QString atomTypeName = match.captured(2);
    m_simulator->system()->atoms()->setAtomType(atomType, atomTypeName);
}

void CommandParser::atomSize(QString command)
{
    QRegularExpression regex("^(?:atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(\\d*.\\d|\\d*)$");
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return;
    float radius = match.captured(2).toFloat(&castOk);
    if(!castOk) return;
    m_simulator->system()->atoms()->setAtomSize(atomType, radius);
}

bool CommandParser::cameraPosition(QString command)
{
    QStringList words = command.split(" ");
    if(words.size() != 5) return false;
    if(words[0] != "camera" || words[1].toLower() != "position") return false;
    float x,y,z;
    bool castOk;
    x = words[2].toFloat(&castOk);
    if(!castOk) return false;
    y = words[3].toFloat(&castOk);
    if(!castOk) return false;
    z = words[4].toFloat(&castOk);
    if(!castOk) return false;

    m_simulator->setCameraPositionRequest(QVector3D(x,y,z));
    return true;
}

bool CommandParser::viewCenter(QString command)
{
    QStringList words = command.split(" ");
    if(words.size() != 5) return false;
    if(words[0] != "camera" || words[1].toLower() != "viewcenter") return false;
    float x,y,z;
    bool castOk;
    x = words[2].toFloat(&castOk);
    if(!castOk) return false;
    y = words[3].toFloat(&castOk);
    if(!castOk) return false;
    z = words[4].toFloat(&castOk);
    if(!castOk) return false;

    m_simulator->setCameraViewCenterRequest(QVector3D(x,y,z));
    return true;
}

void CommandParser::bond(QString command) {
    QRegularExpression regex(QString("^(?:bond)(?:%1)(%2)(?:%3)(%4)(?:%5)(%6)$").arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexFloat));
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int atomType1 = match.captured(1).toInt(&castOk);
    if(!castOk) return;
    int atomType2 = match.captured(2).toInt(&castOk);
    if(!castOk) return;
    int bondLength = match.captured(3).toFloat(&castOk);
    if(!castOk) return;

    Bonds *bonds = m_simulator->system()->atoms()->bonds();
    if(bonds->bondLengths().size() > std::max(atomType1, atomType2)) {
        bonds->bondLengths()[atomType1][atomType2] = bondLength;
        bonds->bondLengths()[atomType2][atomType1] = bondLength;
        bonds->setEnabled(true);
    }
}

void CommandParser::atomColorAndSize(QString command) {
    QRegularExpression regex(QString("^(?:atom)(?:%1)(%2)(?:%1)(%3)(?:%1)(%4)$").arg(regexTabOrSpace).arg(regexInt).arg(regexFloat).arg(regexColor));
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return;
    float scale = match.captured(2).toFloat(&castOk);
    if(!castOk) return;
    QString color = match.captured(3);

    m_simulator->system()->atoms()->setAtomColorAndScale(atomType, color, scale);
}
