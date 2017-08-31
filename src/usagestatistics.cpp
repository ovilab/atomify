#include "usagestatistics.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QSysInfo>

using namespace AtomifyStatistics;

Run::Run(int id, int sessionRunId, int sessionId, QString uuid, QString hash) :
    m_id(id), m_sessionId(sessionId), m_sessionRunId(sessionRunId), m_numAtoms(0), m_numThreads(1), m_numTimesteps(0), m_crashed(false), m_uuid(uuid), m_hash(hash)
{
    m_os = QSysInfo::prettyProductName();
    m_atomifyVersion = QString("%1_%2").arg(ATOMIFYVERSION).arg(BUILDVERSION);
    qDebug() << "Version: " << m_atomifyVersion;
    m_dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

void Run::fromJSON(const QJsonObject &json)
{
    m_id = json["id"].toInt();
    m_crashed = json["crashed"].toBool();
    m_pairStyle = json["pair_style"].toString();
    m_hash = json["hash"].toString();
    m_uuid = json["uuid"].toString();
    m_sessionId = json["sessionid"].toInt();
    m_sessionRunId = json["sessionrunid"].toInt();
    m_atomifyVersion = json["version"].toString();
    m_dateTime = json["datetime"].toString();
    m_os = json["os"].toString();
    m_numThreads = json["numthreads"].toInt();
    m_numAtoms = json["numatoms"].toInt();
    m_numTimesteps = json["numtimesteps"].toInt();
}

QJsonObject Run::toJSON()
{
    QJsonObject json;
    json["id"] = m_id;
    json["crashed"] = int(m_crashed);
    json["pair_style"] = m_pairStyle;
    json["uuid"] = m_uuid;
    json["sessionid"] = m_sessionId;
    json["sessionrunid"] = m_sessionRunId;
    json["hash"] = m_hash;
    json["datetime"] = m_dateTime;
    json["version"] = m_atomifyVersion;
    json["os"] = m_os;
    json["numthreads"] = m_numThreads;
    json["numatoms"] = m_numAtoms;
    json["numtimesteps"] = m_numTimesteps;

    return json;
}

UsageStatistics::UsageStatistics(QObject *parent) : QObject(parent)
{
    m_runsKey = "statistics/runs";
    m_nextRunIDKey = "statistics/nextRunID";
    m_nextSessionIDKey = "statistics/nextSessionID";
    m_sendUsageStatisticsKey = "statistics/sendUsageStatistics";

    m_sendUsageStatistics = true;
    if(!m_settings.value(m_sendUsageStatisticsKey).isNull()) {
        m_sendUsageStatistics = m_settings.value(m_sendUsageStatisticsKey).toBool();
        m_settings.setValue(m_sendUsageStatisticsKey, m_sendUsageStatistics);
    }

    m_sessionID = 0;
    if(!m_settings.value(m_nextSessionIDKey).isNull()) {
        m_sessionID = m_settings.value(m_nextSessionIDKey).toInt();
    }
    m_settings.setValue(m_nextSessionIDKey, (m_sessionID+1) );

    m_nextRunId = 0;
    if(!m_settings.value(m_nextRunIDKey).isNull()) {
        m_nextRunId = m_settings.value(m_nextRunIDKey).toInt();
    }

    m_currentRun = nullptr;
    m_sessionRunID = 0;
    load();
    send();
}

UsageStatistics::~UsageStatistics()
{
    if(m_currentRun) {
        m_runs.push_back(m_currentRun);

        m_currentRun = nullptr;
    }
    save();
}

void UsageStatistics::newRun()
{
    if(m_currentRun) {
        m_runs.push_back(m_currentRun);
        m_currentRun = nullptr;
    }
    send();

    m_sessionRunID++;
    m_nextRunId++;
    m_settings.setValue(m_nextRunIDKey, (m_nextRunId+1) );

    QString uuid = m_settings.value("machine/uuid").toString();
    const QByteArray keyToBeHashed = QString("%1-%2-%3-%4").arg(m_nextRunId).arg(m_sessionRunID).arg(m_sessionID).arg(uuid).toUtf8();
    QString hashed = QString(QCryptographicHash::hash(keyToBeHashed,QCryptographicHash::Md5).toHex());

    m_currentRun = new Run(m_nextRunId,m_sessionRunID,m_sessionID, uuid, hashed);
}

bool UsageStatistics::sendUsageStatistics() const
{
    return m_sendUsageStatistics;
}

Run *UsageStatistics::currentRun()
{
    return m_currentRun;
}

void UsageStatistics::onfinish(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        // qDebug() << "Response: " << response;
        QJsonDocument doc(QJsonDocument::fromJson(response));
        if(!doc.isNull()) {
            QJsonObject obj = doc.object();
            if(obj["status"].toBool() == true) {
                m_runs.clear();
            }
        }
    }
}

void UsageStatistics::setSendUsageStatistics(bool sendUsageStatistics)
{
    if (m_sendUsageStatistics == sendUsageStatistics)
        return;

    m_sendUsageStatistics = sendUsageStatistics;
    m_settings.setValue(m_sendUsageStatisticsKey, m_sendUsageStatistics);
    emit sendUsageStatisticsChanged(m_sendUsageStatistics);
}

void UsageStatistics::send()
{
    if(!m_sendUsageStatistics || !m_runs.size()) return;

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(onfinish(QNetworkReply*)));
    connect(mgr, SIGNAL(finished(QNetworkReply*)), mgr, SLOT(deleteLater()));

    // Build your JSON string as usual
    QByteArray jsonString = runsAsJSON();

    // For your "Content-Length" header
    QByteArray postDataSize = QByteArray::number(jsonString.size());

    // Time for building your request
    QUrl serviceURL("http://kvakkefly.com/submit.php");
    QNetworkRequest request(serviceURL);

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    request.setRawHeader("User-Agent", "Atomify");
    request.setRawHeader("X-Custom-User-Agent", "Atomify");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Content-Length", postDataSize);

    mgr->post(request, jsonString);
}

void UsageStatistics::load()
{
    if(m_settings.value(m_runsKey).isNull()) return;
    QByteArray runsData = m_settings.value(m_runsKey).toByteArray();
    QJsonDocument loadDoc(QJsonDocument::fromJson(runsData));
    QJsonObject runsObject = loadDoc.object();
    QJsonArray runs = runsObject["runs"].toArray();
    for(auto iter=runs.begin(); iter!=runs.end(); iter++) {
        QJsonObject runObject = (*iter).toObject();
        Run *run = new Run();
        run->fromJSON(runObject);
        m_runs.push_back(run);
    }
    // Cap stored runs to 100
    while(m_runs.size()>100) {
        m_runs.pop_front();
    }
}

QByteArray UsageStatistics::runsAsJSON()
{
    QJsonArray runs;

    for(Run *run : m_runs) {
        runs.push_back(run->toJSON());
    }
    QJsonObject data;
    QString uuid = m_settings.value("machine/uuid").toString();
    data["runs"] = runs;
    data["uuid"] = uuid;
    data["sessions"] = m_sessionID;
    data["runCount"] = m_nextRunId-1;
    QJsonDocument json(data);
    return json.toJson(QJsonDocument::JsonFormat::Compact);
}

void UsageStatistics::save()
{
    m_settings.setValue(m_runsKey, runsAsJSON());
}
