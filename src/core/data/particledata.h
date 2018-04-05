#ifndef CORE_DATA_PARTICLEDATA_H
#define CORE_DATA_PARTICLEDATA_H

#include <QVector>
#include <QVector3D>

namespace atomify {

struct ParticleData
{
    uint64_t timestep;
    QVector<QVector3D> positions;
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<uint64_t> ids;
    QVector<int> bitmask; // For detecting group membership
    QVector<int> types;
    QVector<bool> visible;
    void resize(int size);
    void reset();
    void sort(QVector3D cameraPosition);
};

}  // namespace atomify

#endif // CORE_DATA_PARTICLEDATA_H
