#ifndef BULKWATER_H
#define BULKWATER_H
#include "simulation.h"

class BulkWater : public Simulation
{
public:
    BulkWater();

    // Simulation interface
protected:
    virtual void setScaleAndColorEvaluator();
};

#endif // BULKWATER_H
