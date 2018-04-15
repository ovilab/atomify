#ifndef LAMMPSASPECT_H
#define LAMMPSASPECT_H

#include "core/controllers/backendlammpscontroller.h"
#include "core/data/lammps/lammpsdata.h"
#include "core/data/particledata.h"
#include "core/mapper.h"
#include "core/visualization/backendatomify.h"

#include <QAbstractAspect>
#include <QThread>
#include <QVector>

namespace atomify {

class AtomifyAspect : public Qt3DCore::QAbstractAspect {
    Q_OBJECT
public:
    explicit AtomifyAspect(QObject* parent = nullptr);

protected:
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;
    QSharedPointer<Mapper<BackendLAMMPSController>> m_lammpsMapper;
    QSharedPointer<Mapper<BackendAtomify>> m_atomifyMapper;
    //    QMap<Qt3DCore::QNodeId, QSharedPointer<struct AtomifySynchronizationJob>> m_jobs;
};
} // namespace atomify

#endif // LAMMPSASPECT_H
