#include "particledata.h"

namespace atomify {

ParticleData resize(ParticleData data, size_t size)
{
    int sizeint = int(size);
    data.positions.resize(sizeint);
    data.colors.resize(sizeint);
    data.radii.resize(sizeint);
    data.ids.resize(sizeint);
    data.types.resize(sizeint);
    data.visible.resize(sizeint);
    data.size = sizeint;
    return data;
}

} // namespace atomify
