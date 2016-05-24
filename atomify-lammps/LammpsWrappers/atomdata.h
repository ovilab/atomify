#ifndef ATOMDATA_H
#define ATOMDATA_H
#include <QVector3D>
#include <QVector>
#include "neighborlist.h"

struct AtomData {
    QVector<QVector3D> positions;
    QVector<QVector3D> deltaPositions; // For periodic copies
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<int> types;
    Neighborlist neighborList;
    bool isValid();
    ~AtomData();
};

#endif // ATOMDATA_H
