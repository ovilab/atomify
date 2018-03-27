#include "lammpsaspect.h"
#include "backendlammpscontroller.h"
#include "lammpscontroller2.h"
#include <QAbstractAspect>
namespace atomify {

LAMMPSAspect::LAMMPSAspect(QObject *parent)
    : Qt3DCore::QAbstractAspect(parent)
{
    // Register the mapper to handle creation, lookup, and destruction of backend nodes
    auto mapper = QSharedPointer<LAMMPSControllerMapper>::create(this);
    registerBackendType<LAMMPSController2>(mapper);
}

QVector<Qt3DCore::QAspectJobPtr> LAMMPSAspect::jobsToExecute(qint64 time)
{
//    qDebug() << Q_FUNC_INFO << "Frame time =" << time;
    return {};
}

}

QT3D_REGISTER_NAMESPACED_ASPECT("lammps", QT_PREPEND_NAMESPACE(atomify), LAMMPSAspect)
