import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Abstract translocation"
    description: "<p>In this simulation we see a polymer that eventually goes through the hole. It can take quite a lot of time, but the wait might be worth it!</p>"
    initialCameraPosition: Qt.vector3d(0, 30, 0)
    initialUpVector: Qt.vector3d(0,0,1)
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

