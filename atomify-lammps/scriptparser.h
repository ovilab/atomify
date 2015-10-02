#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H
#include <QString>

class ScriptParser
{
public:
    ScriptParser();
    bool isInclude(QString command);
    QString includePath(QString command);
};

#endif // SCRIPTPARSER_H
