#ifndef LENNARDJONESDIFFUSION_H
#define LENNARDJONESDIFFUSION_H

#include "simulation.h"

class LennardJonesDiffusion : public Simulation
{
public:
    LennardJonesDiffusion();

    // Simulation interface
protected:
    virtual void setScaleAndColorEvaluator();
};

#endif // LENNARDJONESDIFFUSION_H
