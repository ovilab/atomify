#ifndef GROUPMODIFIER_H
#define GROUPMODIFIER_H

#include "modifier.h"

class GroupModifier : public Modifier
{
public:
    GroupModifier();

    // Modifier interface
public:
    virtual void apply(class AtomData &atomData) override;
};

#endif // GROUPMODIFIER_H
