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
}

int AtomData::size()
{
    return positions.size();
}

AtomData::~AtomData()
{
    positions.clear();
    deltaPositions.clear();
    colors.clear();
    radii.clear();
    types.clear();
    originalIndex.clear();
}
