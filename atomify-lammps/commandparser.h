#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <QString>
#include <functional>
#include <QRegularExpression>

class CommandParser
{
public:
    CommandParser();
    void parseFile(QString fileName);
    class AtomifySimulator *simulator() const;
    void setSimulator(class AtomifySimulator *simulator);

private:
    class AtomifySimulator *m_simulator = nullptr;
    QString regexFloat = QString("\\d*.\\d*");
    QString regexInt = QString("\\d*");
    QString regexColor = QString("#[0-9a-fA-F]{6,6}");
    QString regexTabOrSpace = QString("\\s*|\\t*");
    QString regexWord = QString("\\w*");

    void parseCommand(QString command);
    void atomColorAndSize(QString command);
    void atomColor(QString command);
    void atomType(QString command);
    void bond(QString command);
    void atomSize(QString command);
    void cameraPosition(QString command);
};

#endif // COMMANDPARSER_H
