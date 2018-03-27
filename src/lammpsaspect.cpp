#include "lammpsaspect.h"
#include "backendlammpscontroller.h"
#include "lammpscontroller2.h"
#include <QAbstractAspect>
#include <QAspectJob>
namespace atomify {

LAMMPSAspect::LAMMPSAspect(QObject *parent)
    : Qt3DCore::QAbstractAspect(parent)
{
    // Register the mapper to handle creation, lookup, and destruction of backend nodes
    m_mapper = QSharedPointer<LAMMPSControllerMapper>::create(this);
    registerBackendType<LAMMPSController2>(m_mapper);
}

QVector<Qt3DCore::QAspectJobPtr> LAMMPSAspect::jobsToExecute(qint64 time)
{
    class LambdaJob : public Qt3DCore::QAspectJob {
    public:
        LambdaJob(std::function<void()> callable) : m_callable(callable) { }

    private:
        const std::function<void()> m_callable;
        void run() override { m_callable(); }
    };
    using LambdaJobPtr = QSharedPointer<LambdaJob>;

    auto job1 = LambdaJobPtr::create([&]() {
        for (const auto &controller : m_mapper->controllers()) {
            controller->synchronize();
        }
    });
    return {job1};
}

}

QT3D_REGISTER_NAMESPACED_ASPECT("lammps", QT_PREPEND_NAMESPACE(atomify), LAMMPSAspect)
