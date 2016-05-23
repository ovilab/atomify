#ifndef ATOMDATA_H
#define ATOMDATA_H
#include <QVector3D>
#include <QVector>

struct AtomData {
    QVector<QVector3D> positions;
    QVector<QVector3D> colors;
    QVector<float> radii;
    QVector<int> types;
    bool isValid();
};

#endif // ATOMDATA_H
