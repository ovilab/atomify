#ifndef SIMULATIONS_H
#define SIMULATIONS_H
#include "simulation.h"
#include "lennardjonescrystal.h"
#include "lennardjonesdiffusion.h"
#include "bulkwater.h"
#include "gashydrates.h"
#include <QMap>
#include <QString>

class Crack : public Simulation
{
public:
    Crack() { initialize(":/scripts/in.crack", QVector3D(0, 0, 60)); }
};

class FlowPoiseuille : public Simulation
{
public:
    FlowPoiseuille() { initialize(":/scripts/in.flow.poiseuille", QVector3D(0, 0, 20)); }
};

class FlowCouette : public Simulation
{
public:
    FlowCouette() { initialize(":/scripts/in.flow.couette", QVector3D(0, 0, 20)); }
};

class Friction : public Simulation
{
public:
    Friction() { initialize(":/scripts/in.friction", QVector3D(0, 0, 30)); }
};

class Obstacle : public Simulation
{
public:
    Obstacle() { initialize(":/scripts/in.obstacle", QVector3D(0, 0, 25)); }
};

QMap<QString, Simulation*> createSimulationObjects();

#endif
