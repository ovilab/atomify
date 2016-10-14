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
    bool isUnsupportedCommand(QString command);
    bool isAtomColorAndSize(QString command);
    bool isDisableBonds(QString command);
    void atomColorAndSize(QString command, std::function<void(float scale, QString color, int atomType)> action);
    void bond(QString command, std::function<void (int atomType1, int atomType2, float bondLength)> action);
    bool isAtomType(QString command);
    bool isBond(QString command);
    void atomType(QString command, std::function<void(QString atomTypeName, int atomType)> action);
    bool isEditorCommand(QString command);
    bool isInclude(QString command);
    QString includePath(QString command);
    bool isDisableAllEnsembleFixes(QString command);
    bool isGUICommand(QString command);
    bool isSimulationSpeed(QString command);
    int simulationSpeed(QString command);
};

#endif // SCRIPTPARSER_H
