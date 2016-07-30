#include "atomdata.h"

bool AtomData::isValid()
{
    return positions.size() == colors.size() &&
            colors.size() == radii.size() &&
            radii.size() == types.size() &&
            types.size() == originalIndex.size() &&
            originalIndex.size() == bitmask.size() &&
            bitmask.size() == visible.size() &&
            visible.size() == sortedIndices.size();
}

void AtomData::resize(int size)
{
    positions.resize(size);
    deltaPositions.resize(size);
    colors.resize(size);
    radii.resize(size);
    types.resize(size);
    originalIndex.resize(size);
    // neighborList.reset(size);
    bitmask.resize(size);
    visible.resize(size);
    sortedIndices.resize(size);
}

int AtomData::size()
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
    sortedIndices.clear();
    neighborList.reset(0);
}

AtomData::~AtomData()
{
    reset();
}
