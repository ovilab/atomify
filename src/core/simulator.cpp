#include "simulator.h"

Simulator::Simulator(QQuickItem *parent) : QQuickItem(parent)
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

void Simulator::step()
{
    if(!m_worker) {
        m_worker = createWorker();
        m_worker->moveToThread(&m_workerThread);
        m_workerThread.start(QThread::TimeCriticalPriority);
    }
    if(m_workerMutex.tryLock()) {
        m_worker->synchronizeSimulator(this);
        emit requestVisualizerSync(m_worker);
        QMetaObject::invokeMethod(m_worker, "workAndUnlock",
                                  Qt::QueuedConnection,
                                  Q_ARG(Simulator*, this)); // call happens on worker's thread
    }
}

void SimulatorWorker::workAndUnlock(Simulator* simulator)
{
    work();
    simulator->m_workerMutex.unlock();
}
