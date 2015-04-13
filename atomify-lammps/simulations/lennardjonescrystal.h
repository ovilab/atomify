#ifndef LENNARDJONESCRYSTAL_H
#define LENNARDJONESCRYSTAL_H
#include "simulation.h"

class LennardJonesCrystal : public Simulation
{
public:
    LennardJonesCrystal();

    // Simulation interface
protected:
    virtual void setScaleAndColorEvaluator() override;
};

#endif // LENNARDJONESCRYSTAL_H
