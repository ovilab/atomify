#ifndef USAGESTATISTICS_H
#define USAGESTATISTICS_H

#include <QJsonObject>
#include <QNetworkReply>
#include <QObject>
#include <QSettings>

namespace AtomifyStatistics
{

class Run {
public:
    Run(int id = -1, int sessionRunId=-1, int sessionId=-1, QString uuid = "", QString hash = "");
    void fromJSON(const QJsonObject &json);
    QJsonObject toJSON();
    int m_id;
    int m_sessionId;
    int m_sessionRunId;
    int m_numAtoms;
    int m_numThreads;
    int m_numTimesteps;
    bool m_crashed;
    QString m_pairStyle;
    QString m_uuid;
    QString m_hash;
    QString m_atomifyVersion;
    QString m_os;
    QString m_dateTime;
};

}

class UsageStatistics : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool sendUsageStatistics READ sendUsageStatistics WRITE setSendUsageStatistics NOTIFY sendUsageStatisticsChanged)
public:
    explicit UsageStatistics(QObject *parent = nullptr);
    ~UsageStatistics();
    AtomifyStatistics::Run *currentRun();
    QByteArray runsAsJSON();
    void newRun();
    bool sendUsageStatistics() const;

signals:
    void sendUsageStatisticsChanged(bool sendUsageStatistics);

public slots:
    void onfinish(QNetworkReply *reply);
    void setSendUsageStatistics(bool sendUsageStatistics);

private:
    void send();
    void load();
    void save();
    QString m_runsKey;
    QString m_nextRunIDKey;
    QString m_nextSessionIDKey;
    QString m_sendUsageStatisticsKey;
    QSettings m_settings;
    int m_sessionID;
    int m_nextRunId;
    int m_sessionRunID;
    AtomifyStatistics::Run *m_currentRun;
    QList<AtomifyStatistics::Run*> m_runs;
    bool m_sendUsageStatistics;
};

#endif // USAGESTATISTICS_H
