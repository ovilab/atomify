#ifndef SINGLECOMMANDHANDLER_H
#define SINGLECOMMANDHANDLER_H

#include <QObject>
#include <QList>
#include <QString>

class SingleCommandHandler : public QObject
{
    Q_OBJECT
public:
    explicit SingleCommandHandler(QObject *parent = nullptr);
    Q_INVOKABLE QString previousSingleCommandString();
    Q_INVOKABLE QString nextSingleCommandString();
    Q_INVOKABLE QString lastSingleCommandString();
    Q_INVOKABLE void addCommand(QString command);
    QList<QString> &singleCommands();

signals:

public slots:

private:
    QList<QString> m_previousSingleCommands;
    QList<QString> m_singleCommands;
    int m_currentPreviousSingleCommandIndex = 0;
};

#endif // SINGLECOMMANDHANDLER_H
