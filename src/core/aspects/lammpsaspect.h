#ifndef LAMMPSASPECT_H
#define LAMMPSASPECT_H

#include "core/data/lammps/lammpsdata.h"
#include "core/data/particledata.h"

#include <QAbstractAspect>
#include <QThread>
#include <QVector>

namespace atomify {

class LAMMPSAspect : public Qt3DCore::QAbstractAspect {
    Q_OBJECT
public:
    explicit LAMMPSAspect(QObject* parent = nullptr);

protected:
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;
    QSharedPointer<class LAMMPSControllerMapper> m_mapper;
    QMap<Qt3DCore::QNodeId, LAMMPSData> m_rawData;
    //    LAMMPSData m_rawData;
    QMap<Qt3DCore::QNodeId, ParticleData> m_particleData;
    QMap<Qt3DCore::QNodeId, QByteArray> m_sphereBufferData;
};
} // namespace atomify

#endif // LAMMPSASPECT_H
