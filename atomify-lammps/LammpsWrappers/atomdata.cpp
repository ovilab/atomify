#include "atomdata.h"

bool AtomData::isValid()
{
    return positions.size() == colors.size() && colors.size() == radii.size() && radii.size() == types.size();
}
