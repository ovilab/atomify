#include "scriptparser.h"
#include <string>
#include <sstream>
#include <QRegExp>
#include <QStringList>
#include <QDebug>

ScriptParser::ScriptParser()
{

}

bool ScriptParser::isDisableAllEnsembleFixes(QString command) {
    QRegExp regex("disableAllEnsembleFixes");
    return regex.exactMatch(command);
}

bool ScriptParser::isAtomColorAndSize(QString command)
{
    QRegExp regex = QRegExp(QString("(atom)(?:%1)(%2)(?:%1)(%3)(?:%1)(%4)").arg(regexTabOrSpace).arg(regexInt).arg(regexFloat).arg(regexColor));
    return regex.exactMatch(command);
}

void ScriptParser::AtomColorAndSize(QString command, std::function<void(float scale, QString color, int atomType)> action) {
    QRegExp regex = QRegExp(QString("(atom)(?:%1)(%2)(?:%1)(%3)(?:%1)(%4)").arg(regexTabOrSpace).arg(regexInt).arg(regexFloat).arg(regexColor));
    regex.exactMatch(command);
    QString color = regex.capturedTexts().at(regex.capturedTexts().length()-1);
    QString scale = regex.capturedTexts().at(regex.capturedTexts().length()-2);
    QString atomType = regex.capturedTexts().at(regex.capturedTexts().length()-3);
    bool castOk;
    float scalef = scale.toFloat(&castOk);
    if(!castOk) return;
    int atomTypeInt = atomType.toInt(&castOk);
    if(!castOk) return;

    action(scalef, color, atomTypeInt);
}

bool ScriptParser::isAtomType(QString command)
{
    QRegExp regex("(atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(\\w*)");
    return regex.exactMatch(command);
}

bool ScriptParser::isStaticSystem(QString command)
{
    QRegExp regex("staticSystem");
    return regex.exactMatch(command);
}

void ScriptParser::atomType(QString command, std::function<void(QString atomTypeName, int atomType)> action)
{
    QRegExp regex("(atom)(?:\\s*|\\t*)(\\d*)(?:\\s*|\\t*)(\\w*)");
    regex.exactMatch(command);
    QString atomTypeName = regex.capturedTexts().last();
    int atomType = regex.capturedTexts().at(regex.capturedTexts().length()-2).toInt();
    action(atomTypeName, atomType);
}

bool ScriptParser::isGUICommand(QString command) {
    command.remove(0,2);
    bool isGUICommand = false;
    if(isAtomType(command)) isGUICommand = true;
    if(isAtomColorAndSize(command)) isGUICommand = true;
    if(isStaticSystem(command)) isGUICommand = true;
    return isGUICommand;
}

bool ScriptParser::isEditorCommand(QString command)
{
    command = command.trimmed();
    return command.startsWith("#/");
}

bool ScriptParser::isInclude(QString command)
{
    std::stringstream command_ss(command.toStdString());
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
    std::stringstream command_ss(command.toStdString());
    std::string word;
    command_ss >> word; // This is the word "include"
    if(command_ss >> word) {
        // This will be the filename
        return QString::fromStdString(word);
    } else return QString("");
}

