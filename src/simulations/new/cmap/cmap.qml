import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Polymer"
    description: "<p>This is just a beautiful polymer that probably has a meaning. I think the name is glycylanalylglycylglycine, but not sure :D</p>"
    initialCameraPosition: Qt.vector3d(0.0, 0.0, 10.0)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "Time [fs]"
            xRange: 250
        }
    ]
}

