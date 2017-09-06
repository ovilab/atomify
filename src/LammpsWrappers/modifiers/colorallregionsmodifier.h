#ifndef COLORALLREGIONSMODIFIER_H
#define COLORALLREGIONSMODIFIER_H

#include "modifier.h"

#include <QVector>
#include <QVector3D>

class ColorAllRegionsModifier : public Modifier
{
    Q_OBJECT
public:
    ColorAllRegionsModifier();
    void apply(AtomData &atomData) override;

private:
    QVector<QVector3D> m_colors;
};

#endif // COLORALLREGIONSMODIFIER_H
