#ifndef SCRIPTCOMMAND_H
#define SCRIPTCOMMAND_H

#include <QString>

class ScriptCommand
{
public:
    enum class Type {NoCommand, File, Editor, SingleCommand};
    explicit ScriptCommand(QString command = "", Type type = Type::Editor, int line = -1, QString fileName = "", QString path = "");

    const QString& command() const;
    Type type() const;
    int line() const;
    QString fileName() const;
    QString path() const;
    bool canProcessSimulatorControls() const;
    void setCanProcessSimulatorControls(bool canProcessSimulatorControls);

private:
    QString m_command;
    QString m_fileName;
    QString m_path;
    Type m_type = Type::NoCommand;
    bool m_canProcessSimulatorControls = false;
    int m_line = -1;
};

#endif // SCRIPTCOMMAND_H
