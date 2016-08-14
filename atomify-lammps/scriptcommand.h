#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <QString>

class ScriptCommand
{
public:
    enum class Type {NoCommand, File, Editor, SingleCommand, SkipLammpsTick};
    explicit ScriptCommand(QString command = "", Type type = Type::NoCommand, int line = -1, int lineInRootFile = -1, QString filename = "", QString path = "");

    const QString& command() const;
    Type type() const;
    int line() const;
    int lineInRootFile() const;
    const QString& filename() const;
    const QString& path() const;

private:
    QString m_command;
    Type m_type = Type::NoCommand;
    int m_line = -1;
    int m_lineInRootFile = -1;
    QString m_filename;
    QString m_path;
};

#endif // SCRIPTCOMMAND_H
