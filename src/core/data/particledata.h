#ifndef CORE_DATA_PARTICLEDATA_H
#define CORE_DATA_PARTICLEDATA_H

#include <QVector3D>
#include <QVector>

namespace atomify {

struct ParticleData {
    uint64_t timestep;
    QVector<QVector3D> positions;
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<uint64_t> ids;
    QVector<int> bitmask; // For detecting group membership
    QVector<int> types;
    QVector<bool> visible;
    int size;
};

void resize(ParticleData* data, size_t size);

} // namespace atomify

#endif // CORE_DATA_PARTICLEDATA_H
