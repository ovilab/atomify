#include "atomdata.h"
#include <QDebug>
bool AtomData::isValid()
{
    return positions.size() == colors.size() &&
            colors.size() == radii.size() &&
            radii.size() == types.size() &&
            types.size() == originalIndex.size() &&
            originalIndex.size() == bitmask.size() &&
            bitmask.size() == visible.size() &&
            visible.size() == lammpsParticleId.size();
}

void AtomData::resize(int size)
{
    positions.resize(size);
    deltaPositions.resize(size);
    colors.resize(size);
    radii.resize(size);
    types.resize(size);
    originalIndex.resize(size);
    bitmask.resize(size);
    visible.resize(size);
    lammpsParticleId.resize(size);
}

int AtomData::size() const
{
    return positions.size();
}

void AtomData::reset()
{
    positions.clear();
    deltaPositions.clear();
    colors.clear();
    radii.clear();
    types.clear();
    originalIndex.clear();
    bitmask.clear();
    visible.clear();
    lammpsParticleId.clear();
}

long AtomData::memoryUsage()
{
    return (positions.capacity() + deltaPositions.capacity() + colors.capacity())*sizeof(QVector3D)
            +(radii.capacity())*sizeof(float)
            +(originalIndex.capacity() + types.capacity() + bitmask.capacity() + lammpsParticleId.capacity())*sizeof(int)
            +visible.capacity()*sizeof(bool);
}

AtomData::~AtomData()
{
    reset();
}
