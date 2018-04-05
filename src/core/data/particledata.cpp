#include "particledata.h"

namespace atomify {

void ParticleData::resize(int size)
{
    positions.resize(size);
    colors.resize(size);
    radii.resize(size);
    ids.resize(size);
    bitmask.resize(size);
    types.resize(size);
    visible.resize(size);
}

void ParticleData::reset()
{
    resize(0);
}

}  // namespace atomify
