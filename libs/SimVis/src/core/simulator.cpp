#include "simulator.h"
#include <Qt3DCore/QTransform>

Simulator::Simulator(QNode *parent)
    : QNode(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &Simulator::step);
    m_timer.start(1);
}

Simulator::~Simulator()
{
    m_workerThread.quit();
    m_workerThread.wait();
    if(m_worker) {
        delete m_worker;
    }
}

bool Simulator::running() const
{
    return m_timer.isActive();
}

double Simulator::interval() const
{
    return m_timer.interval();
}

void Simulator::step()
{
    if(!m_worker) {
        m_worker = createWorker();
        m_worker->moveToThread(&m_workerThread);
        m_workerThread.start(QThread::TimeCriticalPriority);
    }
    if(m_worker->m_needsSynchronization && m_workerSynchronizeMutex.tryLock()) {
        m_worker->synchronizeSimulator(this);
        m_workerSynchronizeMutex.unlock();
    } else if(m_workerMutex.tryLock()) {
        m_worker->synchronizeSimulator(this);
        QMetaObject::invokeMethod(m_worker, "workAndUnlock",
                                  Qt::QueuedConnection,
                                  Q_ARG(Simulator*, this)); // call happens on worker's thread
    }
}

void Simulator::setRunning(bool running)
{
    if (m_timer.isActive() == running)
        return;

    if(running) {
        m_timer.start();
    } else {
        m_timer.stop();
    }
    emit runningChanged(running);
}

void Simulator::setInterval(double interval)
{
    if (m_timer.interval() == interval)
        return;

    m_timer.setInterval(interval);
    emit intervalChanged(interval);
}

void SimulatorWorker::workAndUnlock(Simulator* simulator)
{
    work();
    simulator->m_workerMutex.unlock();
}
