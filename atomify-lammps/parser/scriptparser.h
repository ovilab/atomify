#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H
#include <QString>
#include <QColor>
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
    bool atomColorAndSize(QString command, std::function<void(int atomType, QString color, float scale)> action = {});
    bool atomType(QString command, std::function<void(int atomType, QString atomTypeName)> action = {});
    bool atomColor(QString command, std::function<void (int, QColor)> action = {});
    bool bond(QString command, std::function<void (int atomType1, int atomType2, float bondLength)> action = {});
    QString includePath(QString command);
    int simulationSpeed(QString command);
    bool isEditorCommand(QString command);
};

#endif // SCRIPTPARSER_H
