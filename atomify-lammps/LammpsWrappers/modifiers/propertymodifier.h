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
private:
    void applyColors(AtomData &atomData, const std::vector<double> &values, int groupBit);
};

#endif // PROPERTYMODIFIER_H
