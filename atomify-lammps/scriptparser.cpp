#include "scriptparser.h"
#include <string>
#include <sstream>
#include <QRegularExpression>
#include <QStringList>
#include <QDebug>

ScriptParser::ScriptParser()
{

}

bool ScriptParser::isDisableAllEnsembleFixes(QString command) {
    QRegularExpression regex("^disableAllEnsembleFixes$");
    return regex.match(command).hasMatch();
}

bool ScriptParser::isAtomColorAndSize(QString command)
{
    QRegularExpression regex(QString("^(?:atom)(?:%1)(%2)(?:%1)(%3)(?:%1)(%4)$").arg(regexTabOrSpace).arg(regexInt).arg(regexFloat).arg(regexColor));
    return regex.match(command).hasMatch();
}

void ScriptParser::atomColorAndSize(QString command, std::function<void(float scale, QString color, int atomType)> action) {
    QRegularExpression regex(QString("^(?:atom)(?:%1)(%2)(?:%1)(%3)(?:%1)(%4)$").arg(regexTabOrSpace).arg(regexInt).arg(regexFloat).arg(regexColor));
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return;
    float scale = match.captured(2).toFloat(&castOk);
    if(!castOk) return;

    QString color = match.captured(3);

    action(scale, color, atomType);
}

bool ScriptParser::isAtomType(QString command)
{
    QRegularExpression regex("^(atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(\\w*)$");
    return regex.match(command).hasMatch();
}

bool ScriptParser::isBond(QString command)
{
    QRegularExpression regex(QString("^(?:bond)(?:%1)(%2)(?:%3)(%4)(?:%5)(%6)$").arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexFloat));
    return regex.match(command).hasMatch();
}

bool ScriptParser::isSimulationSpeed(QString command)
{
    QRegularExpression regex(QString("^(?:speed)(?:%1)(%2)$").arg(regexTabOrSpace).arg(regexInt));
    return regex.match(command).hasMatch();
}

int ScriptParser::simulationSpeed(QString command) {
    QRegularExpression regex(QString("^(?:speed)(?:%1)(%2)$").arg(regexTabOrSpace).arg(regexInt));
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int speed = match.captured(1).toInt(&castOk);
    if(castOk) return speed;
    else return -1;
}

void ScriptParser::bond(QString command, std::function<void(int atomType1, int atomType2, float bondLength)> action) {
    QRegularExpression regex(QString("^(?:bond)(?:%1)(%2)(?:%3)(%4)(?:%5)(%6)$").arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexFloat));
    QRegularExpressionMatch match = regex.match(command);
    bool castOk;
    int atomType1 = match.captured(1).toInt(&castOk);
    if(!castOk) return;
    int atomType2 = match.captured(2).toInt(&castOk);
    if(!castOk) return;
    float bondLength = match.captured(3).toFloat(&castOk);
    if(!castOk) return;

    action(atomType1, atomType2, bondLength);
}

bool ScriptParser::isStaticSystem(QString command)
{
    QRegularExpression regex("^staticSystem$");
    return regex.match(command).hasMatch();
}

bool ScriptParser::isDisableBonds(QString command)
{
    QRegularExpression regex("^disableBonds$");
    return regex.match(command).hasMatch();
}

void ScriptParser::atomType(QString command, std::function<void(QString atomTypeName, int atomType)> action)
{
    QRegularExpression regex ("^(?:atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(\\w*)$");
    QRegularExpressionMatch match = regex.match(command);
    int atomType = match.captured(1).toInt();
    QString atomTypeName = match.captured(2);
    action(atomTypeName, atomType);
}

bool ScriptParser::isEditorCommand(QString command)
{
    command = command.trimmed();
    return command.startsWith("#/");
}

bool ScriptParser::isInclude(QString command)
{
    std::string commandString = command.toStdString();
    std::stringstream command_ss(commandString);
    std::string word;
    if(command_ss >> word) {
        if(word.compare("include") == 0) {
            return true;
        }
    }
    return false;
}

QString ScriptParser::includePath(QString command)
{
    QRegularExpression regexInsideQuotes("include\\s*\"([^\"]*)");
    QRegularExpression regexNoQuotes("include\\s*([^\\s]*)");
    if(command.contains(regexInsideQuotes)) {
        QRegularExpressionMatch match = regexInsideQuotes.match(command);
        return match.captured(1);
    }

    if(command.contains(regexNoQuotes)) {
        QRegularExpressionMatch match = regexNoQuotes.match(command);
        return match.captured(1);
    }
    return QString("");
}
