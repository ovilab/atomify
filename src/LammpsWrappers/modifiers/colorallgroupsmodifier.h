#ifndef COLORALLGROUPSMODIFIER_H
#define COLORALLGROUPSMODIFIER_H

#include "modifier.h"

#include <QVector>
#include <QVector3D>

class ColorAllGroupsModifier : public Modifier
{
public:
    ColorAllGroupsModifier();
    void apply(AtomData &atomData) override;

private:
    QVector<QVector3D> m_colors;
};

#endif // COLORALLGROUPSMODIFIER_H
