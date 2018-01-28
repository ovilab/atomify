#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <QObject>

class Performance : public QObject
{
    Q_OBJECT
    Q_PROPERTY(long memoryLAMMPS READ memoryLAMMPS WRITE setMemoryLAMMPS NOTIFY memoryLAMMPSChanged)
    Q_PROPERTY(long memoryAtomify READ memoryAtomify WRITE setMemoryAtomify NOTIFY memoryAtomifyChanged)
    Q_PROPERTY(double timestepsPerSecond READ timestepsPerSecond WRITE setTimestepsPerSecond NOTIFY timestepsPerSecondChanged)
    Q_PROPERTY(double atomTimestepsPerSecond READ atomTimestepsPerSecond WRITE setAtomTimestepsPerSecond NOTIFY atomTimestepsPerSecondChanged)
    Q_PROPERTY(int threads READ threads WRITE setThreads NOTIFY threadsChanged)
public:
    explicit Performance(QObject *parent = 0);
    void reset();
    void synchronize(class LAMMPSController *controller);
    long memoryLAMMPS() const;
    long memoryAtomify() const;
    double timestepsPerSecond() const;
    int threads() const;
    double atomTimestepsPerSecond() const;

signals:
    void memoryLAMMPSChanged(long memoryLAMMPS);
    void memoryAtomifyChanged(long memoryAtomify);
    void timestepsPerSecondChanged(double timestepsPerSecond);
    void threadsChanged(int threads);
    void atomTimestepsPerSecondChanged(double atomTimestepsPerSecond);

public slots:
    void setMemoryLAMMPS(long memoryLAMMPS);
    void setMemoryAtomify(long memoryAtomify);
    void setTimestepsPerSecond(double timestepsPerSecond);
    void setThreads(int threads);
    void setAtomTimestepsPerSecond(double atomTimestepsPerSecond);

private:
    long m_memoryLAMMPS = 0;
    long m_memoryAtomify = 0;
    double m_timestepsPerSecond = 0;
    int m_threads = 1;
    double m_atomTimestepsPerSecond = 0;
};

#endif // PERFORMANCE_H
