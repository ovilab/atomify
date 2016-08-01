#ifndef ATOMDATA_H
#define ATOMDATA_H
#include <QVector3D>
#include <QVector>
#include "neighborlist.h"

class AtomData {
public:
    QVector<int> sortedIndices;
    QVector<QVector3D> positions;
    QVector<QVector3D> deltaPositions; // For periodic copies
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<float> occlusion;
    QVector<int> originalIndex;
    QVector<int> bitmask; // For detecting group membership
    QVector<int> types;
    QVector<bool> visible;
    Neighborlist neighborList;
    bool isValid();
    void resize(int size);
    int size();
    void reset();
    void sort(QVector3D cameraPosition);
    ~AtomData();
};

#endif // ATOMDATA_H
