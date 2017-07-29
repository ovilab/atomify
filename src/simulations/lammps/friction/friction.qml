import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"
Simulation {
    id: root
    name: "Friction"
    description: "<p>In this simulation we see friction between two asperities. This illustrates how friction works at the microscopic level.</p>
<p><b>Things to look for</b></p>
<p>See how the asperities are damaged after contact. Watch the pressure curve when the asperities make contact.</p>
"
    initialCameraPosition: Qt.vector3d(0,0,35)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "Time"
            yScale: 1000
        },
        Pressure {
            xLabel: "Time"
        }
    ]
}

