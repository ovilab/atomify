#pragma once
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QMutex>
#include <QQuickItem>

class Renderable;
class Simulator;

class SimulatorWorker : public QObject {
    Q_OBJECT

private:
    virtual void synchronizeSimulator(Simulator *simulator) = 0;
    virtual void synchronizeRenderer(Renderable *renderableObject) = 0;
    virtual void work() = 0;

    Q_INVOKABLE void workAndUnlock(Simulator *simulator);
signals:
    void workDone();

private:
    friend class Simulator;
    friend class Visualizer;
};

class Simulator : public QQuickItem
{
    Q_OBJECT
public:
    explicit Simulator(QQuickItem *parent = 0);
    ~Simulator();
protected:
    virtual SimulatorWorker *createWorker() = 0;

signals:
    void requestVisualizerSync(SimulatorWorker *worker);

public slots:
    void step();

private:
    SimulatorWorker *m_worker = 0;
    QThread m_workerThread;
    QTimer  m_timer;
    QMutex m_workerMutex;

    friend class Visualizer;
    friend class SimulatorWorker;
};

#endif // SIMULATOR_H
