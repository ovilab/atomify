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
    occlusion.resize(size);
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
    occlusion.clear();
    originalIndex.clear();
    bitmask.clear();
    visible.clear();
    sortedIndices.clear();
    neighborList.reset(0);
}

void AtomData::sort(QVector3D cameraPosition)
{
    qDebug() << "AtomData::sort does not work yet";
    std::sort(
        std::begin(sortedIndices), std::end(sortedIndices),
        [&](size_t a, size_t b) {
            float dx1 = positions[a][0]-cameraPosition[0];
            float dy1 = positions[a][1]-cameraPosition[1];
            float dz1 = positions[a][2]-cameraPosition[2];

            float dx2 = positions[b][0]-cameraPosition[0];
            float dy2 = positions[b][1]-cameraPosition[1];
            float dz2 = positions[b][2]-cameraPosition[2];

            float dr1 = dx1*dx1 + dy1*dy1 + dz1*dz1;
            float dr2 = dx2*dx2 + dy2*dy2 + dz2*dz2;
            return dr1<dr2;

//            float d1 = (positions[a]-cameraPosition).lengthSquared();
//            float d2 = (positions[b]-cameraPosition).lengthSquared();
//            return (d1<d2);
        }
    );
}

AtomData::~AtomData()
{
    reset();
}
