#ifndef REGIONMODIFIER_H
#define REGIONMODIFIER_H

#include "modifier.h"

class RegionModifier : public Modifier
{
public:
    RegionModifier();

    // Modifier interface
public:
    virtual void apply(class AtomData &atomData) override;
};

#endif // REGIONMODIFIER_H
