#ifndef ATOMDATA_H
#define ATOMDATA_H
#include <QVector3D>
#include <QVector>
#include "neighborlist.h"

class AtomData {
public:
    QVector<QVector3D> positions;
    QVector<QVector3D> deltaPositions; // For periodic copies
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<int> originalIndex;
    QVector<int> bitmask; // For detecting group membership
    QVector<int> types;
    Neighborlist neighborList;
    bool isValid();
    void resize(int size);
    int size();
    void reset();
    ~AtomData();
};

#endif // ATOMDATA_H
