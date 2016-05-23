#include "atomdata.h"

bool AtomData::isValid()
{
    return positions.size() == colors.size() && colors.size() == radii.size() && radii.size() == types.size();
}

AtomData::~AtomData()
{
    positions.clear();
    colors.clear();
    radii.clear();
    types.clear();
}
