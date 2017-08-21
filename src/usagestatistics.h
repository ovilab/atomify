#ifndef USAGESTATISTICS_H
#define USAGESTATISTICS_H

#include <QObject>

class UsageStatistics : public QObject
{
    Q_OBJECT
public:
    explicit UsageStatistics(QObject *parent = nullptr);

signals:

public slots:
};

#endif // USAGESTATISTICS_H