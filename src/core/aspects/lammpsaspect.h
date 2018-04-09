#ifndef LAMMPSASPECT_H
#define LAMMPSASPECT_H

#include "core/data/particledata.h"
#include "core/data/lammps/lammpsdata.h"

#include <QAbstractAspect>
#include <QThread>
#include <QVector>

namespace atomify {

class LAMMPSAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    explicit LAMMPSAspect(QObject *parent = nullptr);

protected:
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;
    QSharedPointer<class LAMMPSControllerMapper> m_mapper;
    QMap<Qt3DCore::QNodeId, QPair<bool, LAMMPSData>> m_pendingRawData;
    QMap<Qt3DCore::QNodeId, QPair<bool, ParticleData>> m_pendingParticleData;
    QMap<Qt3DCore::QNodeId, QPair<bool, QByteArray>> m_sphereBufferData;
};
}  // namespace atomify

#endif // LAMMPSASPECT_H
