#include "scriptparser.h"
#include <string>
#include <sstream>

ScriptParser::ScriptParser()
{

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

