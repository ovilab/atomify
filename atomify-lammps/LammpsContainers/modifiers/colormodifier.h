#ifndef COLORMODIFIER_H
#define COLORMODIFIER_H

#include "modifier.h"

class ColorModifier : public Modifier
{
public:
    ColorModifier(Atoms *atoms, System *system);
    virtual void apply(AtomData &atomData) override;
};

#endif // COLORMODIFIER_H
