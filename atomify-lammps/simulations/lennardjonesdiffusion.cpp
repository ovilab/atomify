#include "lennardjonesdiffusion.h"

LennardJonesDiffusion::LennardJonesDiffusion()
{
    setScaleAndColorEvaluator();
    initialize(":/scripts/in.lennardjonesdiffusion", QVector3D(0, 0, 25));
}
