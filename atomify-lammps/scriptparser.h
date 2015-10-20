#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H
#include <QString>
#include <functional>

class ScriptParser
{
private:
    QString regexFloat = QString("\\d*.\\d*");
    QString regexInt = QString("\\d*");
    QString regexColor = QString("#[0-9a-fA-F]{6,6}");
    QString regexTabOrSpace = QString("\\s*|\\t*");
    QString regexWord = QString("\\w*");
public:
    ScriptParser();
    bool isAtomColorAndSize(QString command);
    void AtomColorAndSize(QString command, std::function<void(float scale, QString color, int atomType)> action);
    bool isAtomType(QString command);
    void atomType(QString command, std::function<void(QString atomTypeName, int atomType)> action);
    bool isEditorCommand(QString command);
    bool isInclude(QString command);
    QString includePath(QString command);
    bool isDisableAllEnsembleFixes(QString command);
};

#endif // SCRIPTPARSER_H
