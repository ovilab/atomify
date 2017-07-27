import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Polymer collapse"
    description: "<p>This is a hydrocarbon polymer that is first stretched out, but after not too long, it collapses.</p>"
    initialCameraPosition: Qt.vector3d(0.0, 0.0, 10.0)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        }
    ]
}

