#ifndef LAMMPSERROR_H
#define LAMMPSERROR_H
#include <QObject>

class LammpsError : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString command READ command WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(QString scriptFile READ scriptFile WRITE setScriptFile NOTIFY scriptFileChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(int line READ line WRITE setLine NOTIFY lineChanged)

public:
    LammpsError(QObject *parent = nullptr);
    QString scriptFile() const;
    QString message() const;
    int line() const;
    QString command() const;

public slots:
    void setScriptFile(QString scriptFile);
    void setMessage(QString message);
    void setLine(int line);
    void setCommand(QString command);

signals:
    void scriptFileChanged(QString scriptFile);
    void messageChanged(QString message);
    void lineChanged(int line);
    void commandChanged(QString command);

private:
    QString m_scriptFile;
    QString m_message;
    QString m_command;
    int m_line = 0;
};

#endif // LAMMPSERROR_H
