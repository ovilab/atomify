#ifndef NEIGHBORLIST_H
#define NEIGHBORLIST_H
#include <QVector>

struct Neighborlist {
    QVector<int> numNeighbors;
    QVector<QVector<int>> neighbors;
    Neighborlist();
    Neighborlist(const Neighborlist &old);
};

#endif // NEIGHBORLIST_H
