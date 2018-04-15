#ifndef CORE_MAPPER_H
#define CORE_MAPPER_H

#include <QBackendNode>

namespace atomify {

template <typename T>
class Mapper : public Qt3DCore::QBackendNodeMapper {
public:
    // explicit Mapper(class AtomifyAspect* aspect);
    Mapper(class AtomifyAspect* aspect);
    Qt3DCore::QBackendNode* create(const Qt3DCore::QNodeCreatedChangeBasePtr& change) const override;
    Qt3DCore::QBackendNode* get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;
    T* controller();

private:
    mutable T* m_controller;
};

template <typename T>
Mapper<T>::Mapper(AtomifyAspect* aspect)
    : m_controller(nullptr)
{
}

template <typename T>
Qt3DCore::QBackendNode* Mapper<T>::create(const Qt3DCore::QNodeCreatedChangeBasePtr& change) const
{
    m_controller = new T;
    return m_controller;
}

template <typename T>
Qt3DCore::QBackendNode* Mapper<T>::get(Qt3DCore::QNodeId id) const
{
    if (m_controller && m_controller->peerId() != id)
        throw "Tried to request a node with id different from the backend node";
    return m_controller;
}

template <typename T>
void Mapper<T>::destroy(Qt3DCore::QNodeId id) const
{
    delete m_controller;
    m_controller = nullptr;
}

template <typename T>
T* Mapper<T>::controller()
{
    return m_controller;
}
}
#endif // CORE_MAPPER_H
