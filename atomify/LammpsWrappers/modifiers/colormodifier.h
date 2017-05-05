#ifndef COLORMODIFIER_H
#define COLORMODIFIER_H

#include "modifier.h"

class ColorModifier : public Modifier
{
    Q_OBJECT
public:
    ColorModifier();
    virtual void apply(AtomData &atomData) override;
private:
    float m_scale = 0.3;
};

#endif // COLORMODIFIER_H
