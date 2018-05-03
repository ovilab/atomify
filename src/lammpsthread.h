#ifndef LAMMPSTHREAD_H
#define LAMMPSTHREAD_H

#include "core/data/lammps/lammpsdata.h"
#include "core/kernel/value.h"

#include <QMutex>
#include <QThread>
#include <QVector3D>
#include <QVector>
#include <QSemaphore>

namespace LAMMPS_NS {
class LAMMPS;
}

namespace atomify {

class Cancelled : public std::exception {
};

class LAMMPSThread : public QThread {
    Q_OBJECT
public:
    explicit LAMMPSThread(QObject* parent = nullptr);
    ~LAMMPSThread();

    std::atomic_bool m_atomDataRequested = false;
    std::atomic_bool m_systemDataRequested = false;
    QSemaphore m_dataReady;
    Value<LAMMPSData> m_readyData;

protected:
    void run() override;
    void callback(LAMMPS_NS::LAMMPS* lmp, int mode);

private:
    mutable QMutex m_mutex;
};
}
#endif // LAMMPSTHREAD_H
