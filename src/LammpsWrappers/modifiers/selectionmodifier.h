#ifndef SELECTIONMODIFIER_H
#define SELECTIONMODIFIER_H

#include "modifier.h"

class SelectionModifier : public Modifier
{
    Q_OBJECT

public:
    SelectionModifier();
    void apply(AtomData &atomData) override;
};

#endif // SELECTIONMODIFIER_H
