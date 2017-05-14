import QtQuick 2.5
import "qrc:/core"

Simulation {
    name: "Obstacle flow"
    description: "<p>In this simulation, we apply a force on all atoms so they start to flow. We have also added two regions (obstacles) where the atoms feel a repulsive force, so they will flow around them.</p>"
    initialCameraPosition: Qt.vector3d(0,0,35)
    initialUpVector: Qt.vector3d(0,-1,0)
}

