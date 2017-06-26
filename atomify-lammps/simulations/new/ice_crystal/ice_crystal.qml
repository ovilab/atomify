import QtQuick 2.5
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "Ice crystal"
    description: "<p>This is an ice crystal. Notice how locked the water molecules are.</p>"
    initialCameraPosition: Qt.vector3d(0.0, 0.0, 10.0)
    initialUpVector: Qt.vector3d(0,-1,0)
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "Time [fs]"
            xRange: 1000
        },
        Pressure {
            xLabel: "Time [fs]"
            xRange: 1000
        },
        Thermostat {
            minimumValue: 10
            maximumValue: 500
            temperatureDampening: 25
        }
    ]
}
