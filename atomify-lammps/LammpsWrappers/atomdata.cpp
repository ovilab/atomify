#include "atomdata.h"

bool AtomData::isValid()
{
    return positions.size() == colors.size() && colors.size() == radii.size() && radii.size() == types.size();
}

void AtomData::resize(int size)
{
    positions.resize(size);
    deltaPositions.resize(size);
    colors.resize(size);
    radii.resize(size);
    types.resize(size);
    originalIndex.resize(size);
    neighborList.reset(size);
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
    neighborList.reset(0);
}

AtomData::~AtomData()
{
    reset();
}
