#include "simulations.h"

QMap<QString, Simulation*> createSimulationObjects() {
    QMap<QString, Simulation*> simulations;
    simulations["lennardjonescrystal"] = new LennardJonesCrystal();
    simulations["lennardjonesdiffusion"] = new LennardJonesDiffusion();
    simulations["bulkwater"] = new BulkWater();
    simulations["gashydrates"] = new GasHydrates();
    simulations["crack"] = new Crack();
    simulations["flowcouette"] = new FlowCouette();
    simulations["flowpoiseuille"] = new FlowPoiseuille();
    simulations["friction"] = new Friction();
    simulations["obstacle"] = new Obstacle();

    return simulations;
}
