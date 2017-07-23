import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "LAMMPS logo"
    description: "<p>This 2D simulation of the LAMMPS logo will first dissolve the logo, but then when velocities are reversed, the logo is reconstructed.</p>"
    initialCameraPosition: Qt.vector3d(0.0, 0.0, -45.0)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        }
    ]
}

