import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
import "qrc:/mobile/dashboard"

Simulation {
    id: root
    name: "Faceted cylinder"
    description: "Faceted cylinder"
    longDescription: ""
    initialCameraPosition: Qt.vector3d(0,0,46)
    orthographic: false
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "Time [fs]"
            xScale: 1000
            xRange: 1000
        },
        Pressure {
            xLabel: "Time [fs]"
            xScale: 1000
            xRange: 1000
        }
    ]
}
