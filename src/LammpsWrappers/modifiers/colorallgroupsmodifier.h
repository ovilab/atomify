#ifndef COLORALLGROUPSMODIFIER_H
#define COLORALLGROUPSMODIFIER_H

#include "modifier.h"

#include <QVector>
#include <QVector3D>
#include <QList>
#include <QColor>
#include <QVariantList>

class ColorAllGroupsModifier : public Modifier
{
    Q_OBJECT

public:
    ColorAllGroupsModifier();
    void apply(AtomData &atomData) override;
};

#endif // COLORALLGROUPSMODIFIER_H
