#include "functionsurfacesimulator.h"
#include <cmath>

MyWorker::MyWorker() {

}


SimulatorWorker *MySimulator::createWorker() {
    return new MyWorker();
}

void MyWorker::synchronizeSimulator(Simulator *simulator) {

}

void MyWorker::work() {

}

void MyWorker::synchronizeRenderer(Renderable *renderable)
{

}
