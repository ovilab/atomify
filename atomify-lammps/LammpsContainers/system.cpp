#include "system.h"
#include <domain.h>
using namespace LAMMPS_NS;

System::System()
{

}

void System::synchronize(LAMMPS *lammps)
{
    Domain *domain = lammps->domain;
    for(int i=0; i<3; i++) {
        m_origin[i] = domain->boxlo[i];
        m_size[i] = domain->prd[i];
    }
}

QVector3D System::origin() const
{
    return m_origin;
}

QVector3D System::size() const
{
    return m_size;
}

void System::setOrigin(QVector3D origin)
{
    if (m_origin == origin)
        return;

    m_origin = origin;
    emit originChanged(origin);
}

void System::setSize(QVector3D size)
{
    if (m_size == size)
        return;

    m_size = size;
    emit sizeChanged(size);
}
