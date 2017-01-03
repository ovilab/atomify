#pragma once
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QMutex>
#include <QQuickItem>

#include <Qt3DRender/QBuffer>
#include <Qt3DCore/QEntity>

class SimulatorWorker : public QObject {
    Q_OBJECT

protected:
    virtual void work() = 0;

private:
    virtual void synchronizeSimulator(class Simulator *simulator) = 0;
    Q_INVOKABLE void workAndUnlock(class Simulator *simulator);
signals:
    void workDone();
protected:
    bool m_needsSynchronization = false;

private:
    friend class Simulator;
};

class Simulator : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(double interval READ interval WRITE setInterval NOTIFY intervalChanged)

public:
    explicit Simulator(QNode *parent = 0);
    virtual ~Simulator();

    bool running() const;
    double interval() const;

public slots:
    void step();
    void setRunning(bool running);
    void setInterval(double interval);

signals:
    void runningChanged(bool running);
    void intervalChanged(double interval);

protected:
    virtual SimulatorWorker *createWorker() = 0;

private:
    SimulatorWorker *m_worker = 0;
    QThread m_workerThread;
    QTimer  m_timer;
    QMutex m_workerMutex;
    QMutex m_workerSynchronizeMutex;

    friend class SimulatorWorker;
};

#endif // SIMULATOR_H
