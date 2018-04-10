#ifndef LAMMPSTHREAD_H
#define LAMMPSTHREAD_H

#include "core/data/lammps/lammpsdata.h"

#include <memory>

#include <QMutex>
#include <QThread>
#include <QVector3D>
#include <QVector>

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
    std::shared_ptr<const LAMMPSData> data();
    bool dataDirty() const;

protected:
    void run() override;
    void callback(LAMMPS_NS::LAMMPS* lmp, int mode);

private:
//    mutable QMutex m_mutex;
    std::shared_ptr<LAMMPSData> m_data;
    std::shared_ptr<const LAMMPSData> m_latestData;
    bool m_dataDirty;
};
}
#endif // LAMMPSTHREAD_H
