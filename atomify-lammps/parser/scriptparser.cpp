#include "scriptparser.h"
#include <string>
#include <sstream>
#include <QRegularExpression>
#include <QStringList>
#include <QDebug>

ScriptParser::ScriptParser()
{

}

bool ScriptParser::isUnsupportedCommand(QString command)
{
    command = command.trimmed();

    QStringList unSupportedCommands = {
        QString("loop"),
        QString("jump"),
        QString("temper"),
        QString("tad"),
        QString("rerun"),
        QString("quit"),
        QString("processors"),
        QString("prd"),
        QString("neb"),
        QString("label")
    };
    foreach(QString str, unSupportedCommands) {
        if(command.startsWith(str)) {
            return true;
        }
    }

    return false;
}

bool ScriptParser::atomColorAndSize(QString command, std::function<void(int atomType, QString color, float scale)> action) {
    QRegularExpression regex(QString("^(?:atom)(?:%1)(%2)(?:%1)(%3)(?:%1)(%4)$").arg(regexTabOrSpace).arg(regexInt).arg(regexFloat).arg(regexColor));
    QRegularExpressionMatch match = regex.match(command);
    if(!match.hasMatch()) return false;

    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return false;
    float scale = match.captured(2).toFloat(&castOk);
    if(!castOk) return false;

    QString color = match.captured(3);

    if(action) action(atomType, color, scale);
    return true;
}

int ScriptParser::simulationSpeed(QString command) {
    QRegularExpression regex(QString("^(?:speed)(?:%1)(%2)$").arg(regexTabOrSpace).arg(regexInt));
    QRegularExpressionMatch match = regex.match(command);
    if(!match.hasMatch()) return false;
    bool castOk;
    int speed = match.captured(1).toInt(&castOk);
    if(castOk) return speed;
    else return 0;
}

bool ScriptParser::bond(QString command, std::function<void(int atomType1, int atomType2, float bondLength)> action) {
    QRegularExpression regex(QString("^(?:bond)(?:%1)(%2)(?:%3)(%4)(?:%5)(%6)$").arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexInt).arg(regexTabOrSpace).arg(regexFloat));
    QRegularExpressionMatch match = regex.match(command);
    if(!match.hasMatch()) return false;
    bool castOk;
    int atomType1 = match.captured(1).toInt(&castOk);
    if(!castOk) return false;
    int atomType2 = match.captured(2).toInt(&castOk);
    if(!castOk) return false;
    int bondLength = match.captured(3).toFloat(&castOk);
    if(!castOk) return false;

    if(action) action(atomType1, atomType2, bondLength);
    return true;
}

bool ScriptParser::atomType(QString command, std::function<void(int atomType, QString atomTypeName)> action)
{
    QRegularExpression regex("^(?:atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(\\w*)$");
    QRegularExpressionMatch match = regex.match(command);
    if(!match.hasMatch()) return false;
    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return false;
    QString atomTypeName = match.captured(2);
    if(action) action(atomType, atomTypeName);
    return true;
}

bool ScriptParser::atomColor(QString command, std::function<void(int atomType, QColor color)> action)
{
    QRegularExpression regex ("^(?:atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(?:color)(?:\\s*|\\t*)(#\\w*?|\\w*)$");
    QRegularExpressionMatch match = regex.match(command);
    if(!match.hasMatch()) return false;
    bool castOk;
    int atomType = match.captured(1).toInt(&castOk);
    if(!castOk) return false;
    QString colorString = match.captured(2);
    QColor color(colorString);

    if(action) action(atomType, color);
    return true;
}

QString ScriptParser::includePath(QString command)
{
    QRegularExpression regexInsideQuotes("include\\s*\"([^\"]*)");
    QRegularExpression regexInsideSingleQuotes("include\\s*'([^']*)");
    QRegularExpression regexNoQuotes("include\\s*([^\\s]*)");
    if(command.contains(regexInsideQuotes)) {
        QRegularExpressionMatch match = regexInsideQuotes.match(command);
        return match.captured(1);
    }

    if(command.contains(regexInsideSingleQuotes)) {
        QRegularExpressionMatch match = regexInsideSingleQuotes.match(command);
        return match.captured(1);
    }

    if(command.contains(regexNoQuotes)) {
        QRegularExpressionMatch match = regexNoQuotes.match(command);
        return match.captured(1);
    }
    return QString("");
}

bool ScriptParser::isEditorCommand(QString command)
{
    command = command.trimmed();
    return command.startsWith("#/");
}
