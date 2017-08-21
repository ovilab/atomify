#include "singlecommandhandler.h"

SingleCommandHandler::SingleCommandHandler(QObject *parent) : QObject(parent)
{

}

QString SingleCommandHandler::previousSingleCommandString()
{
    if(--m_currentPreviousSingleCommandIndex < 0) {
        m_currentPreviousSingleCommandIndex = 0;
    }

    return m_previousSingleCommands.at(m_currentPreviousSingleCommandIndex);
}

QString SingleCommandHandler::nextSingleCommandString()
{
    if(++m_currentPreviousSingleCommandIndex < m_previousSingleCommands.count()) {
        return m_previousSingleCommands.at(m_currentPreviousSingleCommandIndex);
    } else{
        m_currentPreviousSingleCommandIndex = m_previousSingleCommands.count();
        return QString("");
    }
}

QString SingleCommandHandler::lastSingleCommandString()
{
    m_currentPreviousSingleCommandIndex = m_previousSingleCommands.count()-1;
    if(m_previousSingleCommands.count() > 0) {
        return m_previousSingleCommands.last();
    }
    else return QString("");
}

void SingleCommandHandler::addCommand(QString command)
{
    m_previousSingleCommands.push_back(command);
    m_singleCommands.push_back(command);
}

QList<QString> &SingleCommandHandler::singleCommands()
{
    return m_singleCommands;
}
