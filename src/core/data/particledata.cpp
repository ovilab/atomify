#include "particledata.h"

namespace atomify {

void resize(ParticleData *data, size_t size)
{
    data->positions.resize(size);
    data->colors.resize(size);
    data->radii.resize(size);
    data->ids.resize(size);
    data->bitmask.resize(size);
    data->types.resize(size);
    data->visible.resize(size);
    data->size = size;
}

}  // namespace atomify
