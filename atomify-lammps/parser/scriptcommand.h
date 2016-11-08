#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <QString>

class ScriptCommand
{
public:
    enum class Type {NoCommand, File, Editor, SingleCommand};
    explicit ScriptCommand(QString command = "", Type type = Type::Editor, int line = -1);

    const QString& command() const;
    Type type() const;
    int line() const;

private:
    QString m_command;
    Type m_type = Type::NoCommand;
    int m_line = -1;
};

#endif // SCRIPTCOMMAND_H
