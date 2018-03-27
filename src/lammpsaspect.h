#ifndef LAMMPSASPECT_H
#define LAMMPSASPECT_H

#include <QAbstractAspect>
#include <QThread>

namespace atomify {

class LAMMPSAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    explicit LAMMPSAspect(QObject *parent = nullptr);

protected:
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;
    QSharedPointer<class LAMMPSControllerMapper> m_mapper;
};
}  // namespace atomify

#endif // LAMMPSASPECT_H
