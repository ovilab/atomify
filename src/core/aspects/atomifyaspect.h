#ifndef LAMMPSASPECT_H
#define LAMMPSASPECT_H

#include "core/data/lammps/lammpsdata.h"
#include "core/data/particledata.h"

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
    QSharedPointer<class LAMMPSControllerMapper> m_mapper;
    QMap<Qt3DCore::QNodeId, QSharedPointer<struct LAMMPSSynchronizationJob>> m_jobs;
};
} // namespace atomify

#endif // LAMMPSASPECT_H
