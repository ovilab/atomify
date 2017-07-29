#include "atomdata.h"

bool AtomData::isValid()
{
    return positions.size() == colors.size() &&
            colors.size() == radii.size() &&
            radii.size() == types.size() &&
            types.size() == originalIndex.size() &&
            originalIndex.size() == visible.size() &&
            visible.size() == occlusion.size();
}

void AtomData::resize(int size)
{
    positions.resize(size);
    deltaPositions.resize(size);
    colors.resize(size);
    radii.resize(size);
    types.resize(size);
    originalIndex.resize(size);
    visible.resize(size);
    occlusion.resize(size);
}

int AtomData::size()
{
    return positions.size();
}

long AtomData::memoryUsage()
{
    return (positions.capacity() + deltaPositions.capacity() + colors.capacity())*sizeof(QVector3D)
            +(radii.capacity() + occlusion.capacity())*sizeof(float)
            +(originalIndex.capacity() + types.capacity())*sizeof(int)
            +visible.capacity()*sizeof(bool);
}

void AtomData::reset()
{
    positions.clear();
    deltaPositions.clear();
    colors.clear();
    radii.clear();
    types.clear();
    originalIndex.clear();
    visible.clear();
    occlusion.clear();
    neighborList.reset(0);
}

AtomData::~AtomData()
{
    reset();
}
