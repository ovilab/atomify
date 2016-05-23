#include "periodicimages.h"

PeriodicImages::PeriodicImages(Atoms *atoms, System *system) : Modifier(atoms, system)
{

}


void PeriodicImages::apply(AtomData &atomData)
{
}

int PeriodicImages::numberOfCopiesX() const
{
    return m_numberOfCopiesX;
}

int PeriodicImages::numberOfCopiesY() const
{
    return m_numberOfCopiesY;
}

int PeriodicImages::numberOfCopiesZ() const
{
    return m_numberOfCopiesZ;
}

void PeriodicImages::setNumberOfCopiesX(int numberOfCopiesX)
{
    if (m_numberOfCopiesX == numberOfCopiesX)
        return;

    m_numberOfCopiesX = numberOfCopiesX;
    emit numberOfCopiesXChanged(numberOfCopiesX);
}

void PeriodicImages::setNumberOfCopiesY(int numberOfCopiesY)
{
    if (m_numberOfCopiesY == numberOfCopiesY)
        return;

    m_numberOfCopiesY = numberOfCopiesY;
    emit numberOfCopiesYChanged(numberOfCopiesY);
}

void PeriodicImages::setNumberOfCopiesZ(int numberOfCopiesZ)
{
    if (m_numberOfCopiesZ == numberOfCopiesZ)
        return;

    m_numberOfCopiesZ = numberOfCopiesZ;
    emit numberOfCopiesZChanged(numberOfCopiesZ);
}
