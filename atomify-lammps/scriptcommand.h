#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <QString>

class ScriptCommand
{
public:
    enum class Type {NoCommand, File, Editor, SingleCommand, SkipLammpsTick};

    ScriptCommand(QString command = "", Type type = Type::NoCommand, int line = -1, QString filename = "");

    const QString& command() const;
    Type type() const;
    int line() const;
    const QString& filename() const;

private:
    QString m_command;
    Type m_type = Type::NoCommand;
    int m_line = -1;
    QString m_filename;
};

#endif // SCRIPTCOMMAND_H
