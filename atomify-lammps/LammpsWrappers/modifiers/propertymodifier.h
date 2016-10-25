#ifndef PROPERTYMODIFIER_H
#define PROPERTYMODIFIER_H

#include "modifier.h"

class PropertyModifier : public Modifier
{
public:
    PropertyModifier();

    // Modifier interface
public:
    virtual void apply(class AtomData &atomData) override;
};

#endif // PROPERTYMODIFIER_H
