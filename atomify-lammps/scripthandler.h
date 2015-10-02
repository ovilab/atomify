#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QObject>
#include <QDebug>
#include <QQueue>
#include <QString>
#include <QPair>
#include "scriptparser.h"

struct CommandInfo {
    enum class Type {NA, File, Editor, SingleCommand};
    Type type;
    QString filename;
    int line = 0;
    CommandInfo() { type = Type::NA; line = -1; }

    CommandInfo(Type type) { this->type = type; }

    CommandInfo(Type type, int line, QString filename) {
        this->type = type;
        this->line = line;
        this->filename = filename;
    }
};

class ScriptHandler : public QObject
{
    Q_OBJECT

private:
    ScriptParser m_parser;
    QMutex m_mutex;
    QQueue<QPair<QString, CommandInfo> > m_lammpsCommandStack;
    QPair<QString, CommandInfo> m_currentCommand;
public:
    ScriptHandler();
    QPair<QString, CommandInfo> nextCommand();

    void loadScriptFromFile(QString filename);
public slots:
    void runScript(QString script, CommandInfo::Type type = CommandInfo::Type::Editor, QString filename = "");
    void runCommand(QString command);
    void reset();

signals:

};

#endif // SCRIPTHANDLER_H
